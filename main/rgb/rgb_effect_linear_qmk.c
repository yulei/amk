/* Copyright 2016-2017 Yang Liu
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
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "wait.h"
#include "progmem.h"
#include "timer.h"

#include "qmk/lib8tion/lib8tion.h"

#include "rgb_effect_linear_qmk.h"
#include "rgb_led.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#ifndef RGB_EFFECT_LINEAR_NUM
    #define RGB_EFFECT_LINEAR_NUM RGB_SEGMENT_NUM
#endif

#ifndef RGB_EFFECTS_DEBUG
#define RGB_EFFECTS_DEBUG 1
#endif

#if RGB_EFFECTS_DEBUG
#define rgb_effects_debug  amk_printf
#else
#define rgb_effects_debug(...)
#endif

typedef struct _rgblight_status_t {
    uint8_t base_mode;
    bool    timer_enabled;
} rgblight_status_t;

typedef struct _rgblight_ranges_t {
    uint8_t clipping_start_pos;
    uint8_t clipping_num_leds;
    uint8_t effect_start_pos;
    uint8_t effect_end_pos;
    uint8_t effect_num_leds;
} rgblight_ranges_t;

typedef struct _animation_status_t {
    uint16_t last_timer;
    uint8_t  delta; /* mode - base_mode */
    bool     restart;
    union {
        uint16_t pos16;
        uint8_t  pos;
        int8_t   current_hue;
        uint16_t current_offset;
    };
} animation_status_t;

typedef struct {
    rgb_config_t        *config;
    uint8_t             index;  // config index
    uint8_t             led_start;
    uint8_t             led_num;

    rgblight_status_t   rgblight_status;
    animation_status_t  animation_status;
    rgblight_ranges_t   rgblight_ranges;
} rgb_linear_state_t;

static rgb_linear_state_t effects_state[RGB_EFFECT_LINEAR_NUM];

