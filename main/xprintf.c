/**
 * xprintf.c
 */

#include <stdarg.h>
#include "rtt/rtt.h"

int xprintf(char* fmt, ...)
{
    int result = 0;
#if SEGGER_RTT_ENABLE
    va_list va;
    va_start(va, fmt);
    result = rtt_printf(fmt, va);
    va_end(va);
#endif
    return result;
}