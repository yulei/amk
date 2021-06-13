/**
 * @file rgb_effect_linear.h
 * @brief linear led effects
 */

#pragma once

#include "rgb_driver.h"

rgb_effect_t rgb_effect_linear_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num);
void rgb_effect_linear_uninit(rgb_effect_t effect);
void rgb_effect_linear_task(rgb_effect_t effect);

bool rgb_effect_linear_enabled(rgb_effect_t effect);
void rgb_effect_linear_toggle(rgb_effect_t effect);

void rgb_effect_linear_inc_hue(rgb_effect_t effect);
void rgb_effect_linear_dec_hue(rgb_effect_t effect);
void rgb_effect_linear_inc_sat(rgb_effect_t effect);
void rgb_effect_linear_dec_sat(rgb_effect_t effect);
void rgb_effect_linear_inc_val(rgb_effect_t effect);
void rgb_effect_linear_dec_val(rgb_effect_t effect);
void rgb_effect_linear_inc_speed(rgb_effect_t effect);
void rgb_effect_linear_dec_speed(rgb_effect_t effect);
void rgb_effect_linear_inc_mode(rgb_effect_t effect);
void rgb_effect_linear_dec_mode(rgb_effect_t effect);
