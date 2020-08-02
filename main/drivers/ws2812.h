/**
 * @file ws2812.h
 * @brief ws2812 driver on nrf52
 */

#pragma once

#include <stdint.h>
#include "rgb_color.h"


void ws2812_init(void);

void ws2812_setleds(rgb_led_t* leds, uint16_t number);

void ws2812_uninit(void);
