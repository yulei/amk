/**
 * amk_printf.h
 */
#pragma once

#include "printf.h"

#define AMK_DEBUG 1

#if AMK_DEBUG
#define amk_printf(fmt, ...) printf_(fmt, ##__VA_ARGS__)
#else
#define amk_printf(fmt, ...)
#endif
