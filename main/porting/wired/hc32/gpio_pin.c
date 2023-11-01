/**
 * @file gpio_pin.c
 */

#include <string.h>
#include "amk_gpio.h"
#include "amk_printf.h"

int gpio_read_pin(pin_t pin)
{
    return PORT_GetBit(GET_PORT(pin), GET_PIN(pin));
}

void gpio_write_pin(pin_t pin, int value)
{
    if (value) {
        PORT_SetBits(GET_PORT(pin), GET_PIN(pin));
    } else {
        PORT_ResetBits(GET_PORT(pin), GET_PIN(pin));
    }
}

static void gpio_set_mode(pin_t pin, uint32_t mode, uint32_t output_type, uint32_t drv, uint32_t pull)
{
    stc_port_init_t init_param = {0};
    init_param.enPinMode = mode;
    if (init_param.enPinMode == Pin_Mode_In) {
        init_param.enPullUp = pull;
    } else if (init_param.enPinMode == Pin_Mode_Out) {
        init_param.enPinOType = output_type;
        init_param.enPinDrv = drv;
    } else {
        init_param.enPinOType = Pin_Mode_Ana;
    }

    PORT_Init(GET_PORT(pin), GET_PIN(pin), &init_param);
}

void gpio_set_output_pushpull(pin_t pin)
{
    gpio_set_mode(pin, Pin_Mode_Out, Pin_OType_Cmos, Pin_Drv_L, 0);
}

void gpio_set_output_opendrain(pin_t pin)
{
    gpio_set_mode(pin, Pin_Mode_Out, Pin_OType_Od, Pin_Drv_L, 0);
}

void gpio_set_input_floating(pin_t pin)
{
    gpio_set_mode(pin, Pin_Mode_In, 0, 0, 0);
}

void gpio_set_input_pullup(pin_t pin)
{
    gpio_set_mode(pin, Pin_Mode_In, 0, 0, 1);
}

void gpio_set_input_pulldown(pin_t pin)
{
    gpio_set_mode(pin, Pin_Mode_In, 0, 0, 0);
    // not support
    amk_printf("HC32 gpio do not support input pulldown\n");
}
