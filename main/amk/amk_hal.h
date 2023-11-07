/**
 * @file amk_hal.h
 * @author astro
 * 
 * @copyright Copyright (C) 2023
 * 
*/

#pragma once

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
    #ifdef MAX
    #undef MAX
    #endif
    #ifdef MIN
    #undef MIN
    #endif
    #ifdef ARRAY_SIZE
    #undef ARRAY_SIZE
    #endif
    #include "nordic_common.h"

    #include "nrf.h"
#elif defined(M484)
    #undef TICK
    #include "NuMicro.h"
#elif defined(CH32V307)
    #include "ch32v30x.h"
#elif defined(GD32E10X)
    #include "gd32e10x.h"
#elif defined(GD32E50X)
    #include "gd32e50x.h"
#elif defined(HC32F460xE)
    #include "hc32_ddl.h"
    /* priority group - define the pre-emption priority and the subpriority */
    #ifndef NVIC_PRIGROUP_PRE4_SUB0
    #define NVIC_PRIGROUP_PRE4_SUB0     ((uint32_t)0x300) /*!< 4 bits for pre-emption priority 0 bits for subpriority */
    #endif
#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F446xx) || defined(STM32F401xC) || defined(STM32F412Rx)
    #include "stm32f4xx.h"
    #include "stm32f4xx_hal.h"
#elif defined(STM32F722xx) 
    #include "stm32f7xx.h"
    #include "stm32f7xx_hal.h"
#elif defined(STM32F103xB) 
    #include "stm32f1xx.h"
    #include "stm32f1xx_hal.h"
#elif defined(STM32L432xx) || defined(STM32L452xx) || defined(STM32L476xx)
    #include "stm32l4xx.h"
    #include "stm32l4xx_hal.h"
#elif defined(STM32L072xx) 
    #include "stm32l0xx.h"
    #include "stm32l0xx_hal.h"
#elif defined(STM32G431xx) 
    #include "stm32g4xx.h"
    #include "stm32g4xx_hal.h"
#elif defined(APM32F407)
    #include "apm32f4xx.h"
    #include "apm32f4xx_adc.h"
    #include "apm32f4xx_dbgmcu.h"
    #include "apm32f4xx_dma.h"
    #include "apm32f4xx_fmc.h"
    #include "apm32f4xx_gpio.h"
    #include "apm32f4xx_i2c.h"
    #include "apm32f4xx_misc.h"
    #include "apm32f4xx_pmu.h"
    #include "apm32f4xx_rcm.h"
    #include "apm32f4xx_rtc.h"
    #include "apm32f4xx_spi.h"
    #include "apm32f4xx_syscfg.h"
    #include "apm32f4xx_tmr.h"
    #include "apm32f4xx_usart.h"
    #include "apm32f4xx_usb_device.h"
    #include "apm32f4xx_usb.h"
    extern void Error_Handler(void);
#else
#error "unsupport platform"
#endif

extern void Error_Handler(void);
