/**
 * gpio_pin.h
 * generic pin implementation
 */

#pragma once

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
    #include "nrf_gpio.h"
    typedef uint32_t pin_t;
    #define MAKE_PIN(port, pin) NRF_GPIO_PIN_MAP(port, pin)
#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32L433xx) || defined(STM32F722xx) || defined(STM32F103xB)
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
