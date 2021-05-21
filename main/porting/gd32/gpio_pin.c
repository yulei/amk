/**
 * gpio_pin.c
 */

#include <string.h>
#include "generic_hal.h"
#include "gpio_pin.h"

int gpio_read_pin(pin_t pin)
{
    return gpio_input_bit_get(GET_PORT(pin), GET_PIN(pin)) ? 1 : 0;
}

void gpio_write_pin(pin_t pin, int value)
{
    gpio_bit_write(GET_PORT(pin), GET_PIN(pin), value ? SET:RESET);
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_init(GET_PORT(pin), GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GET_PIN(pin));
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_init(GET_PORT(pin), GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GET_PIN(pin));
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_init(GET_PORT(pin), GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GET_PIN(pin));
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_init(GET_PORT(pin), GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GET_PIN(pin));
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_init(GET_PORT(pin), GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GET_PIN(pin));
}