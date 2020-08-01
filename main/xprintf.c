/**
 * xprintf.c
 */

#include <stdarg.h>
#include "rtt/rtt.h"

int xprintf(char* fmt, ...)
{
    int result = 0;
    va_list va;
    va_start(va, fmt);
    result = rtt_printf(0, fmt, va);
    va_end(va);
    return result;
    
   return 0;
}