static void effects_set_color(rgb_linear_state_t *state, uint8_t index, uint8_t hue, uint8_t sat, uint8_t val);
static void effects_set_color_all(rgb_linear_state_t *state, uint8_t hue, uint8_t sat, uint8_t val);
static void effects_set_color_rgb(rgb_linear_state_t *state, uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
static void effects_set_color_all_rgb(rgb_linear_state_t *state, uint8_t red, uint8_t green, uint8_t blue);

#define sync_timer_init()
#define sync_timer_clear()
#define sync_timer_update(t)
#define sync_timer_read() timer_read()
#define sync_timer_read32() timer_read32()
#define sync_timer_elapsed(t) timer_elapsed(t)
#define sync_timer_elapsed32(t) timer_elapsed32(t)

// Utility functions to check if a future time has expired & autmatically handle time wrapping if checked / reset frequently (half of max value)
#define timer_expired(current, future) ((uint16_t)(current - future) < UINT16_MAX / 2)
#define timer_expired32(current, future) ((uint32_t)(current - future) < UINT32_MAX / 2)

#ifndef MIN
#    define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#    define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define _RGBM_SINGLE_STATIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_SINGLE_DYNAMIC(sym)
#define _RGBM_MULTI_STATIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_MULTI_DYNAMIC(sym)
#define _RGBM_TMP_STATIC(sym, msym) RGBLIGHT_MODE_##sym,
#define _RGBM_TMP_DYNAMIC(sym, msym)
static uint8_t static_effect_table[] = {
#include "qmk/rgb_light/rgblight_modes.h"
};

#define _RGBM_SINGLE_STATIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_SINGLE_DYNAMIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_MULTI_STATIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_MULTI_DYNAMIC(sym) RGBLIGHT_MODE_##sym,
#define _RGBM_TMP_STATIC(sym, msym) RGBLIGHT_MODE_##msym,
#define _RGBM_TMP_DYNAMIC(sym, msym) RGBLIGHT_MODE_##msym,
static uint8_t mode_base_table[] = {
    0, // RGBLIGHT_MODE_zero
#include "qmk/rgb_light/rgblight_modes.h"
};

#if !defined(RGBLIGHT_DEFAULT_MODE)
#    define RGBLIGHT_DEFAULT_MODE RGBLIGHT_MODE_STATIC_LIGHT
#endif

#if !defined(RGBLIGHT_DEFAULT_HUE)
#    define RGBLIGHT_DEFAULT_HUE 0
#endif

#if !defined(RGBLIGHT_DEFAULT_SAT)
#    define RGBLIGHT_DEFAULT_SAT UINT8_MAX
#endif

#if !defined(RGBLIGHT_DEFAULT_VAL)
#    define RGBLIGHT_DEFAULT_VAL RGBLIGHT_LIMIT_VAL
#endif

#if !defined(RGBLIGHT_DEFAULT_SPD)
#    define RGBLIGHT_DEFAULT_SPD 0
#endif

static inline int is_static_effect(uint8_t mode) {
    return memchr(static_effect_table, mode, sizeof(static_effect_table)) != NULL;
}

#ifdef RGBLIGHT_EFFECT_STATIC_GRADIENT
__attribute__((weak)) const uint8_t RGBLED_GRADIENT_RANGES[] PROGMEM = {255, 170, 127, 85, 64};
#endif

#ifdef RGBLIGHT_SLEEP
static bool is_suspended;
static bool pre_suspend_enabled;
#endif

#if defined(RGBLIGHT_EFFECT_BREATHING) || defined(RGBLIGHT_EFFECT_RAINBOW_MOOD) || defined(RGBLIGHT_EFFECT_RAINBOW_SWIRL) || defined(RGBLIGHT_EFFECT_SNAKE) || defined(RGBLIGHT_EFFECT_KNIGHT) || defined(RGBLIGHT_EFFECT_TWINKLE)

static uint8_t get_interval_time(const uint8_t *default_interval_address, uint8_t velocikey_min, uint8_t velocikey_max) {
    return
#    ifdef VELOCIKEY_ENABLE
        velocikey_enabled() ? velocikey_match_speed(velocikey_min, velocikey_max) :
#    endif
                            pgm_read_byte(default_interval_address);
}

#endif


#ifdef RGBLIGHT_SLEEP

void rgblight_suspend(void) {
    rgblight_timer_disable();
    if (!is_suspended) {
        is_suspended        = true;
        pre_suspend_enabled = rgblight_config.enable;

#    ifdef RGBLIGHT_LAYER_BLINK
        // make sure any layer blinks don't come back after suspend
        rgblight_status.enabled_layer_mask &= ~_blinking_layer_mask;
        _blinking_layer_mask = 0;
#    endif

        rgblight_disable_noeeprom();
    }
}

void rgblight_wakeup(void) {
    is_suspended = false;

    if (pre_suspend_enabled) {
        rgblight_enable_noeeprom();
    }
#    ifdef RGBLIGHT_LAYERS_OVERRIDE_RGB_OFF
    // Need this or else the LEDs won't be set
    else if (rgblight_status.enabled_layer_mask != 0) {
        rgblight_set();
    }
#    endif

    rgblight_timer_enable();
}

#endif

#ifdef RGBLIGHT_USE_TIMER

//typedef void (*effect_func_t)(animation_status_t *anim);
typedef void (*effect_func_t)(rgb_linear_state_t *state);

// Animation timer -- use system timer (AVR Timer0)
void rgblight_timer_init(rgb_linear_state_t *state) {
    state->rgblight_status.timer_enabled = false;
}
void rgblight_timer_enable(rgb_linear_state_t *state) {
    if (!is_static_effect(state->config->mode)) {
        state->rgblight_status.timer_enabled = true;
    }
    state->animation_status.last_timer = sync_timer_read();
}
void rgblight_timer_disable(rgb_linear_state_t *state) {
    state->rgblight_status.timer_enabled = false;
}
void rgblight_timer_toggle(rgb_linear_state_t *state) {
    if (state->rgblight_status.timer_enabled) {
        rgblight_timer_disable(state);
    } else {
        rgblight_timer_enable(state);
    }
}

static void rgblight_effect_dummy(rgb_linear_state_t *state) {
    // do nothing
    /********
    dprintf("rgblight_task() what happened?\n");
    dprintf("is_static_effect %d\n", is_static_effect(rgblight_config.mode));
    dprintf("mode = %d, base_mode = %d, timer_enabled %d, ",
            rgblight_config.mode, rgblight_status.base_mode,
            rgblight_status.timer_enabled);
    dprintf("last_timer = %d\n",anim->last_timer);
    **/
}

#endif /* RGBLIGHT_USE_TIMER */

#if defined(RGBLIGHT_EFFECT_BREATHING) || defined(RGBLIGHT_EFFECT_TWINKLE)

#    ifndef RGBLIGHT_EFFECT_BREATHE_CENTER
#        ifndef RGBLIGHT_BREATHE_TABLE_SIZE
#            define RGBLIGHT_BREATHE_TABLE_SIZE 256 // 256 or 128 or 64
#        endif
#        include "qmk/rgb_light/rgblight_breathe_table.h"
#    endif

static uint8_t breathe_calc(uint8_t pos) {
    // http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
#    ifdef RGBLIGHT_EFFECT_BREATHE_TABLE
    return pgm_read_byte(&rgblight_effect_breathe_table[pos / table_scale]);
#    else
    return (exp(sin((pos / 255.0) * M_PI)) - RGBLIGHT_EFFECT_BREATHE_CENTER / M_E) * (RGBLIGHT_EFFECT_BREATHE_MAX / (M_E - 1 / M_E));
#    endif
}

#endif

// Effects
#ifdef RGBLIGHT_EFFECT_BREATHING

__attribute__((weak)) const uint8_t RGBLED_BREATHING_INTERVALS[] PROGMEM = {30, 20, 10, 5};

void rgblight_effect_breathing(rgb_linear_state_t *state) {
    uint8_t val = breathe_calc(state->animation_status.pos);
    effects_set_color_all(state, state->config->hue, state->config->sat, val);
    //rgblight_sethsv_noeeprom_old(rgblight_config.hue, rgblight_config.sat, val);
    state->animation_status.pos ++;
}
#endif

#ifdef RGBLIGHT_EFFECT_RAINBOW_MOOD
__attribute__((weak)) const uint8_t RGBLED_RAINBOW_MOOD_INTERVALS[] PROGMEM = {120, 60, 30};

void rgblight_effect_rainbow_mood(rgb_linear_state_t *state) {
    effects_set_color_all(state, state->animation_status.current_hue, state->config->sat, state->config->val);
    //rgblight_sethsv_noeeprom_old(anim->current_hue, rgblight_config.sat, rgblight_config.val);
    state->animation_status.current_hue++;
}
#endif

#ifdef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    ifndef RGBLIGHT_RAINBOW_SWIRL_RANGE
#        define RGBLIGHT_RAINBOW_SWIRL_RANGE 255
#    endif

__attribute__((weak)) const uint8_t RGBLED_RAINBOW_SWIRL_INTERVALS[] PROGMEM = {100, 50, 20};

void rgblight_effect_rainbow_swirl(rgb_linear_state_t *state) {
    uint8_t hue;
    uint8_t i;

    for (i = 0; i < state->rgblight_ranges.effect_num_leds; i++) {
        hue = (RGBLIGHT_RAINBOW_SWIRL_RANGE / state->rgblight_ranges.effect_num_leds * i + state->animation_status.current_hue);
        effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, hue, state->config->sat, state->config->val);
        //sethsv(hue, rgblight_config.sat, rgblight_config.val, (LED_TYPE *)&led[i + rgblight_ranges.effect_start_pos]);
    }
    //rgblight_set();

    if (state->animation_status.delta % 2) {
        state->animation_status.current_hue++;
    } else {
        state->animation_status.current_hue--;
    }
}
#endif

