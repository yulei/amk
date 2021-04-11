/**
 * gpio_pin.c
 */

#include "generic_hal.h"
#include "gpio_pin.h"

#define PIN_CVT(pin)    GPIO_PIN_DATA((GET_PORT(pin)), (GET_PIN(pin)))
#define PORT_CVT(pin)   (GPIO_T *)(GPIO_BASE + (GET_PORT(pin))*0x40)

int gpio_read_pin(pin_t pin)
{
    return PIN_CVT(pin);
}

void gpio_write_pin(pin_t pin, int value)
{
    PIN_CVT(pin) = value;
}

void gpio_set_output_pushpull(pin_t pin)
{
    GPIO_SetMode(PORT_CVT(pin), GET_PIN(pin), GPIO_MODE_OUTPUT);
}

void gpio_set_output_opendrain(pin_t pin)
{
    GPIO_SetMode(PORT_CVT(pin), GET_PIN(pin), GPIO_MODE_OPEN_DRAIN);
}

void gpio_set_input_floating(pin_t pin)
{
    GPIO_SetMode(PORT_CVT(pin), GET_PIN(pin), GPIO_MODE_INPUT);
}

void gpio_set_input_pullup(pin_t pin)
{
    GPIO_SetMode(PORT_CVT(pin), GET_PIN(pin), GPIO_MODE_QUASI);
}

void gpio_set_input_pulldown(pin_t pin)
{
    GPIO_SetMode(PORT_CVT(pin), GET_PIN(pin), GPIO_MODE_INPUT);
}