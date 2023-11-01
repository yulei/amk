/**
 * gpio_pin.c
 */

#include <string.h>
#include "amk_hal.h"
#include "amk_gpio.h"

int gpio_read_pin(pin_t pin)
{
    return gpio_get_pin_level(pin);
}

void gpio_write_pin(pin_t pin, int value)
{
    gpio_set_pin_level(pin, value);
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_pin_direction(pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_OFF);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_pin_direction(pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_DOWN);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_OFF);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_UP);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_DOWN);
}