#ifdef RGBLIGHT_EFFECT_SNAKE
__attribute__((weak)) const uint8_t RGBLED_SNAKE_INTERVALS[] PROGMEM = {100, 50, 20};

void rgblight_effect_snake(rgb_linear_state_t *state) {
    static uint8_t pos = 0;
    uint8_t        i, j;
    int8_t         k;
    int8_t         increment = 1;

    if (state->animation_status.delta % 2) {
        increment = -1;
    }

#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
    if (anim->pos == 0) { // restart signal
        if (increment == 1) {
            pos = rgblight_ranges.effect_num_leds - 1;
        } else {
            pos = 0;
        }
        anim->pos = 1;
    }
#    endif

    for (i = 0; i < state->rgblight_ranges.effect_num_leds; i++) {
        //LED_TYPE *ledp = led + i + rgblight_ranges.effect_start_pos;
        //ledp->r        = 0;
        //ledp->g        = 0;
        //ledp->b        = 0;
#    ifdef RGBW
        ledp->w = 0;
#    endif
        for (j = 0; j < RGBLIGHT_EFFECT_SNAKE_LENGTH; j++) {
            k = pos + j * increment;
            if (k > state->rgblight_ranges.effect_num_leds){//RGBLED_NUM) {
                k = k % state->rgblight_ranges.effect_num_leds;//RGBLED_NUM;
            }
            if (k < 0) {
                k = k + state->rgblight_ranges.effect_num_leds;
            }
            if (i == k) {
                //sethsv(rgblight_config.hue, rgblight_config.sat, (uint8_t)(rgblight_config.val * (RGBLIGHT_EFFECT_SNAKE_LENGTH - j) / RGBLIGHT_EFFECT_SNAKE_LENGTH), ledp);
                effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, state->config->hue, state->config->sat, (uint8_t)(state->config->val * (RGBLIGHT_EFFECT_SNAKE_LENGTH - j) / RGBLIGHT_EFFECT_SNAKE_LENGTH));
            }
        }
    }
    //rgblight_set();
    if (increment == 1) {
        if (pos - 1 < 0) {
            pos = state->rgblight_ranges.effect_num_leds - 1;
#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
            anim->pos = 0;
#    endif
        } else {
            pos -= 1;
#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
            anim->pos = 1;
#    endif
        }
    } else {
        pos = (pos + 1) % state->rgblight_ranges.effect_num_leds;
#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
        anim->pos = pos;
#    endif
    }
}
#endif

