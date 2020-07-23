/**
 * gpio_pin.c
 *  nrf52 gpio wrapper
 */
#include "gpio_pin.h"
#include "nrf_gpio.h"

int gpio_read_pin(pin_t pin)
{
    return nrf_gpio_pin_read(pin);
}

void gpio_write_pin(pin_t pin, int value)
{
    nrf_gpio_pin_write(pin, value ? 1 : 0);
}

void gpio_set_output_pushpull(pin_t pin)
{
    nrf_gpio_cfg(pin,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}

void gpio_set_output_opendrain(pin_t pin)
{
    nrf_gpio_cfg(pin,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0D1,
        NRF_GPIO_PIN_NOSENSE);
}

void gpio_set_input_floating(pin_t pin)
{
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL);
}

void gpio_set_input_pullup(pin_t pin)
{
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLUP);
}

void gpio_set_input_pulldown(pin_t pin)
{
    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLDOWN);
}