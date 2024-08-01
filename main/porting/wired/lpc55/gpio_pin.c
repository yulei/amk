/**
 * @file gpio_pin.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include "amk_hal.h"
#include "amk_gpio.h"

int gpio_read_pin(pin_t pin)
{
    return GPIO_PinRead(GPIO, GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    GPIO_PinWrite(GPIO,GET_PORT(pin), GET_PIN(pin), value);
}

static void gpio_set_mode(pin_t pin, uint32_t mode, uint32_t strength, uint32_t otype, uint32_t pull)
{
    gpio_init_type config = {0};
    config.gpio_pins            = GET_PIN(pin);
    config.gpio_out_type        = otype;
    config.gpio_pull            = pull;
    config.gpio_mode            = mode;
    config.gpio_drive_strength  = strength;
    gpio_init(GET_PORT(pin), &config);
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_OUTPUT, GPIO_DRIVE_STRENGTH_STRONGER, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_NONE);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_OUTPUT, GPIO_DRIVE_STRENGTH_STRONGER, GPIO_OUTPUT_OPEN_DRAIN, GPIO_PULL_NONE);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_INPUT, GPIO_DRIVE_STRENGTH_STRONGER, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_NONE);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_INPUT, GPIO_DRIVE_STRENGTH_STRONGER, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_UP);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_INPUT, GPIO_DRIVE_STRENGTH_STRONGER, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_DOWN);
}
