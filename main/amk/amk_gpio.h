/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_gpio.h
 * @brief generic pin implementation
 */

#pragma once

#include <stdint.h>
#include "amk_hal.h"

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
    #include "nrf_gpio.h"

    typedef uint32_t pin_t;
    typedef uint32_t port_t;
    #define INVALID_PIN 0xFFFFFFFF

    #define P0_0 NRF_GPIO_PIN_MAP(0,0)
    #define P0_1 NRF_GPIO_PIN_MAP(0,1)
    #define P0_2 NRF_GPIO_PIN_MAP(0,2)
    #define P0_3 NRF_GPIO_PIN_MAP(0,3)
    #define P0_4 NRF_GPIO_PIN_MAP(0,4)
    #define P0_5 NRF_GPIO_PIN_MAP(0,5)
    #define P0_6 NRF_GPIO_PIN_MAP(0,6)
    #define P0_7 NRF_GPIO_PIN_MAP(0,7)
    #define P0_8 NRF_GPIO_PIN_MAP(0,8)
    #define P0_9 NRF_GPIO_PIN_MAP(0,9)
    #define P0_10 NRF_GPIO_PIN_MAP(0,10)
    #define P0_11 NRF_GPIO_PIN_MAP(0,11)
    #define P0_12 NRF_GPIO_PIN_MAP(0,12)
    #define P0_13 NRF_GPIO_PIN_MAP(0,13)
    #define P0_14 NRF_GPIO_PIN_MAP(0,14)
    #define P0_15 NRF_GPIO_PIN_MAP(0,15)
    #define P0_16 NRF_GPIO_PIN_MAP(0,16)
    #define P0_17 NRF_GPIO_PIN_MAP(0,17)
    #define P0_18 NRF_GPIO_PIN_MAP(0,18)
    #define P0_19 NRF_GPIO_PIN_MAP(0,19)
    #define P0_20 NRF_GPIO_PIN_MAP(0,20)
    #define P0_21 NRF_GPIO_PIN_MAP(0,21)
    #define P0_22 NRF_GPIO_PIN_MAP(0,22)
    #define P0_23 NRF_GPIO_PIN_MAP(0,23)
    #define P0_24 NRF_GPIO_PIN_MAP(0,24)
    #define P0_25 NRF_GPIO_PIN_MAP(0,25)
    #define P0_26 NRF_GPIO_PIN_MAP(0,26)
    #define P0_27 NRF_GPIO_PIN_MAP(0,27)
    #define P0_28 NRF_GPIO_PIN_MAP(0,28)
    #define P0_29 NRF_GPIO_PIN_MAP(0,29)
    #define P0_30 NRF_GPIO_PIN_MAP(0,30)
    #define P0_31 NRF_GPIO_PIN_MAP(0,31)

    #define P1_0 NRF_GPIO_PIN_MAP(1,0)
    #define P1_1 NRF_GPIO_PIN_MAP(1,1)
    #define P1_2 NRF_GPIO_PIN_MAP(1,2)
    #define P1_3 NRF_GPIO_PIN_MAP(1,3)
    #define P1_4 NRF_GPIO_PIN_MAP(1,4)
    #define P1_5 NRF_GPIO_PIN_MAP(1,5)
    #define P1_6 NRF_GPIO_PIN_MAP(1,6)
    #define P1_7 NRF_GPIO_PIN_MAP(1,7)
    #define P1_8 NRF_GPIO_PIN_MAP(1,8)
    #define P1_9 NRF_GPIO_PIN_MAP(1,9)
    #define P1_10 NRF_GPIO_PIN_MAP(1,10)
    #define P1_11 NRF_GPIO_PIN_MAP(1,11)
    #define P1_12 NRF_GPIO_PIN_MAP(1,12)
    #define P1_13 NRF_GPIO_PIN_MAP(1,13)
    #define P1_14 NRF_GPIO_PIN_MAP(1,14)
    #define P1_15 NRF_GPIO_PIN_MAP(1,15)
    #define P1_16 NRF_GPIO_PIN_MAP(1,16)
    #define P1_17 NRF_GPIO_PIN_MAP(1,17)
    #define P1_18 NRF_GPIO_PIN_MAP(1,18)
    #define P1_19 NRF_GPIO_PIN_MAP(1,19)
    #define P1_20 NRF_GPIO_PIN_MAP(1,20)
    #define P1_21 NRF_GPIO_PIN_MAP(1,21)
    #define P1_22 NRF_GPIO_PIN_MAP(1,22)
    #define P1_23 NRF_GPIO_PIN_MAP(1,23)
    #define P1_24 NRF_GPIO_PIN_MAP(1,24)
    #define P1_25 NRF_GPIO_PIN_MAP(1,25)
    #define P1_26 NRF_GPIO_PIN_MAP(1,26)
    #define P1_27 NRF_GPIO_PIN_MAP(1,27)
    #define P1_28 NRF_GPIO_PIN_MAP(1,28)
    #define P1_29 NRF_GPIO_PIN_MAP(1,29)
    #define P1_30 NRF_GPIO_PIN_MAP(1,30)
    #define P1_31 NRF_GPIO_PIN_MAP(1,31)

#elif \
    defined(STM32F401xC) || defined(STM32F411xE) || defined(STM32F405xx) || \
    defined(STM32F722xx) || defined(STM32F103xB) || defined(STM32L432xx) ||  \
    defined(STM32L072xx) || defined(STM32F446xx) || defined(STM32G431xx) || \
    defined(STM32L452xx) || defined(STM32F412Rx) || defined(STM32L476xx) || \
    defined(STM32U575xx)
    #include "gpio_stm32.h"
