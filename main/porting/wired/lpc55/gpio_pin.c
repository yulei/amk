/**
 * @file gpio_pin.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include "amk_hal.h"
#include "amk_gpio.h"

#define INPUT_FLOATING      0
#define INPUT_PULL_DOWN     1
#define INPUT_PULL_UP       2

#define OUTPUT_PUSHPULL     0
#define OUTPUT_OPENDRAIN    1

int gpio_read_pin(pin_t pin)
{
    return GPIO_PinRead(GPIO, GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    GPIO_PinWrite(GPIO,GET_PORT(pin), GET_PIN(pin), value);
}

static void gpio_set_mode(pin_t pin, uint32_t mode, uint32_t otype, uint32_t pull)
{
    // set direction
    gpio_pin_config_t config = {
        .pinDirection = mode,
        .outputLogic = 0U
    };
    uint32_t p_port = GET_PORT(pin);
    uint32_t p_pin = GET_PIN(pin);
    GPIO_PinInit(GPIO, p_port, p_pin, &config);

    IOCON->PIO[p_port][p_pin] = ((IOCON->PIO[p_port][p_pin] & (~(IOCON_PIO_FUNC_MASK | 
                                                                    IOCON_PIO_MODE_MASK | 
                                                                    IOCON_PIO_DIGIMODE_MASK |
                                                                    IOCON_PIO_OD_MASK)))
                                | IOCON_PIO_FUNC(IOCON_FUNC0)
                                | IOCON_PIO_MODE(pull)
                                | IOCON_PIO_DIGIMODE(1)
                                | IOCON_PIO_OD(otype));
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_mode(pin, kGPIO_DigitalOutput, OUTPUT_PUSHPULL, INPUT_FLOATING);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_mode(pin, kGPIO_DigitalOutput, OUTPUT_OPENDRAIN, INPUT_FLOATING);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_mode(pin, kGPIO_DigitalInput, OUTPUT_PUSHPULL, INPUT_FLOATING);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_mode(pin, kGPIO_DigitalInput, OUTPUT_PUSHPULL, INPUT_PULL_UP);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_mode(pin, kGPIO_DigitalInput, OUTPUT_PUSHPULL, INPUT_PULL_DOWN);
}