#ifdef RGBLIGHT_EFFECT_KNIGHT
__attribute__((weak)) const uint8_t RGBLED_KNIGHT_INTERVALS[] PROGMEM = {127, 63, 31};

void rgblight_effect_knight(rgb_linear_state_t *state) {
    static int8_t low_bound  = 0;
    static int8_t high_bound = RGBLIGHT_EFFECT_KNIGHT_LENGTH - 1;
    static int8_t increment  = 1;
    uint8_t       i, cur;

#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
    if (anim->pos == 0) { // restart signal
        anim->pos  = 1;
        low_bound  = 0;
        high_bound = RGBLIGHT_EFFECT_KNIGHT_LENGTH - 1;
        increment  = 1;
    }
#    endif
    // Set all the LEDs to 0
    for (i = state->rgblight_ranges.effect_start_pos; i < state->rgblight_ranges.effect_end_pos; i++) {
        effects_set_color_rgb(state, i, 0, 0, 0);
//        led[i].r = 0;
//        led[i].g = 0;
//        led[i].b = 0;
#    ifdef RGBW
        led[i].w = 0;
#    endif
    }
    // Determine which LEDs should be lit up
    for (i = 0; i < RGBLIGHT_EFFECT_KNIGHT_LED_NUM; i++) {
        cur = (i + RGBLIGHT_EFFECT_KNIGHT_OFFSET) % state->rgblight_ranges.effect_num_leds + state->rgblight_ranges.effect_start_pos;

        if (i >= low_bound && i <= high_bound) {
            effects_set_color(state, cur, state->config->hue, state->config->sat, state->config->val);
            //sethsv(rgblight_config.hue, rgblight_config.sat, rgblight_config.val, (LED_TYPE *)&led[cur]);
        } else {
            effects_set_color_rgb(state, cur, 0, 0, 0);
            //led[cur].r = 0;
            //led[cur].g = 0;
            //led[cur].b = 0;
#    ifdef RGBW
            led[cur].w = 0;
#    endif
        }
    }
    //rgblight_set();

    // Move from low_bound to high_bound changing the direction we increment each
    // time a boundary is hit.
    low_bound += increment;
    high_bound += increment;

    if (high_bound <= 0 || low_bound >= RGBLIGHT_EFFECT_KNIGHT_LED_NUM - 1) {
        increment = -increment;
#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
        if (increment == 1) {
            anim->pos = 0;
        }
#    endif
    }
}
#endif

#ifdef RGBLIGHT_EFFECT_CHRISTMAS
#    define CUBED(x) ((x) * (x) * (x))

/**
 * Christmas lights effect, with a smooth animation between red & green.
 */
