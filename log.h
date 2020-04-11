/* SPDX-License-Identifier: MIT */
#ifndef LOG_H
#define LOG_H

extern int quiet;

/* From https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/Common-Function-Attributes.html :
 * The format attribute specifies that a function takes printf
 * style arguments that should be type-checked against a format string.
 */
void debug(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
