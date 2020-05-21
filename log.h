/* SPDX-License-Identifier: MIT */
#ifndef LOG_H
#define LOG_H

extern int quiet;

/* From https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/Common-Function-Attributes.html :
 * The format attribute specifies that a function takes printf
 * style arguments that should be type-checked against a format string.
 */
void debug(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Exit codes
enum {
    FATAL_USAGE = 2, // Usage error
    FATAL_SYSTEM_BUS = 3, // Problem connecting to system d-bus
    FATAL_USER_BUS = 4, // Problem connecting to user d-bus
    FATAL_SYSTEM_BUS_PROCESS = 5, // Problem processing d-bus signals from system d-bus
    FATAL_EVENT = 6, // Problem in event loop
    FATAL_SEND_NOTIFY = 7, // problem calling notify on the user bus
};

#endif
