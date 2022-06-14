
/* Copyright 2017 Yang Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/***** rgblight_mode(mode)/rgblight_mode_noeeprom(mode) ****

 old mode number (before 0.6.117) to new mode name table

|-----------------|-----------------------------------|
| old mode number | new mode name                     |
|-----------------|-----------------------------------|
|        1        | RGBLIGHT_MODE_STATIC_LIGHT        |
|        2        | RGBLIGHT_MODE_BREATHING           |
|        3        | RGBLIGHT_MODE_BREATHING + 1       |
|        4        | RGBLIGHT_MODE_BREATHING + 2       |
|        5        | RGBLIGHT_MODE_BREATHING + 3       |
|        6        | RGBLIGHT_MODE_RAINBOW_MOOD        |
|        7        | RGBLIGHT_MODE_RAINBOW_MOOD + 1    |
|        8        | RGBLIGHT_MODE_RAINBOW_MOOD + 2    |
|        9        | RGBLIGHT_MODE_RAINBOW_SWIRL       |
|       10        | RGBLIGHT_MODE_RAINBOW_SWIRL + 1   |
|       11        | RGBLIGHT_MODE_RAINBOW_SWIRL + 2   |
|       12        | RGBLIGHT_MODE_RAINBOW_SWIRL + 3   |
|       13        | RGBLIGHT_MODE_RAINBOW_SWIRL + 4   |
|       14        | RGBLIGHT_MODE_RAINBOW_SWIRL + 5   |
|       15        | RGBLIGHT_MODE_SNAKE               |
|       16        | RGBLIGHT_MODE_SNAKE + 1           |
|       17        | RGBLIGHT_MODE_SNAKE + 2           |
|       18        | RGBLIGHT_MODE_SNAKE + 3           |
|       19        | RGBLIGHT_MODE_SNAKE + 4           |
|       20        | RGBLIGHT_MODE_SNAKE + 5           |
|       21        | RGBLIGHT_MODE_KNIGHT              |
|       22        | RGBLIGHT_MODE_KNIGHT + 1          |
|       23        | RGBLIGHT_MODE_KNIGHT + 2          |
|       24        | RGBLIGHT_MODE_CHRISTMAS           |
|       25        | RGBLIGHT_MODE_STATIC_GRADIENT     |
|       26        | RGBLIGHT_MODE_STATIC_GRADIENT + 1 |
|       27        | RGBLIGHT_MODE_STATIC_GRADIENT + 2 |
|       28        | RGBLIGHT_MODE_STATIC_GRADIENT + 3 |
|       29        | RGBLIGHT_MODE_STATIC_GRADIENT + 4 |
|       30        | RGBLIGHT_MODE_STATIC_GRADIENT + 5 |
|       31        | RGBLIGHT_MODE_STATIC_GRADIENT + 6 |
|       32        | RGBLIGHT_MODE_STATIC_GRADIENT + 7 |
|       33        | RGBLIGHT_MODE_STATIC_GRADIENT + 8 |
|       34        | RGBLIGHT_MODE_STATIC_GRADIENT + 9 |
|       35        | RGBLIGHT_MODE_RGB_TEST            |
|       36        | RGBLIGHT_MODE_ALTERNATING         |
|       37        | RGBLIGHT_MODE_TWINKLE             |
|       38        | RGBLIGHT_MODE_TWINKLE + 1         |
|       39        | RGBLIGHT_MODE_TWINKLE + 2         |
|       40        | RGBLIGHT_MODE_TWINKLE + 3         |
|       41        | RGBLIGHT_MODE_TWINKLE + 4         |
|       42        | RGBLIGHT_MODE_TWINKLE + 5         |
|-----------------|-----------------------------------|
 *****/

#define RGBLIGHT_ANIMATIONS

#ifdef RGBLIGHT_ANIMATIONS
// for backward compatibility
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_ALTERNATING
#endif

#ifdef RGBLIGHT_STATIC_PATTERNS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#endif

// clang-format off

