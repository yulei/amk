/**
 * @file rgb_indicator.h
 */

#pragma once

#include <stdint.h>

extern uint8_t g_rgb_indicator_index[RGB_INDICATOR_LED_NUM];

void rgb_indicator_init(void);
void rgb_indicator_set(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void rgb_indicator_task(void);
void rgb_indicator_uninit(void);