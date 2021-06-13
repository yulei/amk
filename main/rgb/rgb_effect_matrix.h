/**
 * @file rgb_effect_matrix.h
 * @brief rgb matrix definition
 */

#pragma once

#include "rgb_driver.h"

typedef void* rgb_effect_matrix_t;

rgb_effect_matrix_t rgb_effect_matrix_init(rgb_driver_t *driver, rgb_config_t *config);
void rgb_effect_matrix_task(rgb_effect_matrix_t effect);
bool rgb_effect_matrix_enabled(rgb_effect_matrix_t effect);
void rgb_effect_matrix_toggle(rgb_effect_matrix_t effect);

void rgb_effect_matrix_inc_hue(rgb_effect_matrix_t effect);
void rgb_effect_matrix_dec_hue(rgb_effect_matrix_t effect);
void rgb_effect_matrix_inc_sat(rgb_effect_matrix_t effect);
void rgb_effect_matrix_dec_sat(rgb_effect_matrix_t effect);
void rgb_effect_matrix_inc_val(rgb_effect_matrix_t effect);
void rgb_effect_matrix_dec_val(rgb_effect_matrix_t effect);
void rgb_effect_matrix_inc_speed(rgb_effect_matrix_t effect);
void rgb_effect_matrix_dec_speed(rgb_effect_matrix_t effect);
void rgb_effect_matrix_inc_mode(rgb_effect_matrix_t effect);
void rgb_effect_matrix_dec_mode(rgb_effect_matrix_t effect);