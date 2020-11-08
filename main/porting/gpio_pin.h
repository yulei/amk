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
    typedef struct {
        GPIO_TypeDef *port;
        uint16_t pin;
    } pin_t;

    #define MAKE_PIN(port, pin) {port, pin}

    #define A0 MAKE_PIN(GPIOA, GPIO_PIN_0)
    #define A1 MAKE_PIN(GPIOA, GPIO_PIN_1)
    #define A2 MAKE_PIN(GPIOA, GPIO_PIN_2)
    #define A3 MAKE_PIN(GPIOA, GPIO_PIN_3)
    #define A4 MAKE_PIN(GPIOA, GPIO_PIN_4)
    #define A5 MAKE_PIN(GPIOA, GPIO_PIN_5)
    #define A6 MAKE_PIN(GPIOA, GPIO_PIN_6)
    #define A7 MAKE_PIN(GPIOA, GPIO_PIN_7)
    #define A8 MAKE_PIN(GPIOA, GPIO_PIN_8)
    #define A9 MAKE_PIN(GPIOA, GPIO_PIN_9)
    #define A10 MAKE_PIN(GPIOA, GPIO_PIN_10)
    #define A11 MAKE_PIN(GPIOA, GPIO_PIN_11)
    #define A12 MAKE_PIN(GPIOA, GPIO_PIN_12)
    #define A13 MAKE_PIN(GPIOA, GPIO_PIN_13)
    #define A14 MAKE_PIN(GPIOA, GPIO_PIN_14)
    #define A15 MAKE_PIN(GPIOA, GPIO_PIN_15)

    #define B0 MAKE_PIN(GPIOB, GPIO_PIN_0)
    #define B1 MAKE_PIN(GPIOB, GPIO_PIN_1)
    #define B2 MAKE_PIN(GPIOB, GPIO_PIN_2)
    #define B3 MAKE_PIN(GPIOB, GPIO_PIN_3)
    #define B4 MAKE_PIN(GPIOB, GPIO_PIN_4)
    #define B5 MAKE_PIN(GPIOB, GPIO_PIN_5)
    #define B6 MAKE_PIN(GPIOB, GPIO_PIN_6)
    #define B7 MAKE_PIN(GPIOB, GPIO_PIN_7)
    #define B8 MAKE_PIN(GPIOB, GPIO_PIN_8)
    #define B9 MAKE_PIN(GPIOB, GPIO_PIN_9)
    #define B10 MAKE_PIN(GPIOB, GPIO_PIN_10)
    #define B11 MAKE_PIN(GPIOB, GPIO_PIN_11)
    #define B12 MAKE_PIN(GPIOB, GPIO_PIN_12)
    #define B13 MAKE_PIN(GPIOB, GPIO_PIN_13)
    #define B14 MAKE_PIN(GPIOB, GPIO_PIN_14)
    #define B15 MAKE_PIN(GPIOB, GPIO_PIN_15)

    #define C0 MAKE_PIN(GPIOC, GPIO_PIN_0)
    #define C1 MAKE_PIN(GPIOC, GPIO_PIN_1)
    #define C2 MAKE_PIN(GPIOC, GPIO_PIN_2)
    #define C3 MAKE_PIN(GPIOC, GPIO_PIN_3)
    #define C4 MAKE_PIN(GPIOC, GPIO_PIN_4)
    #define C5 MAKE_PIN(GPIOC, GPIO_PIN_5)
    #define C6 MAKE_PIN(GPIOC, GPIO_PIN_6)
    #define C7 MAKE_PIN(GPIOC, GPIO_PIN_7)
    #define C8 MAKE_PIN(GPIOC, GPIO_PIN_8)
    #define C9 MAKE_PIN(GPIOC, GPIO_PIN_9)
    #define C10 MAKE_PIN(GPIOC, GPIO_PIN_10)
    #define C11 MAKE_PIN(GPIOC, GPIO_PIN_11)
    #define C12 MAKE_PIN(GPIOC, GPIO_PIN_12)
    #define C13 MAKE_PIN(GPIOC, GPIO_PIN_13)
    #define C14 MAKE_PIN(GPIOC, GPIO_PIN_14)
    #define C15 MAKE_PIN(GPIOC, GPIO_PIN_15)

    #define D0 MAKE_PIN(GPIOD, GPIO_PIN_0)
    #define D1 MAKE_PIN(GPIOD, GPIO_PIN_1)
    #define D2 MAKE_PIN(GPIOD, GPIO_PIN_2)
    #define D3 MAKE_PIN(GPIOD, GPIO_PIN_3)
    #define D4 MAKE_PIN(GPIOD, GPIO_PIN_4)
    #define D5 MAKE_PIN(GPIOD, GPIO_PIN_5)
    #define D6 MAKE_PIN(GPIOD, GPIO_PIN_6)
    #define D7 MAKE_PIN(GPIOD, GPIO_PIN_7)
    #define D8 MAKE_PIN(GPIOD, GPIO_PIN_8)
    #define D9 MAKE_PIN(GPIOD, GPIO_PIN_9)
    #define D10 MAKE_PIN(GPIOD, GPIO_PIN_10)
    #define D11 MAKE_PIN(GPIOD, GPIO_PIN_11)
    #define D12 MAKE_PIN(GPIOD, GPIO_PIN_12)
    #define D13 MAKE_PIN(GPIOD, GPIO_PIN_13)
    #define D14 MAKE_PIN(GPIOD, GPIO_PIN_14)
    #define D15 MAKE_PIN(GPIOD, GPIO_PIN_15)
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
