/* SPDX-License-Identifier: MIT */

#ifndef NOTIFY_H
#define NOTIFY_H

void notify(sd_bus* bus, const char* app, uint32_t id, const char* icon, const char* summary, const char* body, int32_t timeout);

#endif