void rgblight_effect_christmas(rgb_linear_state_t *state) {
    static int8_t increment = 1;
    const uint8_t max_pos   = 32;
    const uint8_t hue_green = 85;

    uint32_t xa;
    uint8_t  hue, val;
    uint8_t  i;

    // The effect works by animating anim->pos from 0 to 32 and back to 0.
    // The pos is used in a cubic bezier formula to ease-in-out between red and green, leaving the interpolated colors visible as short as possible.
    xa  = CUBED((uint32_t)state->animation_status.pos);
    hue = ((uint32_t)hue_green) * xa / (xa + CUBED((uint32_t)(max_pos - state->animation_status.pos)));
    // Additionally, these interpolated colors get shown with a slightly darker value, to make them less prominent than the main colors.
    val = 255 - (3 * (hue < hue_green / 2 ? hue : hue_green - hue) / 2);

    for (i = 0; i < state->rgblight_ranges.effect_num_leds; i++) {
        uint8_t local_hue = (i / RGBLIGHT_EFFECT_CHRISTMAS_STEP) % 2 ? hue : hue_green - hue;
        //sethsv(local_hue, rgblight_config.sat, val, (LED_TYPE *)&led[i + rgblight_ranges.effect_start_pos]);
        effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, local_hue, state->config->sat, val);
    }
    //rgblight_set();

    if (state->animation_status.pos == 0) {
        increment = 1;
    } else if (state->animation_status.pos == max_pos) {
        increment = -1;
    }
    state->animation_status.pos += increment;
}
#endif

#ifdef RGBLIGHT_EFFECT_RGB_TEST
__attribute__((weak)) const uint16_t RGBLED_RGBTEST_INTERVALS[] PROGMEM = {1024};

void rgblight_effect_rgbtest(rgb_linear_state_t *state) {
    static uint8_t maxval = 0;
    uint8_t        g;
    uint8_t        r;
    uint8_t        b;

    if (maxval == 0) {
        //LED_TYPE tmp_led;
        //sethsv(0, 255, RGBLIGHT_LIMIT_VAL, &tmp_led);
        //maxval = tmp_led.r;
        HSV hsv = {0,255,RGBLIGHT_LIMIT_VAL};
        RGB rgb = hsv_to_rgb(hsv);
        maxval = rgb.r;
    }
    g = r = b = 0;
    switch (state->animation_status.pos) {
        case 0:
            r = maxval;
            break;
        case 1:
            g = maxval;
            break;
        case 2:
            b = maxval;
            break;
    }
    //rgblight_setrgb(r, g, b);
    effects_set_color_all_rgb(state, r, g, b);
    state->animation_status.pos = (state->animation_status.pos + 1) % 3;
}
#endif

#ifdef RGBLIGHT_EFFECT_ALTERNATING
void rgblight_effect_alternating(rgb_linear_state_t *state) {
    for (int i = 0; i < state->rgblight_ranges.effect_num_leds; i++) {
        //LED_TYPE *ledp = led + i + rgblight_ranges.effect_start_pos;
        if (i < state->rgblight_ranges.effect_num_leds / 2 && state->animation_status.pos) {
            effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, state->config->hue, state->config->sat, state->config->val);
            //sethsv(rgblight_config.hue, rgblight_config.sat, rgblight_config.val, ledp);
        } else if (i >= state->rgblight_ranges.effect_num_leds / 2 && !state->animation_status.pos) {
            effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, state->config->hue, state->config->sat, state->config->val);
            //sethsv(rgblight_config.hue, rgblight_config.sat, rgblight_config.val, ledp);
        } else {
            effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, state->config->hue, 0, state->config->val);
            //sethsv(rgblight_config.hue, rgblight_config.sat, 0, ledp);
        }
    }
    //rgblight_set();
    state->animation_status.pos = (state->animation_status.pos + 1) % 2;
}
#endif

#ifdef RGBLIGHT_EFFECT_TWINKLE
__attribute__((weak)) const uint8_t RGBLED_TWINKLE_INTERVALS[] PROGMEM = {30, 15, 5};

typedef struct PACKED {
    HSV     hsv;
    uint8_t life;
    uint8_t max_life;
} TwinkleState;

static TwinkleState led_twinkle_state[RGBLED_NUM];

