/**
 * gpio_pin.h
 * generic pin implementation
 */

#pragma once

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
    #include "nrf_gpio.h"
    typedef uint32_t pin_t;
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

#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F722xx) || defined(STM32F103xB)
    #include "generic_hal.h"

    typedef uint32_t pin_t;

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
#elif defined(__SAMD21G18A__)
    #include "generic_hal.h"
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
#elif   defined(NUC126)
    #include "generic_hal.h"
    typedef uint32_t pin_t;
    #define MAKE_PIN(port, pin)     (((port)<<16) | (pin))
    #define GET_PIN(port_pin)       ((port_pin) & 0xFF)
    #define GET_PORT(port_pin)      (((port_pin)>>16) & 0xFF)
    #define A0          MAKE_PIN(0, 0 ) /*!< Specify PA.0 Pin Data Input/Output */
    #define A1          MAKE_PIN(0, 1 ) /*!< Specify PA.1 Pin Data Input/Output */
    #define A2          MAKE_PIN(0, 2 ) /*!< Specify PA.2 Pin Data Input/Output */
    #define A3          MAKE_PIN(0, 3 ) /*!< Specify PA.3 Pin Data Input/Output */
    #define A4          MAKE_PIN(0, 4 ) /*!< Specify PA.4 Pin Data Input/Output */
    #define A5          MAKE_PIN(0, 5 ) /*!< Specify PA.5 Pin Data Input/Output */
    #define A6          MAKE_PIN(0, 6 ) /*!< Specify PA.6 Pin Data Input/Output */
    #define A7          MAKE_PIN(0, 7 ) /*!< Specify PA.7 Pin Data Input/Output */
    #define A8          MAKE_PIN(0, 8 ) /*!< Specify PA.8 Pin Data Input/Output */
    #define A9          MAKE_PIN(0, 9 ) /*!< Specify PA.9 Pin Data Input/Output */
    #define A10         MAKE_PIN(0, 10) /*!< Specify PA.10 Pin Data Input/Output */
    #define A11         MAKE_PIN(0, 11) /*!< Specify PA.11 Pin Data Input/Output */
    #define A12         MAKE_PIN(0, 12) /*!< Specify PA.12 Pin Data Input/Output */
    #define A13         MAKE_PIN(0, 13) /*!< Specify PA.13 Pin Data Input/Output */
    #define A14         MAKE_PIN(0, 14) /*!< Specify PA.14 Pin Data Input/Output */
    #define A15         MAKE_PIN(0, 15) /*!< Specify PA.15 Pin Data Input/Output */
    #define B0          MAKE_PIN(1, 0 ) /*!< Specify PB.0 Pin Data Input/Output */
    #define B1          MAKE_PIN(1, 1 ) /*!< Specify PB.1 Pin Data Input/Output */
    #define B2          MAKE_PIN(1, 2 ) /*!< Specify PB.2 Pin Data Input/Output */
    #define B3          MAKE_PIN(1, 3 ) /*!< Specify PB.3 Pin Data Input/Output */
    #define B4          MAKE_PIN(1, 4 ) /*!< Specify PB.4 Pin Data Input/Output */
    #define B5          MAKE_PIN(1, 5 ) /*!< Specify PB.5 Pin Data Input/Output */
    #define B6          MAKE_PIN(1, 6 ) /*!< Specify PB.6 Pin Data Input/Output */
    #define B7          MAKE_PIN(1, 7 ) /*!< Specify PB.7 Pin Data Input/Output */
    #define B8          MAKE_PIN(1, 8 ) /*!< Specify PB.8 Pin Data Input/Output */
    #define B9          MAKE_PIN(1, 9 ) /*!< Specify PB.9 Pin Data Input/Output */
    #define B10         MAKE_PIN(1, 10) /*!< Specify PB.10 Pin Data Input/Output */
    #define B11         MAKE_PIN(1, 11) /*!< Specify PB.11 Pin Data Input/Output */
    #define B12         MAKE_PIN(1, 12) /*!< Specify PB.12 Pin Data Input/Output */
    #define B13         MAKE_PIN(1, 13) /*!< Specify PB.13 Pin Data Input/Output */
    #define B14         MAKE_PIN(1, 14) /*!< Specify PB.14 Pin Data Input/Output */
    #define B15         MAKE_PIN(1, 15) /*!< Specify PB.15 Pin Data Input/Output */
    #define C0          MAKE_PIN(2, 0 ) /*!< Specify PC.0 Pin Data Input/Output */
    #define C1          MAKE_PIN(2, 1 ) /*!< Specify PC.1 Pin Data Input/Output */
    #define C2          MAKE_PIN(2, 2 ) /*!< Specify PC.2 Pin Data Input/Output */
    #define C3          MAKE_PIN(2, 3 ) /*!< Specify PC.3 Pin Data Input/Output */
    #define C4          MAKE_PIN(2, 4 ) /*!< Specify PC.4 Pin Data Input/Output */
    #define C5          MAKE_PIN(2, 5 ) /*!< Specify PC.5 Pin Data Input/Output */
    #define C6          MAKE_PIN(2, 6 ) /*!< Specify PC.6 Pin Data Input/Output */
    #define C7          MAKE_PIN(2, 7 ) /*!< Specify PC.7 Pin Data Input/Output */
    #define C8          MAKE_PIN(2, 8 ) /*!< Specify PC.8 Pin Data Input/Output */
    #define C9          MAKE_PIN(2, 9 ) /*!< Specify PC.9 Pin Data Input/Output */
    #define C10         MAKE_PIN(2, 10) /*!< Specify PC.10 Pin Data Input/Output */
    #define C11         MAKE_PIN(2, 11) /*!< Specify PC.11 Pin Data Input/Output */
    #define C12         MAKE_PIN(2, 12) /*!< Specify PC.12 Pin Data Input/Output */
    #define C13         MAKE_PIN(2, 13) /*!< Specify PC.13 Pin Data Input/Output */
    #define C14         MAKE_PIN(2, 14) /*!< Specify PC.14 Pin Data Input/Output */
    #define C15         MAKE_PIN(2, 15) /*!< Specify PC.15 Pin Data Input/Output */
    #define D0          MAKE_PIN(3, 0 ) /*!< Specify PD.0 Pin Data Input/Output */
    #define D1          MAKE_PIN(3, 1 ) /*!< Specify PD.1 Pin Data Input/Output */
    #define D2          MAKE_PIN(3, 2 ) /*!< Specify PD.2 Pin Data Input/Output */
    #define D3          MAKE_PIN(3, 3 ) /*!< Specify PD.3 Pin Data Input/Output */
    #define D4          MAKE_PIN(3, 4 ) /*!< Specify PD.4 Pin Data Input/Output */
    #define D5          MAKE_PIN(3, 5 ) /*!< Specify PD.5 Pin Data Input/Output */
    #define D6          MAKE_PIN(3, 6 ) /*!< Specify PD.6 Pin Data Input/Output */
    #define D7          MAKE_PIN(3, 7 ) /*!< Specify PD.7 Pin Data Input/Output */
    #define D8          MAKE_PIN(3, 8 ) /*!< Specify PD.8 Pin Data Input/Output */
    #define D9          MAKE_PIN(3, 9 ) /*!< Specify PD.9 Pin Data Input/Output */
    #define D10         MAKE_PIN(3, 10) /*!< Specify PD.10 Pin Data Input/Output */
    #define D11         MAKE_PIN(3, 11) /*!< Specify PD.11 Pin Data Input/Output */
    #define D12         MAKE_PIN(3, 12) /*!< Specify PD.12 Pin Data Input/Output */
    #define D13         MAKE_PIN(3, 13) /*!< Specify PD.13 Pin Data Input/Output */
    #define D14         MAKE_PIN(3, 14) /*!< Specify PD.14 Pin Data Input/Output */
    #define D15         MAKE_PIN(3, 15) /*!< Specify PD.15 Pin Data Input/Output */
    #define E0          MAKE_PIN(4, 0 ) /*!< Specify PE.0 Pin Data Input/Output */
    #define E1          MAKE_PIN(4, 1 ) /*!< Specify PE.1 Pin Data Input/Output */
    #define E2          MAKE_PIN(4, 2 ) /*!< Specify PE.2 Pin Data Input/Output */
    #define E3          MAKE_PIN(4, 3 ) /*!< Specify PE.3 Pin Data Input/Output */
    #define E4          MAKE_PIN(4, 4 ) /*!< Specify PE.4 Pin Data Input/Output */
    #define E5          MAKE_PIN(4, 5 ) /*!< Specify PE.5 Pin Data Input/Output */
    #define E6          MAKE_PIN(4, 6 ) /*!< Specify PE.6 Pin Data Input/Output */
    #define E7          MAKE_PIN(4, 7 ) /*!< Specify PE.7 Pin Data Input/Output */
    #define E8          MAKE_PIN(4, 8 ) /*!< Specify PE.8 Pin Data Input/Output */
    #define E9          MAKE_PIN(4, 9 ) /*!< Specify PE.9 Pin Data Input/Output */
    #define E10         MAKE_PIN(4, 10) /*!< Specify PE.10 Pin Data Input/Output */
    #define E11         MAKE_PIN(4, 11) /*!< Specify PE.11 Pin Data Input/Output */
    #define E12         MAKE_PIN(4, 12) /*!< Specify PE.12 Pin Data Input/Output */
    #define E13         MAKE_PIN(4, 13) /*!< Specify PE.13 Pin Data Input/Output */
    #define F0          MAKE_PIN(5, 0 ) /*!< Specify PF.0 Pin Data Input/Output */
    #define F1          MAKE_PIN(5, 1 ) /*!< Specify PF.1 Pin Data Input/Output */
    #define F2          MAKE_PIN(5, 2 ) /*!< Specify PF.2 Pin Data Input/Output */
    #define F3          MAKE_PIN(5, 3 ) /*!< Specify PF.3 Pin Data Input/Output */
    #define F4          MAKE_PIN(5, 4 ) /*!< Specify PF.4 Pin Data Input/Output */
    #define F5          MAKE_PIN(5, 5 ) /*!< Specify PF.5 Pin Data Input/Output */
    #define F6          MAKE_PIN(5, 6 ) /*!< Specify PF.6 Pin Data Input/Output */
    #define F7          MAKE_PIN(5, 7 ) /*!< Specify PF.7 Pin Data Input/Output */
#else
    #error "MCU_TYPE not defined"
#endif

int gpio_read_pin(pin_t pin);
void gpio_write_pin(pin_t pin, int value);

void gpio_set_output_pushpull(pin_t pin);
void gpio_set_output_opendrain(pin_t pin);

void gpio_set_input_floating(pin_t pin);
void gpio_set_input_pullup(pin_t pin);
void gpio_set_input_pulldown(pin_t pin);
