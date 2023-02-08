/**
 * @file generic_hal.h
 */

#pragma once

#if defined(CH32V307)
#include "ch32v30x.h"
#else
    #error "CH32 MCU not defined"
#endif

extern void Error_Handler(void);
