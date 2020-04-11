/* SPDX-License-Identifier: MIT */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

#include "log.h"
#include "notify.h"

static sd_bus* user_bus = NULL;

// Exit codes
enum {
    FATAL_USAGE = 2, // Usage error
    FATAL_SYSTEM_BUS = 3, // Problem connecting to system d-bus
    FATAL_USER_BUS = 4, // Problem connecting to user d-bus
    FATAL_SYSTEM_BUS_PROCESS = 5, // Problem processing d-bus signals from system d-bus
};

// handle_signal is called a "Notify" signal is received.
int handle_signal(sd_bus_message* m, void* userdata, sd_bus_error* ret_error)
{
    // unused
    (void)userdata;
    (void)ret_error;

    const char* summary = NULL;
    int ret = sd_bus_message_read_basic(m, 's', &summary);
    if (ret < 0) {
        fprintf(stderr, "sd_bus_message_read_basic: %s\n", strerror(-ret));
        return 0;
    }
    // body is optional. It stays NULL when the signal does not contain
    // a second string value.
    const char* body = NULL;
    sd_bus_message_read_basic(m, 's', &body);
    debug("\nreceived d-bus signal on system bus\n");
    debug("↳ summary: %s\n", summary);
    debug("↳ body:    %s\n", body);
    notify(user_bus, summary, body);
    return 0;
}

int main(int argc, char* argv[])
{
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
    debug("connecting to d-bus user bus: ");
    int ret = sd_bus_default_user(&user_bus);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_default_user: %s\n", strerror(-ret));
        exit(FATAL_USER_BUS);
    }
    debug("ok\n");

    debug("connecting to d-bus system bus: ");
    sd_bus* system_bus = NULL;
    ret = sd_bus_default_system(&system_bus);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_default_system: %s\n", strerror(-ret));
        exit(FATAL_SYSTEM_BUS);
    }
    debug("ok\n");

    // Connect D-Bus signal handler
    const char* match_rule = "type='signal',interface='net.nuetzlich.SystemNotifications',member='Notify'";
    ret = sd_bus_add_match(system_bus, NULL, match_rule, handle_signal, NULL);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_match_signal: %s\n", strerror(-ret));
        exit(FATAL_SYSTEM_BUS);
    }
    debug("waiting for d-bus signals on system bus: %s\n", match_rule);
    // Processing loop
    while (1) {
        ret = sd_bus_process(system_bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "fatal: sd_bus_process: %s\n", strerror(-ret));
            exit(FATAL_SYSTEM_BUS_PROCESS);
        } else if (ret > 0) {
            continue;
        }
        /* From man sd_bus_process:
         * When zero is returned the caller should synchronously
         * poll for I/O events before calling into sd_bus_process() */
        ret = sd_bus_wait(system_bus, UINT64_MAX);
        if (ret < 0) {
            fprintf(stderr, "fatal: sd_bus_wait: %s\n", strerror(-ret));
            exit(FATAL_SYSTEM_BUS_PROCESS);
        }
    }
}
