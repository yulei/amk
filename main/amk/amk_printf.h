/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_printf.h
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

#define AMK_CONCAT_(a, b) a##b
#define AMK_CONCAT(a, b) AMK_CONCAT_(a, b)

#define AMK_ASSERT(expr, msg) enum { AMK_CONCAT(amk_assert_, __COUNTER__) = 1/((e) ? 1 : 0) }
