/**
 * @file rgb_matrix.h
 */

#pragma once

#include "rgb_common.h"

extern rgb_param_t g_rgb_matrix_params[RGB_MATRIX_NUM];

void rgb_matrix_init_stub(void);
void rgb_matrix_task_stub(void);
void rgb_matrix_uninit_stub(void);
void rgb_matrix_prepare_sleep_stub(void);
bool rgb_matrix_enabled_stub(uint8_t index);
void rgb_matrix_set_rgb_stub(uint8_t index, uint8_t led, uint8_t r, uint8_t g, uint8_t b);

void rgb_matrix_step_stub(void);
void rgb_matrix_step_reverse_stub(void);


#include "rgb_matrix.h"