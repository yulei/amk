/**
 * generic_hal.h
 *  HAL generic definitions
 */

#pragma once

#if defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F446xx) || defined(STM32F401xC) || defined(STM32F412Rx)
    #include "stm32f4xx.h"
    #include "stm32f4xx_hal.h"
#elif defined(STM32F722xx) 
    #include "stm32f7xx.h"
    #include "stm32f7xx_hal.h"
#elif defined(STM32F103xB) 
    #include "stm32f1xx.h"
    #include "stm32f1xx_hal.h"
#elif defined(STM32L432xx) 
    #include "stm32l4xx.h"
    #include "stm32l4xx_hal.h"
#elif defined(STM32L072xx) 
    #include "stm32l0xx.h"
    #include "stm32l0xx_hal.h"
#elif defined(STM32G431xx) 
    #include "stm32g4xx.h"
    #include "stm32g4xx_hal.h"
#elif defined(STM32L452xx) 
    #include "stm32l4xx.h"
    #include "stm32l4xx_hal.h"
#else
    #error "HAL MCU not defined"
#endif

extern void Error_Handler(void);