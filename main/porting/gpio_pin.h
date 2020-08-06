/**
 * gpio_pin.h
 * generic pin implementation
 */

#pragma once


#ifdef GPIO_NRF5X
    #include "nrf_gpio.h"
    typedef uint32_t pin_t;
    #define MAKE_PIN(port, pin) NRF_GPIO_PIN_MAP(port, pin)
#else
    #ifdef GPIO_STM32 
    #include "stm32f4xx_hal.h"
            typedef struct {
        GPIO_TypeDef *port;
        uint16_t pin;
    } pin_t;
    #define MAKE_PIN(port, pin) {port, pin}

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
    #else
    #error "MCU_TYPE not defined"
    #endif
#endif

int gpio_read_pin(pin_t pin);
void gpio_write_pin(pin_t pin, int value);

void gpio_set_output_pushpull(pin_t pin);
void gpio_set_output_opendrain(pin_t pin);

void gpio_set_input_floating(pin_t pin);
void gpio_set_input_pullup(pin_t pin);
void gpio_set_input_pulldown(pin_t pin);
