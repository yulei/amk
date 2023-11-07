/**
 * @file gpio_stm32.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

    #include "amk_hal.h"

    #define INVALID_PIN 0xFFFFFFFF
    typedef uint32_t pin_t;
    typedef uint32_t port_t;

    #define PIN_BIT(pin, pos) ((((pin)>>(pos))&1) == 1 ? (pos) : (pin))
    #define PIN_C(x) \
    PIN_BIT(PIN_BIT(PIN_BIT(PIN_BIT(\
    PIN_BIT(PIN_BIT(PIN_BIT(PIN_BIT(\
    PIN_BIT(PIN_BIT(PIN_BIT(PIN_BIT(\
    PIN_BIT(PIN_BIT(PIN_BIT(PIN_BIT((x),\
    0),1),2),3),4),5),6),7),8),9),10),11),12),13),14),15)

    #define PIN_P(x) (1<<(x))

    //#define MAKE_PIN(port, pin) (((uint32_t)(port))|((PIN_C(pin))&0x0F))
    #define MAKE_PIN(port, pin) (((uint32_t)(port))|((pin)&0x0F))
    #define GET_PIN(port_pin) PIN_P(((port_pin)&0x0F))
    #define GET_PORT(port_pin) ((GPIO_TypeDef*)((port_pin)&~0x0F))

    #define PORT_A  (port_t)(GPIOA)
    #define PORT_B  (port_t)(GPIOB)
    #define PORT_C  (port_t)(GPIOC)
    #define PORT_D  (port_t)(GPIOD)

    #define A0 MAKE_PIN(GPIOA, 0)
    #define A1 MAKE_PIN(GPIOA, 1)
    #define A2 MAKE_PIN(GPIOA, 2)
    #define A3 MAKE_PIN(GPIOA, 3)
    #define A4 MAKE_PIN(GPIOA, 4)
    #define A5 MAKE_PIN(GPIOA, 5)
    #define A6 MAKE_PIN(GPIOA, 6)
    #define A7 MAKE_PIN(GPIOA, 7)
    #define A8 MAKE_PIN(GPIOA, 8)
    #define A9 MAKE_PIN(GPIOA, 9)
    #define A10 MAKE_PIN(GPIOA, 10)
    #define A11 MAKE_PIN(GPIOA, 11)
    #define A12 MAKE_PIN(GPIOA, 12)
    #define A13 MAKE_PIN(GPIOA, 13)
    #define A14 MAKE_PIN(GPIOA, 14)
    #define A15 MAKE_PIN(GPIOA, 15)

    #define B0 MAKE_PIN(GPIOB, 0)
    #define B1 MAKE_PIN(GPIOB, 1)
    #define B2 MAKE_PIN(GPIOB, 2)
    #define B3 MAKE_PIN(GPIOB, 3)
    #define B4 MAKE_PIN(GPIOB, 4)
    #define B5 MAKE_PIN(GPIOB, 5)
    #define B6 MAKE_PIN(GPIOB, 6)
    #define B7 MAKE_PIN(GPIOB, 7)
    #define B8 MAKE_PIN(GPIOB, 8)
    #define B9 MAKE_PIN(GPIOB, 9)
    #define B10 MAKE_PIN(GPIOB, 10)
    #define B11 MAKE_PIN(GPIOB, 11)
    #define B12 MAKE_PIN(GPIOB, 12)
    #define B13 MAKE_PIN(GPIOB, 13)
    #define B14 MAKE_PIN(GPIOB, 14)
    #define B15 MAKE_PIN(GPIOB, 15)

    #define C0 MAKE_PIN(GPIOC, 0)
    #define C1 MAKE_PIN(GPIOC, 1)
    #define C2 MAKE_PIN(GPIOC, 2)
    #define C3 MAKE_PIN(GPIOC, 3)
    #define C4 MAKE_PIN(GPIOC, 4)
    #define C5 MAKE_PIN(GPIOC, 5)
    #define C6 MAKE_PIN(GPIOC, 6)
    #define C7 MAKE_PIN(GPIOC, 7)
    #define C8 MAKE_PIN(GPIOC, 8)
    #define C9 MAKE_PIN(GPIOC, 9)
    #define C10 MAKE_PIN(GPIOC, 10)
    #define C11 MAKE_PIN(GPIOC, 11)
    #define C12 MAKE_PIN(GPIOC, 12)
    #define C13 MAKE_PIN(GPIOC, 13)
    #define C14 MAKE_PIN(GPIOC, 14)
    #define C15 MAKE_PIN(GPIOC, 15)

    #define D0 MAKE_PIN(GPIOD, 0)
    #define D1 MAKE_PIN(GPIOD, 1)
    #define D2 MAKE_PIN(GPIOD, 2)
    #define D3 MAKE_PIN(GPIOD, 3)
    #define D4 MAKE_PIN(GPIOD, 4)
    #define D5 MAKE_PIN(GPIOD, 5)
    #define D6 MAKE_PIN(GPIOD, 6)
    #define D7 MAKE_PIN(GPIOD, 7)
    #define D8 MAKE_PIN(GPIOD, 8)
    #define D9 MAKE_PIN(GPIOD, 9)
    #define D10 MAKE_PIN(GPIOD, 10)
    #define D11 MAKE_PIN(GPIOD, 11)
    #define D12 MAKE_PIN(GPIOD, 12)
    #define D13 MAKE_PIN(GPIOD, 13)
    #define D14 MAKE_PIN(GPIOD, 14)
    #define D15 MAKE_PIN(GPIOD, 15)
