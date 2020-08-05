/**
 * @file ws2812.h
 * @brief ws2812 driver on nrf52
 */

#pragma once

#include <stdint.h>
#include "rgb_color.h"
#include "gpio_pin.h"

#ifndef WS2812_LED_NUM
    #define WS2812_LED_NUM RGB_LED_NUM
#endif


void ws2812_init(pin_t pin);

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
void ws2812_update_buffers(pin_t pin);

void ws2812_uninit(pin_t pin);
