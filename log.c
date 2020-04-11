#include <stdarg.h>
#include <stdio.h>

int quiet = 0;

void debug(const char* fmt, ...)
{
    if (quiet) {
        return;
    }
    va_list vl;
    va_start(vl, fmt);
    vfprintf(stdout, fmt, vl);
    va_end(vl);
    return;
}
