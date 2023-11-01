/**
 * @file gpio_pin.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "amk_gpio.h"
#include "amk_printf.h"

int gpio_read_pin(pin_t pin)
{
    if (*(volatile uint32_t*)pin) {
        return 1;
    } else {
        return 0;
    }
}

void gpio_write_pin(pin_t pin, int value)
{
    *(volatile uint32_t*)pin = value;
}

static GPIO_T* ports[] = { PA, PB, PC, PD, PE, PF, PG, PH};

GPIO_T* map_port(pin_t pin)
{
//    #define GET_PIN(port_pin)   PIN_P((((port_pin)%0x40)>>2))
//    #define GET_PORT(port_pin)  ((GPIO_T*)(GPIOA_BASE+(((port_pin)/0x40)*0x40)))
    uint32_t port_index = (pin-GPIO_PIN_DATA_BASE)/0x40;
    
    return ports[port_index];
}

void gpio_set_output_pushpull(pin_t pin)
{
    GPIO_SetMode(map_port(pin), GET_PIN(pin), GPIO_MODE_OUTPUT);
}

void gpio_set_output_opendrain(pin_t pin)
{
    GPIO_SetMode(map_port(pin), GET_PIN(pin), GPIO_MODE_OPEN_DRAIN);
}

void gpio_set_input_floating(pin_t pin)
{
    GPIO_SetMode(map_port(pin), GET_PIN(pin), GPIO_MODE_INPUT);
    GPIO_SetPullCtl(map_port(pin), GET_PIN(pin), GPIO_PUSEL_DISABLE);
}

void gpio_set_input_pullup(pin_t pin)
{
    GPIO_SetMode(map_port(pin), GET_PIN(pin), GPIO_MODE_INPUT);
    GPIO_SetPullCtl(map_port(pin), GET_PIN(pin), GPIO_PUSEL_PULL_UP);
}

void gpio_set_input_pulldown(pin_t pin)
{
    GPIO_SetMode(map_port(pin), GET_PIN(pin), GPIO_MODE_INPUT);
    GPIO_SetPullCtl(map_port(pin), GET_PIN(pin), GPIO_PUSEL_PULL_DOWN);
}