void rgblight_effect_twinkle(rgb_linear_state_t *state) {
    const bool random_color = state->animation_status.delta / 3;
    const bool restart      = state->animation_status.pos == 0;
    anim->pos               = 1;

    const uint8_t bottom = breathe_calc(0);
    const uint8_t top    = breathe_calc(127);

    uint8_t frac(uint8_t n, uint8_t d) {
        return (uint16_t)255 * n / d;
    }
    uint8_t scale(uint16_t v, uint8_t scale) {
        return (v * scale) >> 8;
    }

    for (uint8_t i = 0; i < state->rgblight_ranges.effect_num_leds; i++) {
        TwinkleState *t = &(led_twinkle_state[i]);
        HSV *         c = &(t->hsv);

        if (!random_color) {
            c->h = state->config->hue;
            c->s = state->config->sat;
        }

        if (restart) {
            // Restart
            t->life = 0;
            c->v    = 0;
        } else if (t->life) {
            // This LED is already on, either brightening or dimming
            t->life--;
            uint8_t unscaled = frac(breathe_calc(frac(t->life, t->max_life)) - bottom, top - bottom);
            c->v             = scale(state->config->val, unscaled);
        } else if (rand() < scale((uint16_t)RAND_MAX * RGBLIGHT_EFFECT_TWINKLE_PROBABILITY, 127 + state->config->val / 2)) {
            // This LED is off, but was randomly selected to start brightening
            if (random_color) {
                c->h = rand() % 0xFF;
                c->s = (rand() % (state->config->sat / 2)) + (state->config->sat / 2);
            }
            c->v        = 0;
            t->max_life = MAX(20, MIN(RGBLIGHT_EFFECT_TWINKLE_LIFE, state->config->val));
            t->life     = t->max_life;
        } else {
            // This LED is off, and was NOT selected to start brightening
        }

        //LED_TYPE *ledp = led + i + rgblight_ranges.effect_start_pos;
        //sethsv(c->h, c->s, c->v, ledp);
        effects_set_color(i+state->rgblight_ranges.effect_start_pos, c->h, c->s, c->v);
    }

    //rgblight_set();
}
#endif

static void effects_set_color(rgb_linear_state_t *state, uint8_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    uint8_t g_index = index + state->led_start;
    rgb_driver_t *driver = rgb_led_map(g_index);
    if (driver) {
        driver->set_color(driver, g_index, hue, sat, val);
    }
}

static void effects_set_color_all(rgb_linear_state_t *state, uint8_t hue, uint8_t sat, uint8_t val)
{
    for (uint8_t i = 0; i < state->led_num; i++) {
        effects_set_color(state, i, hue, sat, val);
    }
}

static void effects_set_color_rgb(rgb_linear_state_t *state, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t g_index = index + state->led_start;
    rgb_driver_t *driver = rgb_led_map(g_index);
    if (driver) {
        driver->set_color_rgb(driver, g_index, red, green, blue);
    }
}

static void effects_set_color_all_rgb(rgb_linear_state_t *state, uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < state->led_num; i++) {
        effects_set_color_rgb(state, i, red, green, blue);
    }
}

static void effects_set_enable(rgb_linear_state_t *state, uint8_t enable)
{
    state->config->enable = enable;
    eeconfig_update_rgb(state->config, state->config->index);
}

static bool effects_config_valid(rgb_config_t *config) {
    
    if (config->mode < RGBLIGHT_MODE_STATIC_LIGHT) {
        return false;
    } else if (config->mode > RGBLIGHT_MODES) {
        return false;
    }

    if (config->val > RGBLIGHT_LIMIT_VAL) {
        return false;
    }

    return true;
}

static void effects_update_default(rgb_linear_state_t *state)
{
    state->config->enable = 1;
    state->config->mode   = RGBLIGHT_DEFAULT_MODE;
    state->config->hue    = RGBLIGHT_DEFAULT_HUE;
    state->config->sat    = RGBLIGHT_DEFAULT_SAT;
    state->config->val    = RGBLIGHT_DEFAULT_VAL;
    state->config->speed  = RGBLIGHT_DEFAULT_SPD;

    eeconfig_update_rgb(state->config, state->config->index);
}

