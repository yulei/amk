/**
 * @file rgb_effect_matrix.h
 * @brief rgb matrix definition
 */

#pragma once

#include "rgb_driver.h"

enum {
    RM_EFFECT_STATIC,
    RM_EFFECT_BREATH,
    RM_EFFECT_GRADIENT,
    RM_EFFECT_RAINBOW,
    RM_EFFECT_GRADIENT_V,
    RM_EFFECT_RAINBOW_V,
    RM_EFFECT_ROTATE,
    RM_EFFECT_SNAKE,
    RM_EFFECT_KEYHIT,
    RM_EFFECT_RANDOM,
    RM_EFFECT_MAX,
};

rgb_effect_t rgb_effect_matrix_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num);
void rgb_effect_matrix_uninit(rgb_effect_t effect);
void rgb_effect_matrix_task(rgb_effect_t effect);

bool rgb_effect_matrix_enabled(rgb_effect_t effect);
void rgb_effect_matrix_toggle(rgb_effect_t effect);

void rgb_effect_matrix_init_mode(rgb_effect_t effect);