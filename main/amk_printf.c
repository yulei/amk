/**
 * amk_printf.c
 */

#include "amk_printf.h"

int xprintf(char* fmt, ...)
{
    int result = 0;
    va_list va;
    va_start(va, fmt);
    result = vprintf_(fmt, va);
    va_end(va);
    return result;
}

#ifndef SEGGER_RTT_ENABLE
#define SEGGER_RTT_ENABLE 1
#endif

#if SEGGER_RTT_ENABLE

#include "SEGGER_RTT.h"
void _putchar(char character)
{
    SEGGER_RTT_PutChar(0, character);
}
#else
void _putchar(char character)
{}
#endif