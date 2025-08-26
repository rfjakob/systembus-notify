/* SPDX-License-Identifier: MIT */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

#include "log.h"

// notify sends a desktop notification according to the
// Desktop Notifications Specification
// (https://developer.gnome.org/notification-spec/).
void notify(sd_bus* bus, const char* summary, const char* body)
{
    sd_bus_message* m = NULL;
    debug("sending d-bus desktop notification on user bus: ");
    int ret = sd_bus_message_new_method_call(bus, &m, "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications", "Notify");
    if (ret < 0) {
        fprintf(stderr, "sd_bus_message_new_method_call: %s\n", strerror(-ret));
        return;
    }
    // Fill out the parameters according to
    // https://specifications.freedesktop.org/notification-spec/1.3/protocol.html#command-notify
    ret = sd_bus_message_append(m, "susssasa{sv}i",
        "systembus-notify", // STRING app_name
        0, // UINT32 replaces_id
        "utilities-system-monitor", // STRING app_icon
        summary, // STRING summary
        body, // STRING body
        0, // ARRAY actions
        0, // DICT hints
        -1 // INT32 expire_timeout
    );
    if (ret < 0) {
        fprintf(stderr, "sd_bus_message_append: %s\n", strerror(-ret));
        sd_bus_message_unref(m);
        return;
    }
    ret = sd_bus_call(bus, m, 0, NULL, NULL);
    sd_bus_message_unref(m);
    if (ret == -ENOTCONN) {
        fprintf(stderr, "fatal: sd_bus_call: %s\n", strerror(-ret));
        exit(FATAL_SEND_NOTIFY);
    } else if (ret < 0) {
        fprintf(stderr, "sd_bus_call: %s\n", strerror(-ret));
        return;
    }
    debug("ok\n");
}
