/**
 * @file generic_hal.h
 */

#pragma once

#if defined(HC32F460xE)
#include "hc32_ddl.h"
/* priority group - define the pre-emption priority and the subpriority */
#ifndef NVIC_PRIGROUP_PRE4_SUB0
#define NVIC_PRIGROUP_PRE4_SUB0     ((uint32_t)0x300) /*!< 4 bits for pre-emption priority 0 bits for subpriority */
#endif
#else
    #error "HAL MCU not defined"
#endif

extern void Error_Handler(void);
