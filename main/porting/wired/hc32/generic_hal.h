/**
 * @file generic_hal.h
 */

#pragma once

#if defined(HC32F460xE)
#include "hc32_ddl.h"
#else
    #error "HAL MCU not defined"
#endif

extern void Error_Handler(void);
