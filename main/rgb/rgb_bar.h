/**
 * @file rgb_bar.h
 * @brief for linear led animation
 */

#pragma once

#include <stdint.h>

#ifndef RGB_SEGMENT_NUM
    #define RGB_SEGMENT_NUM 1
#endif

typedef struct {
    uint8_t device;
    uint8_t index;
    uint8_t led_start;
    uint8_t led_count;
} rgb_bar_param_t;

extern rgb_bar_param_t g_rgb_bar_params[RGB_SEGMENT_NUM];

void rgb_bar_init(void);
void rgb_bar_task(void);
void rgb_bar_uninit(void);