// check dynamic animation effects chose ?
#if  defined(RGBLIGHT_EFFECT_BREATHING)     \
  || defined(RGBLIGHT_EFFECT_RAINBOW_MOOD)  \
  || defined(RGBLIGHT_EFFECT_RAINBOW_SWIRL) \
  || defined(RGBLIGHT_EFFECT_SNAKE)         \
  || defined(RGBLIGHT_EFFECT_KNIGHT)        \
  || defined(RGBLIGHT_EFFECT_CHRISTMAS)     \
  || defined(RGBLIGHT_EFFECT_RGB_TEST)      \
  || defined(RGBLIGHT_EFFECT_ALTERNATING)   \
  || defined(RGBLIGHT_EFFECT_TWINKLE)
#    define RGBLIGHT_USE_TIMER
#endif

// clang-format on

#define _RGBM_SINGLE_STATIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_SINGLE_DYNAMIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_MULTI_STATIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_MULTI_DYNAMIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_TMP_STATIC(sym, msym) RGBLIGHT_MODE_##sym,
#define _RGBM_TMP_DYNAMIC(sym, msym) RGBLIGHT_MODE_##sym,
enum RGBLIGHT_EFFECT_MODE {
    RGBLIGHT_MODE_zero = 0,
#include "qmk/rgb_light/rgblight_modes.h"
    RGBLIGHT_MODE_last
};

#define RGBLIGHT_MODES (RGBLIGHT_MODE_last - 1)

// sample: #define RGBLIGHT_EFFECT_BREATHE_CENTER   1.85

#ifndef RGBLIGHT_EFFECT_BREATHE_MAX
#    define RGBLIGHT_EFFECT_BREATHE_MAX 255 // 0-255
#endif

#ifndef RGBLIGHT_EFFECT_SNAKE_LENGTH
#    define RGBLIGHT_EFFECT_SNAKE_LENGTH 4
#endif

#ifndef RGBLIGHT_EFFECT_KNIGHT_LENGTH
#    define RGBLIGHT_EFFECT_KNIGHT_LENGTH 3
#endif

#ifndef RGBLIGHT_EFFECT_KNIGHT_OFFSET
#    define RGBLIGHT_EFFECT_KNIGHT_OFFSET 0
#endif

#ifndef RGBLIGHT_EFFECT_KNIGHT_LED_NUM
#    define RGBLIGHT_EFFECT_KNIGHT_LED_NUM (state->rgblight_ranges.effect_num_leds)
#endif

#ifndef RGBLIGHT_EFFECT_CHRISTMAS_INTERVAL
#    define RGBLIGHT_EFFECT_CHRISTMAS_INTERVAL 40
#endif

#ifndef RGBLIGHT_EFFECT_CHRISTMAS_STEP
#    define RGBLIGHT_EFFECT_CHRISTMAS_STEP 2
#endif

#ifndef RGBLIGHT_EFFECT_TWINKLE_LIFE
#    define RGBLIGHT_EFFECT_TWINKLE_LIFE 200
#endif

#ifndef RGBLIGHT_EFFECT_TWINKLE_PROBABILITY
#    define RGBLIGHT_EFFECT_TWINKLE_PROBABILITY 1 / 127
#endif

#ifndef RGBLIGHT_HUE_STEP
#    define RGBLIGHT_HUE_STEP 8
#endif
#ifndef RGBLIGHT_SAT_STEP
#    define RGBLIGHT_SAT_STEP 17
#endif
#ifndef RGBLIGHT_VAL_STEP
#    define RGBLIGHT_VAL_STEP 17
#endif
#ifndef RGBLIGHT_LIMIT_VAL
#    define RGBLIGHT_LIMIT_VAL 255
#endif

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////////////////////

#include "rgb_driver.h"

rgb_effect_t rgb_effect_linear_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num);
void rgb_effect_linear_uninit(rgb_effect_t effect);
void rgb_effect_linear_task(rgb_effect_t effect);

bool rgb_effect_linear_enabled(rgb_effect_t effect);
void rgb_effect_linear_toggle(rgb_effect_t effect);

void rgb_effect_linear_init_mode(rgb_effect_t effect);
