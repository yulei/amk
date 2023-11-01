/**
 * @file gpio_pin.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include <string.h>
#include "amk_hal.h"
#include "amk_gpio.h"
#include "amk_printf.h"

int gpio_read_pin(pin_t pin)
{
    return GPIO_ReadInputBit(GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    if (value) {
        GPIO_SetBit(GET_PORT(pin), GET_PIN(pin));
    } else {
        GPIO_ResetBit(GET_PORT(pin), GET_PIN(pin));
    }
}

static void gpio_set_mode(pin_t pin, uint32_t mode, uint32_t speed, uint32_t otype, uint32_t pull)
{
    GPIO_Config_T config = {0};
    config.pin = GET_PIN(pin);
    config.mode = mode;
    config.speed = speed;
    config.otype = otype;
    config.pupd = pull;
    GPIO_Config(GET_PORT(pin), &config);
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_OUT, GPIO_SPEED_100MHz, GPIO_OTYPE_PP, GPIO_PUPD_NOPULL);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_OUT, GPIO_SPEED_100MHz, GPIO_OTYPE_OD, GPIO_PUPD_NOPULL);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_IN, GPIO_SPEED_100MHz, GPIO_OTYPE_PP, GPIO_PUPD_NOPULL);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_IN, GPIO_SPEED_100MHz, GPIO_OTYPE_PP, GPIO_PUPD_UP);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_mode(pin, GPIO_MODE_IN, GPIO_SPEED_100MHz, GPIO_OTYPE_PP, GPIO_PUPD_DOWN);
}
