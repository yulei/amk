/**
 * @file generic_hal.h
 */

#pragma once

#if defined(GD32E10X)
#include "gd32e10x.h"
#else
    #error "GD32 MCU not defined"
#endif

extern void Error_Handler(void);
