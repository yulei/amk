/**
 * @file gpio_pin.c
 */

#include <string.h>
#include "generic_hal.h"
#include "amk_gpio.h"
#include "amk_printf.h"

int gpio_read_pin(pin_t pin)
{
    return gpio_input_bit_get(GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    if (value) {
        gpio_bit_set(GET_PORT(pin), GET_PIN(pin));
    } else {
        gpio_bit_reset(GET_PORT(pin), GET_PIN(pin));
    }
}

static void gpio_set_mode(pin_t pin, uint32_t mode, uint32_t speed)
{
    gpio_init(GET_PORT(pin), mode, speed, GET_PORT(pin));
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_IPD, GPIO_OSPEED_10MHZ);
}

uint32_t gpio_read_port(port_t port)
{
    return gpio_input_port_get(port);
}

void gpio_write_port(port_t port, uint32_t value)
{
    gpio_port_write(port, (uint16_t)value);
}