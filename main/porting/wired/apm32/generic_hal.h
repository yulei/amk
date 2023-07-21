/**
 * @file generic_hal.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#if defined(APM32F407)
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
#else
    #error "APM32 MCU not defined"
#endif

extern void Error_Handler(void);
