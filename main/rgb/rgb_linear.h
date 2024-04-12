/**
 * @file rgb_linear.h
 * @brief for linear led animation
 */

#pragma once

#include "rgb_common.h"

extern rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM];

void rgb_linear_init(void);
void rgb_linear_task(void);
void rgb_linear_uninit(void);
void rgb_linear_prepare_sleep(void);
void rgb_linear_set_rgb(uint8_t index, uint8_t led, uint8_t r, uint8_t g, uint8_t b);

void rgb_linear_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param);
void rgb_linear_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param);