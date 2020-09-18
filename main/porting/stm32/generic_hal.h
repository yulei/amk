/**
 * generic_hal.h
 *  HAL generic definitions
 */

#pragma once

#if defined(STM32F411xE) 
    #include "stm32f4xx_hal.h"
#elif defined(STM32L433xx) 
    #include "stm32l4xx_hal.h"
#elif defined(STM32F722xx) 
    #include "stm32f7xx_hal.h"
#elif defined(STM32F103xB) 
    #include "stm32f1xx_hal.h"
#else
    #error "MCU_TYPE not defined"
#endif

extern void Error_Handler(void);