#elif defined(__SAMD21G18A__)
    typedef uint8_t pin_t;

    #define GPIO_PIN_FUNCTION_A 0
    #define GPIO_PIN_FUNCTION_B 1
    #define GPIO_PIN_FUNCTION_C 2
    #define GPIO_PIN_FUNCTION_D 3
    #define GPIO_PIN_FUNCTION_E 4
    #define GPIO_PIN_FUNCTION_F 5
    #define GPIO_PIN_FUNCTION_G 6
    #define GPIO_PIN_FUNCTION_H 7

    #define A0 GPIO(GPIO_PORTA, 0)
    #define A1 GPIO(GPIO_PORTA, 1)
    #define A2 GPIO(GPIO_PORTA, 2)
    #define A3 GPIO(GPIO_PORTA, 3)
    #define A4 GPIO(GPIO_PORTA, 4)
    #define A5 GPIO(GPIO_PORTA, 5)
    #define A6 GPIO(GPIO_PORTA, 6)
    #define A7 GPIO(GPIO_PORTA, 7)
    #define A8 GPIO(GPIO_PORTA, 8)
    #define A9 GPIO(GPIO_PORTA, 9)
    #define A10 GPIO(GPIO_PORTA, 10)
    #define A11 GPIO(GPIO_PORTA, 11)
    #define A12 GPIO(GPIO_PORTA, 12)
    #define A13 GPIO(GPIO_PORTA, 13)
    #define A14 GPIO(GPIO_PORTA, 14)
    #define A15 GPIO(GPIO_PORTA, 15)
    #define A16 GPIO(GPIO_PORTA, 16)
    #define A17 GPIO(GPIO_PORTA, 17)
    #define A18 GPIO(GPIO_PORTA, 18)
    #define A19 GPIO(GPIO_PORTA, 19)
    #define A20 GPIO(GPIO_PORTA, 20)
    #define A21 GPIO(GPIO_PORTA, 21)
    #define A22 GPIO(GPIO_PORTA, 22)
    #define A23 GPIO(GPIO_PORTA, 23)
    #define A24 GPIO(GPIO_PORTA, 24)
    #define A25 GPIO(GPIO_PORTA, 25)
    #define A26 GPIO(GPIO_PORTA, 26)
    #define A27 GPIO(GPIO_PORTA, 27)
    #define A28 GPIO(GPIO_PORTA, 28)
    #define A29 GPIO(GPIO_PORTA, 29)
    #define A30 GPIO(GPIO_PORTA, 30)
    #define A31 GPIO(GPIO_PORTA, 31)

    #define B0 GPIO(GPIO_PORTB, 0)
    #define B1 GPIO(GPIO_PORTB, 1)
    #define B2 GPIO(GPIO_PORTB, 2)
    #define B3 GPIO(GPIO_PORTB, 3)
    #define B4 GPIO(GPIO_PORTB, 4)
    #define B5 GPIO(GPIO_PORTB, 5)
    #define B6 GPIO(GPIO_PORTB, 6)
    #define B7 GPIO(GPIO_PORTB, 7)
    #define B8 GPIO(GPIO_PORTB, 8)
    #define B9 GPIO(GPIO_PORTB, 9)
    #define B10 GPIO(GPIO_PORTB, 10)
    #define B11 GPIO(GPIO_PORTB, 11)
    #define B12 GPIO(GPIO_PORTB, 12)
    #define B13 GPIO(GPIO_PORTB, 13)
    #define B14 GPIO(GPIO_PORTB, 14)
    #define B15 GPIO(GPIO_PORTB, 15)
    #define B16 GPIO(GPIO_PORTB, 16)
    #define B17 GPIO(GPIO_PORTB, 17)
    #define B18 GPIO(GPIO_PORTB, 18)
    #define B19 GPIO(GPIO_PORTB, 19)
    #define B20 GPIO(GPIO_PORTB, 20)
    #define B21 GPIO(GPIO_PORTB, 21)
    #define B22 GPIO(GPIO_PORTB, 22)
    #define B23 GPIO(GPIO_PORTB, 23)
    #define B24 GPIO(GPIO_PORTB, 24)
    #define B25 GPIO(GPIO_PORTB, 25)
    #define B26 GPIO(GPIO_PORTB, 26)
    #define B27 GPIO(GPIO_PORTB, 27)
    #define B28 GPIO(GPIO_PORTB, 28)
    #define B29 GPIO(GPIO_PORTB, 29)
    #define B30 GPIO(GPIO_PORTB, 30)
    #define B31 GPIO(GPIO_PORTB, 31)
#elif   defined(M484)
    #include "gpio_m480.h"
#elif defined(GD32E10X) || defined(GD32E50X)
    #include "gpio_gd32.h"
#elif defined(HC32F460xE)
   #include "gpio_hc32.h" 
#elif defined(CH32V307)
   #include "gpio_ch32.h" 
#elif defined(APM32F407)
   #include "gpio_apm32.h" 
#elif defined(AT32F405)
   #include "gpio_at32.h" 
#elif defined(LPC55xx)
   #include "gpio_lpc55.h" 
#else
    #error "MCU_TYPE not defined"
#endif

int gpio_read_pin(pin_t pin);
void gpio_write_pin(pin_t pin, int value);

uint32_t gpio_read_port(port_t port);
void gpio_write_port(port_t port, uint32_t value);

void gpio_set_output_pushpull(pin_t pin);
void gpio_set_output_opendrain(pin_t pin);

void gpio_set_input_floating(pin_t pin);
void gpio_set_input_pullup(pin_t pin);
void gpio_set_input_pulldown(pin_t pin);
