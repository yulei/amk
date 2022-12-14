/**
 * @file rgb_matrix.h
 */

#pragma once

#include "rgb_common.h"

extern rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM];

void rgb_matrix_init(void);
void rgb_matrix_task(void);
void rgb_matrix_uninit(void);
void rgb_matrix_prepare_sleep(void);
bool rgb_matrix_enabled(uint8_t index);
void rgb_matrix_set_rgb(uint8_t index, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