rgb_effect_t rgb_effect_linear_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    rgb_linear_state_t *state = &effects_state[index];
    state->config = config;
    state->config->type = RGB_EFFECT_LINEAR;
    state->config->data = state;
    state->index = index;
    state->led_start = led_start;
    state->led_num = led_num;
    // status
    state->rgblight_status.base_mode = 0;
    state->rgblight_status.timer_enabled = false;
    // ranges
    state->rgblight_ranges.clipping_start_pos = 0;
    state->rgblight_ranges.clipping_num_leds = led_num;
    state->rgblight_ranges.effect_start_pos = 0;
    state->rgblight_ranges.effect_end_pos = led_num;
    state->rgblight_ranges.effect_num_leds = led_num;

    // animation
    state->animation_status.last_timer = 0;
    state->animation_status.delta = 0;
    state->animation_status.restart = false;
    state->animation_status.pos16 = 0;

    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    eeconfig_read_rgb(state->config, state->config->index);
    rgb_effects_debug("RGB config: index:%d, enable:%d, mode:%d, speed:%d, hue:%d, sat:%d, val:%d\n", 
    index, state->config->enable, state->config->mode, state->config->speed, state->config->hue, state->config->sat, state->config->val);

    if ( !effects_config_valid(state->config)){
        effects_update_default(state);
    }

    rgb_effect_linear_init_mode(state); // initialize current mode

    return state;
}

void rgb_effect_linear_uninit(rgb_effect_t effect)
{}

void rgb_effect_linear_task(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;

    if (!state->config->enable) {
       effects_set_color_all(state, 0, 0, 0);
       return;
    }

    if (state->rgblight_status.timer_enabled) {
    effect_func_t effect_func       = rgblight_effect_dummy;
    uint16_t      interval_time     = 2000; // dummy interval
    uint8_t       delta             = state->config->mode - state->rgblight_status.base_mode;
    state->animation_status.delta   = delta;

    // static light mode, do nothing here
    if (1 == 0) { // dummy
    }
#    ifdef RGBLIGHT_EFFECT_BREATHING
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_BREATHING) {
            // breathing mode
            interval_time = get_interval_time(&RGBLED_BREATHING_INTERVALS[delta], 1, 100);
            effect_func   = rgblight_effect_breathing;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_RAINBOW_MOOD
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_RAINBOW_MOOD) {
            // rainbow mood mode
            interval_time = get_interval_time(&RGBLED_RAINBOW_MOOD_INTERVALS[delta], 5, 100);
            effect_func   = rgblight_effect_rainbow_mood;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_RAINBOW_SWIRL
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_RAINBOW_SWIRL) {
            // rainbow swirl mode
            interval_time = get_interval_time(&RGBLED_RAINBOW_SWIRL_INTERVALS[delta / 2], 1, 100);
            effect_func   = rgblight_effect_rainbow_swirl;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_SNAKE
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_SNAKE) {
            // snake mode
            interval_time = get_interval_time(&RGBLED_SNAKE_INTERVALS[delta / 2], 1, 200);
            effect_func   = rgblight_effect_snake;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_KNIGHT
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_KNIGHT) {
            // knight mode
            interval_time = get_interval_time(&RGBLED_KNIGHT_INTERVALS[delta], 5, 100);
            effect_func   = rgblight_effect_knight;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_CHRISTMAS
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_CHRISTMAS) {
            // christmas mode
            interval_time = RGBLIGHT_EFFECT_CHRISTMAS_INTERVAL;
            effect_func   = (effect_func_t)rgblight_effect_christmas;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_RGB_TEST
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_RGB_TEST) {
            // RGB test mode
            interval_time = pgm_read_word(&RGBLED_RGBTEST_INTERVALS[0]);
            effect_func   = (effect_func_t)rgblight_effect_rgbtest;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_ALTERNATING
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_ALTERNATING) {
            interval_time = 500;
            effect_func   = (effect_func_t)rgblight_effect_alternating;
        }
#    endif
#    ifdef RGBLIGHT_EFFECT_TWINKLE
        else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_TWINKLE) {
            interval_time = get_interval_time(&RGBLED_TWINKLE_INTERVALS[delta % 3], 5, 30);
            effect_func   = (effect_func_t)rgblight_effect_twinkle;
        }
