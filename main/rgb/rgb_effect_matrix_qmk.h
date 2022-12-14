/**
 * @file rgb_effect_matrix_qmk.h
 * @brief rgb matrix from qmk
 */

#pragma once
#include "rgb_driver.h"
#include "qmk/lib8tion/lib8tion.h"

// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
#    define ENABLE_RGB_MATRIX_ALPHAS_MODS
#    define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#    define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#    define ENABLE_RGB_MATRIX_BREATHING
#    define ENABLE_RGB_MATRIX_BAND_SAT
#    define ENABLE_RGB_MATRIX_BAND_VAL
#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#    define ENABLE_RGB_MATRIX_CYCLE_ALL
#    define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#    define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#    define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#    define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#    define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#    define ENABLE_RGB_MATRIX_DUAL_BEACON
#    define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#    define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#    define ENABLE_RGB_MATRIX_RAINDROPS
#    define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#    define ENABLE_RGB_MATRIX_HUE_BREATHING
#    define ENABLE_RGB_MATRIX_HUE_PENDULUM
#    define ENABLE_RGB_MATRIX_HUE_WAVE
#    define ENABLE_RGB_MATRIX_PIXEL_RAIN
#    define ENABLE_RGB_MATRIX_PIXEL_FLOW
#    define ENABLE_RGB_MATRIX_PIXEL_FRACTAL

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
#    define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#    define ENABLE_RGB_MATRIX_DIGITAL_RAIN

#define RGB_MATRIX_KEYPRESSES
// enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#    define ENABLE_RGB_MATRIX_SPLASH
#    define ENABLE_RGB_MATRIX_MULTISPLASH
#    define ENABLE_RGB_MATRIX_SOLID_SPLASH
#    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH


#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
#    define RGB_MATRIX_KEYREACTIVE_ENABLED
#endif

// Last led hit
#ifndef LED_HITS_TO_REMEMBER
#    define LED_HITS_TO_REMEMBER 8
#endif  // LED_HITS_TO_REMEMBER

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
typedef struct {
    uint8_t  count;
    uint8_t  x[LED_HITS_TO_REMEMBER];
    uint8_t  y[LED_HITS_TO_REMEMBER];
    uint8_t  index[LED_HITS_TO_REMEMBER];
    uint16_t tick[LED_HITS_TO_REMEMBER];
} last_hit_t;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

typedef enum rgb_task_states { STARTING, RENDERING, FLUSHING, SYNCING } rgb_task_states;

typedef uint8_t led_flags_t;

typedef struct {
    uint8_t     iter;
    led_flags_t flags;
    bool        init;
} effect_params_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} led_point_t;

#define HAS_FLAGS(bits, flags) ((bits & flags) == flags)
#define HAS_ANY_FLAGS(bits, flags) ((bits & flags) != 0x00)

#define LED_FLAG_ALL 0xFF
#define LED_FLAG_NONE 0x00
#define LED_FLAG_MODIFIER 0x01
#define LED_FLAG_UNDERGLOW 0x02
#define LED_FLAG_KEYLIGHT 0x04
#define LED_FLAG_INDICATOR 0x08

#define NO_LED 255
#define DRIVER_LED_TOTAL RGB_MATRIX_LED_NUM

typedef struct {
    uint8_t     matrix_co[MATRIX_ROWS][MATRIX_COLS];
    led_point_t point[DRIVER_LED_TOTAL];
    uint8_t     flags[DRIVER_LED_TOTAL];
} led_config_t;

#ifndef RGB_MATRIX_LED_FLUSH_LIMIT
#    define RGB_MATRIX_LED_FLUSH_LIMIT 16
#endif

#ifndef RGB_MATRIX_LED_PROCESS_LIMIT
#    define RGB_MATRIX_LED_PROCESS_LIMIT (DRIVER_LED_TOTAL + 4) / 5
#endif

