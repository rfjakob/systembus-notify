/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <systemd/sd-bus.h>

// notify sends a desktop notification according to the
// Desktop Notifications Specification
// (https://developer.gnome.org/notification-spec/).
void notify(sd_bus* bus, const char* summary)
{
    sd_bus_message* m = NULL;
    int ret = sd_bus_message_new_method_call(bus, &m, "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications", "Notify");
    if (ret < 0) {
        fprintf(stderr, "sd_bus_message_new_method_call: %s\n", strerror(-ret));
        return;
    }
    // Fill out the parameters according to
    // https://developer.gnome.org/notification-spec/#command-notify
    uint32_t u = 0;
    int32_t i = -1;
    ret = sd_bus_message_append(m, "susssasa{sv}i",
        "system-notify", // STRING app_name
        &u, // UINT32 replaces_id
        "utilities-system-monitor", // STRING app_icon
        summary, // STRING summary
        NULL, // STRING body
        0, // ARRAY actions
        0, // DICT hints
        &i // INT32 expire_timeout
    );
    if (ret < 0) {
        fprintf(stderr, "sd_bus_message_append: %s\n", strerror(-ret));
        sd_bus_message_unref(m);
        return;
    }
    ret = sd_bus_call(bus, m, 0, NULL, NULL);
    sd_bus_message_unref(m);
    if (ret < 0) {
        fprintf(stderr, "sd_bus_call: %s\n", strerror(-ret));
        return;
    }
}
