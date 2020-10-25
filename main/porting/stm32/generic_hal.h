/**
 * generic_hal.h
 *  HAL generic definitions
 */

#pragma once

#if defined(STM32F411xE) || defined(STM32F405xx)
    #include "stm32f4xx.h"
    #include "stm32f4xx_hal.h"
#elif defined(STM32F722xx) 
    #include "stm32f7xx.h"
    #include "stm32f7xx_hal.h"
#elif defined(STM32F103xB) 
    #include "stm32f1xx.h"
    #include "stm32f1xx_hal.h"
#else
    #error "HAL MCU not defined"
#endif

extern void Error_Handler(void);