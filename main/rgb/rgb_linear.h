/**
 * @file rgb_linear.h
 * @brief for linear led animation
 */

#pragma once

#include <stdint.h>

typedef struct {
    uint8_t device;
    uint8_t segment;
    uint8_t led_start;
    uint8_t led_num;
} rgb_linear_param_t;

extern rgb_linear_param_t g_rgb_linear_params[RGB_SEGMENT_NUM];

void rgb_linear_init(void);
void rgb_linear_task(void);
void rgb_linear_uninit(void);