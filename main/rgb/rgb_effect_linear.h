/**
 * @file rgb_effect_linear.h
 * @brief linear led effects
 */

#pragma once

#include "rgb_driver.h"

enum {
    RL_EFFECT_STATIC,
    RL_EFFECT_BLINK,
    RL_EFFECT_GRADIENT,
    RL_EFFECT_RAINBOW,
    RL_EFFECT_RANDOM,
    RL_EFFECT_BREATH,
    RL_EFFECT_WIPE,
    RL_EFFECT_SCAN,
    RL_EFFECT_CIRCLE,
    RL_EFFECT_MAX,
};


rgb_effect_t rgb_effect_linear_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num);
void rgb_effect_linear_uninit(rgb_effect_t effect);
void rgb_effect_linear_task(rgb_effect_t effect);

bool rgb_effect_linear_enabled(rgb_effect_t effect);
void rgb_effect_linear_toggle(rgb_effect_t effect);

void rgb_effect_linear_init_mode(rgb_effect_t effect);