#    endif
        if (state->animation_status.restart) {
            state->animation_status.restart    = false;
            state->animation_status.last_timer = sync_timer_read();
            state->animation_status.pos16      = 0; // restart signal to local each effect
        }
        uint16_t now = sync_timer_read();
        if (timer_expired(now, state->animation_status.last_timer)) {
#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
            static uint16_t report_last_timer = 0;
            static bool     tick_flag         = false;
            uint16_t        oldpos16;
            if (tick_flag) {
                tick_flag = false;
                if (timer_expired(now, report_last_timer)) {
                    report_last_timer += 30000;
                    dprintf("rgblight animation tick report to slave\n");
                    RGBLIGHT_SPLIT_ANIMATION_TICK;
                }
            }
            oldpos16 = animation_status.pos16;
#    endif
            state->animation_status.last_timer += interval_time;
            effect_func(state);
#    if defined(RGBLIGHT_SPLIT) && !defined(RGBLIGHT_SPLIT_NO_ANIMATION_SYNC)
            if (animation_status.pos16 == 0 && oldpos16 != 0) {
                tick_flag = true;
            }
#    endif
        }
    } else {
        // static mode
        state->rgblight_status.base_mode = mode_base_table[state->config->mode];
        uint8_t hue = state->config->hue;
        uint8_t sat = state->config->sat;
        uint8_t val = state->config->val;

        if (state->config->mode == RGBLIGHT_MODE_STATIC_LIGHT) {
            // same static color
            //LED_TYPE tmp_led;
            //sethsv(hue, sat, val, &tmp_led);
            //rgblight_setrgb(tmp_led.r, tmp_led.g, tmp_led.b);
            effects_set_color_all(state, hue, sat, val);
        } else {
            // all LEDs in same color
            if (1 == 0) { // dummy
            }
#ifdef RGBLIGHT_EFFECT_BREATHING
            else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_BREATHING) {
                // breathing mode, ignore the change of val, use in memory value instead
                val = state->config->val;
            }
#endif
#ifdef RGBLIGHT_EFFECT_RAINBOW_MOOD
            else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_RAINBOW_MOOD) {
                // rainbow mood, ignore the change of hue
                hue = state->config->hue;
            }
#endif
#ifdef RGBLIGHT_EFFECT_RAINBOW_SWIRL
            else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_RAINBOW_SWIRL) {
                // rainbow swirl, ignore the change of hue
                hue = state->config->hue;
            }
#endif
#ifdef RGBLIGHT_EFFECT_STATIC_GRADIENT
            else if (state->rgblight_status.base_mode == RGBLIGHT_MODE_STATIC_GRADIENT) {
                // static gradient
                uint8_t delta     = state->config->mode - state->rgblight_status.base_mode;
                bool    direction = (delta % 2) == 0;
                uint8_t range = RGBLED_GRADIENT_RANGES[delta / 2];

                for (uint8_t i = 0; i < state->rgblight_ranges.effect_num_leds; i++) {
                    uint8_t _hue = ((uint16_t)i * (uint16_t)range) / state->rgblight_ranges.effect_num_leds;
                    if (direction) {
                        _hue = hue + _hue;
                    } else {
                        _hue = hue - _hue;
                    }
                    //dprintf("rgblight rainbow set hsv: %d,%d,%d,%u\n", i, _hue, direction, range);
                    //sethsv(_hue, sat, val, (LED_TYPE *)&led[i + rgblight_ranges.effect_start_pos]);
                    effects_set_color(state, i+state->rgblight_ranges.effect_start_pos, _hue, sat, val);
                }
                //rgblight_set();
            }
#endif
        }

        state->config->hue = hue;
        state->config->sat = sat;
        state->config->val = val;
    }
}

bool rgb_effect_linear_enabled(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    return state->config->enable ? true :false;
}

void rgb_effect_linear_toggle(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    state->config->enable = !state->config->enable;
    effects_set_enable(state, state->config->enable);
}

void rgb_effect_linear_init_mode(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    bool write_to_eeprom = false;

    if (state->config->mode < RGBLIGHT_MODE_STATIC_LIGHT) {
        state->config->mode = RGBLIGHT_MODE_STATIC_LIGHT;
        write_to_eeprom = true;
    } else if (state->config->mode > RGBLIGHT_MODES) {
        state->config->mode = RGBLIGHT_MODES;
        write_to_eeprom = true;
    } 

    if (write_to_eeprom) {
        eeconfig_update_rgb(state->config, state->config->index);
    }

    if (is_static_effect(state->config->mode)) {
        rgblight_timer_disable(state);
    } else {
        rgblight_timer_enable(state);
    }

    state->animation_status.restart = true;
}
