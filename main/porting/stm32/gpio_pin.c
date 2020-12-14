/**
 * gpio_pin.c
 */

#include <string.h>
#include "generic_hal.h"
#include "gpio_pin.h"

int gpio_read_pin(pin_t pin)
{
    return HAL_GPIO_ReadPin(GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    HAL_GPIO_WritePin(GET_PORT(pin), GET_PIN(pin), value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_set_output_pushpull(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = GET_PIN(pin);
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_OUTPUT_PP;
    init_param.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GET_PORT(pin), &init_param);
}

void gpio_set_output_opendrain(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = GET_PIN(pin);
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_OUTPUT_OD;
    init_param.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GET_PORT(pin), &init_param);
}

void gpio_set_input_floating(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = GET_PIN(pin);
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_INPUT;
    init_param.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GET_PORT(pin), &init_param);
}

void gpio_set_input_pullup(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = GET_PIN(pin);
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_INPUT;
    init_param.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GET_PORT(pin), &init_param);
}

void gpio_set_input_pulldown(pin_t pin)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.Pin = GET_PIN(pin);
    init_param.Speed = GPIO_SPEED_FREQ_LOW;
    init_param.Mode = GPIO_MODE_INPUT;
    init_param.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GET_PORT(pin), &init_param);
}