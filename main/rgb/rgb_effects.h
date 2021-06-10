/**
 * rgb_effects.h
 * an simple rgb led effect lib
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "rgb_driver.h"

#ifndef EFFECTS_HUE_STEP
#define EFFECTS_HUE_STEP    8
#endif

#ifndef EFFECTS_SAT_STEP
#define EFFECTS_SAT_STEP    8
#endif

#ifndef EFFECTS_VAL_STEP
#define EFFECTS_VAL_STEP    8
#endif

typedef struct {
    uint8_t enable;
    uint8_t mode;
    uint8_t speed;
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
} rgb_effects_config_t;

typedef void* rgb_effects_t;

rgb_effects_t rgb_effects_init(rgb_driver_t* driver, uint8_t index, uint8_t led_start, uint8_t led_count);
void rgb_effects_uninit(rgb_effects_t effects);
void rgb_effects_task(rgb_effects_t effects);

bool rgb_effects_enabled(rgb_effects_t effects);
void rgb_effects_toggle(rgb_effects_t effects);

void rgb_effects_inc_hue(rgb_effects_t effects);
void rgb_effects_dec_hue(rgb_effects_t effects);
void rgb_effects_inc_sat(rgb_effects_t effects);
void rgb_effects_dec_sat(rgb_effects_t effects);
void rgb_effects_inc_val(rgb_effects_t effects);
void rgb_effects_dec_val(rgb_effects_t effects);
void rgb_effects_inc_speed(rgb_effects_t effects);
void rgb_effects_dec_speed(rgb_effects_t effects);
void rgb_effects_inc_mode(rgb_effects_t effects);
void rgb_effects_dec_mode(rgb_effects_t effects);
