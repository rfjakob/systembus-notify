/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

#include "notify.h"

static sd_bus* user_bus = NULL;

// handle_signal is called a "Notify" signal is received.
int handle_signal(sd_bus_message* m, void* userdata, sd_bus_error* ret_error)
{
    // unused
    (void)userdata;
    (void)ret_error;

    const char* text = NULL;
    int ret = sd_bus_message_read_basic(m, 's', &text);
    if (ret < 0) {
        fprintf(stderr, "sd_bus_message_read_basic: %s\n", strerror(-ret));
        return 0;
    }
    puts(text);
    notify(user_bus, text);
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        fprintf(stderr, "(no parameters accepted)\n");
        exit(1);
    }
    // Connect to user bus
    int ret = sd_bus_default_user(&user_bus);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_default_user: %s\n", strerror(-ret));
        exit(1);
    }
    // Connect to system bus
    sd_bus* system_bus = NULL;
    ret = sd_bus_default_system(&system_bus);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_default_system: %s\n", strerror(-ret));
        exit(1);
    }
    // Connect D-Bus signal handler
    const char *match_rule = "type='signal',interface='net.nuetzlich.SystemNotifications',member='Notify'";
    ret = sd_bus_add_match(system_bus, NULL, match_rule, handle_signal, NULL);
    if (ret < 0) {
        fprintf(stderr, "fatal: sd_bus_match_signal: %s\n", strerror(-ret));
        exit(1);
    }
    // Processing loop
    while (1) {
        ret = sd_bus_process(system_bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "fatal: sd_bus_process: %s\n", strerror(-ret));
            exit(1);
        } else if (ret > 0) {
            continue;
        }
        /* From man sd_bus_process:
         * When zero is returned the caller should synchronously
         * poll for I/O events before calling into sd_bus_process() */
        ret = sd_bus_wait(system_bus, UINT64_MAX);
        if (ret < 0) {
            fprintf(stderr, "fatal: sd_bus_wait: %s\n", strerror(-ret));
            exit(1);
        }
    }
}
