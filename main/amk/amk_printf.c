/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_printf.c
 */

#include "printf.h"
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

#if AMK_PRINT_TIMESTAMP

#include "generic_hal.h"
int amk_printf_ts(const char* format, ...)
{
    int size = 0;
#ifndef STM32L072xx
    unsigned int ticks = DWT->CYCCNT / (SystemCoreClock/1000000);
    size += printf_("[TS-%8u]: ", ticks);
#endif
    va_list va;
    va_start(va, format);
    size += printf_(format, va);
    va_end(va);

    return size;
}
#else

int amk_printf_fl(const char* format, ...)
{
    int size = 0;

    va_list va;
    va_start(va, format);
    size += printf_(format, va);
    va_end(va);
    return size;
}

#endif