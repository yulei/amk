/**
 * gpio_pin.h
 * generic pin implementation
 */

#pragma once

//#if CFG_TUSB_MCU == OPT_MCU_NRF5X

//#elif CFG_TUSB_MCU == OPT_MCU_STM32F4
#include "stm32f4xx_hal.h"
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} pin_t;

int gpio_read_pin(pin_t pin);
void gpio_write_pin(pin_t pin, int value);

void gpio_set_output_pushpull(pin_t pin);
void gpio_set_output_opendrain(pin_t pin);

void gpio_set_input_floating(pin_t pin);
void gpio_set_input_pullup(pin_t pin);
void gpio_set_input_pulldown(pin_t pin);
/*
#else
#error "CFG_TUSB_MCU not set"
#endif
*/