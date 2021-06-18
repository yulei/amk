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