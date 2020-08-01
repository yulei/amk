/**
 * @file ws2812.h
 * @brief ws2812 driver on nrf52
 */

#pragma once

#include <stdint.h>
#include "color.h"

void ws2812_setleds(LED_TYPE* leds, uint16_t number);

void ws2812_uninit(void);
