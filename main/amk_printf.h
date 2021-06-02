/**
 * amk_printf.h
 */
#pragma once

#include "printf.h"

#define AMK_DEBUG 1
#define AMK_PRINT_TIMESTAMP 0

#if AMK_DEBUG
#if AMK_PRINT_TIMESTAMP
int amk_printf_ts(const char* format, ...);
#define amk_printf(fmt, ...) amk_printf_ts(fmt, ##__VA_ARGS__)
#else
#define amk_printf(fmt, ...) printf_(fmt, ##__VA_ARGS__)
#endif
#else
#define amk_printf(fmt, ...)
#endif
