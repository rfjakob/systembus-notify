/* SPDX-License-Identifier: MIT */

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>
#include <unistd.h>

#include "log.h"
#include "notify.h"

static sd_bus* user_bus = NULL;

// Did we alread send a notification this decisecond (tenth of a second)?
bool over_ratelimit() {
    static long decisec_last = 0;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long decisec_now = now.tv_sec*10+now.tv_nsec/100000000;

    // Decisec has rolled over?
    if (decisec_last != decisec_now) {
        decisec_last = decisec_now;
        return false;
    }

    return true;
}

// handle_dbus_signal is called when a d-bus "Notify" signal is received.
int handle_dbus_signal(sd_bus_message* m, void* userdata, sd_bus_error* ret_error)
{
    // unused
    (void)userdata;
    (void)ret_error;

    // summary is optional. It stays NULL when the signal does not contain
    // a string value.
    const char* summary = NULL;
    sd_bus_message_read_basic(m, 's', &summary);
    // body is optional. It stays NULL when the signal does not contain
    // a second string value.
    const char* body = NULL;
    sd_bus_message_read_basic(m, 's', &body);
    debug("received d-bus signal on system bus: summary=%s body=%s\n", summary, body);
    if(over_ratelimit()) {
        fprintf(stderr, "over ratelimit, dropping message\n");
        return 0;
    }
    notify(user_bus, summary, body);
    return 0;
}

int main(int argc, char* argv[])
{
    // Don't buffer our debug output to prevent interleaving
    // with stderr.
    setvbuf(stdout, NULL, _IONBF, 0);
    setlinebuf(stdout);
    // Parse command line
    int c = 0;
    const char* short_opt = "hq";
    struct option long_opt[] = {
        { "help", no_argument, NULL, 'h' },
        { 0, 0, NULL, 0 } /* end-of-array marker */
    };
    while ((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch (c) {
        case -1: /* no more arguments */
        case 0: /* long option toggles */
            break;
        case 'q':
            quiet = 1;
            break;
        case 'h':
            fprintf(stderr, "Usage: %s [-q] [-h]\n", argv[0]);
            fprintf(stderr, "  -q ........... quiet (show errors only)\n");
            fprintf(stderr, "  -h, --help ... this help text\n");
            exit(0);
            break;
        case '?':
        default:
            // getopt already printed out something like this:
            //   ./systembus-notify: unrecognized option '--asdfasdf'
            fprintf(stderr, "Try '--help' for more information.\n");
            exit(FATAL_USAGE);
        }
    }
    if (optind < argc) {
        fprintf(stderr, "Extra argument not understood: '%s'\n", argv[optind]);
        exit(FATAL_USAGE);
    }

    // Connect to D-Buses
    while(1) {
        debug("connecting to d-bus user   bus: ");
        int ret = sd_bus_default_user(&user_bus);
        if(ret >= 0) {
            const char* a = NULL;
            sd_bus_get_address(user_bus, &a);
            debug("ok: %s\n", a);
            sd_bus_set_exit_on_disconnect(user_bus, 1);
            break;
        }
        fprintf(stderr, "sd_bus_default_user: %s. Retrying\n", strerror(-ret));
        sleep(1);
    }

    sd_bus* system_bus = NULL;
    while(1) {
        debug("connecting to d-bus system bus: ");
        int ret = sd_bus_default_system(&system_bus);
        if(ret >= 0) {
            const char* a = NULL;
            sd_bus_get_address(system_bus, &a);
            debug("ok: %s\n", a);
            sd_bus_set_exit_on_disconnect(system_bus, 1);
            break;
        }
        fprintf(stderr, "sd_bus_default_system: %s. Retrying\n", strerror(-ret));
        sleep(1);
    }

    // Connect D-Bus signal handler
    const char* match_rule = "type='signal',interface='net.nuetzlich.SystemNotifications',member='Notify'";
    int ret = sd_bus_add_match(system_bus, NULL, match_rule, handle_dbus_signal, NULL);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_match_signal: %s\n", strerror(-ret));
        exit(FATAL_SYSTEM_BUS);
    }
    debug("waiting for d-bus signals on system bus: %s\n", match_rule);

    // Set up event loop
    sd_event* event = NULL;
    ret = sd_event_default(&event);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_event_default: %s\n", strerror(-ret));
        exit(FATAL_EVENT);
    }
    ret = sd_bus_attach_event(user_bus, event, 0);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_attach_event: %s\n", strerror(-ret));
        exit(FATAL_EVENT);
    }
    ret = sd_bus_attach_event(system_bus, event, 0);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_attach_event: %s\n", strerror(-ret));
        exit(FATAL_EVENT);
    }

    // Run event loop (should not return)
    ret = sd_event_loop(event);
    fprintf(stderr, "fatal: sd_event_loop: %s\n", strerror(-ret));
    exit(FATAL_EVENT);
}
