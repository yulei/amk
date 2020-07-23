/**
 * gpio_pin.h
 * generic pin implementation
 */

#pragma once

#if defined(MCU_NRF5X)
#include "nrf_gpio.h"
typedef uint32_t pin_t;
#define MAKE_PIN(port, pin) NRF_GPIO_PIN_MAP(port, pin)

#elif defined(MCU_STM32F4)
#include "stm32f4xx_hal.h"
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} pin_t;

#define MAKE_PIN(port, pin) {port, pin}

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