#if defined(RGB_MATRIX_LED_PROCESS_LIMIT) && RGB_MATRIX_LED_PROCESS_LIMIT > 0 && RGB_MATRIX_LED_PROCESS_LIMIT < DRIVER_LED_TOTAL
#    if defined(RGB_MATRIX_SPLIT)
#        define RGB_MATRIX_USE_LIMITS(min, max)                                                   \
            uint8_t min = RGB_MATRIX_LED_PROCESS_LIMIT * params->iter;                            \
            uint8_t max = min + RGB_MATRIX_LED_PROCESS_LIMIT;                                     \
            if (max > DRIVER_LED_TOTAL) max = DRIVER_LED_TOTAL;                                   \
            uint8_t k_rgb_matrix_split[2] = RGB_MATRIX_SPLIT;                                     \
            if (is_keyboard_left() && (max > k_rgb_matrix_split[0])) max = k_rgb_matrix_split[0]; \
            if (!(is_keyboard_left()) && (min < k_rgb_matrix_split[0])) min = k_rgb_matrix_split[0];
#    else
#        define RGB_MATRIX_USE_LIMITS(min, max)                        \
            uint8_t min = RGB_MATRIX_LED_PROCESS_LIMIT * state->rgb_effect_params.iter; \
            uint8_t max = min + RGB_MATRIX_LED_PROCESS_LIMIT;          \
            if (max > DRIVER_LED_TOTAL) max = DRIVER_LED_TOTAL;
#    endif
#else
#    if defined(RGB_MATRIX_SPLIT)
#        define RGB_MATRIX_USE_LIMITS(min, max)                                                   \
            uint8_t       min                   = 0;                                              \
            uint8_t       max                   = DRIVER_LED_TOTAL;                               \
            const uint8_t k_rgb_matrix_split[2] = RGB_MATRIX_SPLIT;                               \
            if (is_keyboard_left() && (max > k_rgb_matrix_split[0])) max = k_rgb_matrix_split[0]; \
            if (!(is_keyboard_left()) && (min < k_rgb_matrix_split[0])) min = k_rgb_matrix_split[0];
#    else
#        define RGB_MATRIX_USE_LIMITS(min, max) \
            uint8_t min = 0;                    \
            uint8_t max = DRIVER_LED_TOTAL;
#    endif
#endif

#define RGB_MATRIX_INDICATOR_SET_COLOR(i, r, g, b) \
    if (i >= led_min && i <= led_max) {            \
        rgb_matrix_set_color(i, r, g, b);          \
    }

#define RGB_MATRIX_TEST_LED_FLAGS() \
    if (!HAS_ANY_FLAGS(g_led_config.flags[i], state->rgb_effect_params.flags)) continue

enum rgb_matrix_effects {
    RGB_MATRIX_NONE = 0,

// --------------------------------------
// -----Begin rgb effect enum macros-----
#define RGB_MATRIX_EFFECT(name, ...) RGB_MATRIX_##name,
#include "qmk/rgb_matrix/rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT

#if defined(RGB_MATRIX_CUSTOM_KB) || defined(RGB_MATRIX_CUSTOM_USER)
#    define RGB_MATRIX_EFFECT(name, ...) RGB_MATRIX_CUSTOM_##name,
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
#    undef RGB_MATRIX_EFFECT
#endif
    // --------------------------------------
    // -----End rgb effect enum macros-------

    RGB_MATRIX_EFFECT_MAX
};

#define RM_EFFECT_MAX RGB_MATRIX_EFFECT_MAX
//extern uint32_t     g_rgb_timer;
extern led_config_t g_led_config;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
//extern last_hit_t g_last_hit_tracker;
#endif
#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
//extern uint8_t g_rgb_frame_buffer[MATRIX_ROWS][MATRIX_COLS];
#endif

static inline bool rgb_matrix_check_finished_leds(uint8_t led_idx) {
#if defined(RGB_MATRIX_SPLIT)
    if (is_keyboard_left()) {
        uint8_t k_rgb_matrix_split[2] = RGB_MATRIX_SPLIT;
        return led_idx < k_rgb_matrix_split[0];
    } else
        return led_idx < DRIVER_LED_TOTAL;
#else
    return led_idx < DRIVER_LED_TOTAL;
#endif
}

rgb_effect_t rgb_effect_matrix_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num);
void rgb_effect_matrix_uninit(rgb_effect_t effect);
void rgb_effect_matrix_task(rgb_effect_t effect);

bool rgb_effect_matrix_enabled(rgb_effect_t effect);
void rgb_effect_matrix_toggle(rgb_effect_t effect);

void rgb_effect_matrix_init_mode(rgb_effect_t effect);

void rgb_effect_matrix_set_rgb(rgb_effect_t effect, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
