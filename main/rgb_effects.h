/**
 * rgb_effects.h
 * an simple rgb led effect lib
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "rgb_driver.h"

#ifndef EFFECTS_LED_NUM
#define EFFECTS_LED_NUM     4
#endif

#ifndef EFFECTS_HUE_STEP
#define EFFECTS_HUE_STEP    8
#endif

#ifndef EFFECTS_SAT_STEP
#define EFFECTS_SAT_STEP    8
#endif

#ifndef EFFECTS_VAL_STEP
#define EFFECTS_VAL_STEP    8
#endif

void rgb_effects_init(rgb_driver_t* driver);
bool rgb_effects_enabled(void);
void rgb_effects_inc_hue(void);
void rgb_effects_dec_hue(void);
void rgb_effects_inc_sat(void);
void rgb_effects_dec_sat(void);
void rgb_effects_inc_val(void);
void rgb_effects_dec_val(void);
void rgb_effects_inc_speed(void);
void rgb_effects_dec_speed(void);
void rgb_effects_inc_mode(void);
void rgb_effects_dec_mode(void);
void rgb_effects_toggle(void);
void rgb_effects_task(void);
