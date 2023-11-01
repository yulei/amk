/**
 * @file gpio_pin.c
 */

#include "amk_gpio.h"

int gpio_read_pin(pin_t pin)
{
    return GPIO_ReadInputDataBit(GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    //GPIO_WriteBit(GET_PORT(pin), GET_PIN(pin), value ? Bit_SET : Bit_RESET);
    if (value) {
        GPIO_SetBits(GET_PORT(pin), GET_PIN(pin));
    } else {
        GPIO_ResetBits(GET_PORT(pin), GET_PIN(pin));
    }
}

static void gpio_set_mode(pin_t pin, uint32_t mode, uint32_t speed)
{
    GPIO_InitTypeDef init_param = {0};
    init_param.GPIO_Pin = GET_PIN(pin);
    init_param.GPIO_Speed = speed;
    init_param.GPIO_Mode = mode;
    GPIO_Init(GET_PORT(pin), &init_param);
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_mode(pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_mode(pin, GPIO_Mode_Out_OD, GPIO_Speed_50MHz);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_mode(pin, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_mode(pin, GPIO_Mode_IPU, GPIO_Speed_50MHz);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_mode(pin, GPIO_Mode_IPD, GPIO_Speed_50MHz);
}

uint32_t gpio_read_port(port_t port)
{
    return GPIO_ReadInputData((GPIO_TypeDef*)port);
}

void gpio_write_port(port_t port, uint32_t value)
{
    GPIO_Write((GPIO_TypeDef*)port, (uint16_t)value);
}