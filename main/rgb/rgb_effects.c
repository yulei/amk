/**
 * rgb_effects.c
 * rgb effects implementation
 */

#include <stdlib.h>
#include <stdbool.h>

#include "rgb_effects.h"
#include "rgb_led.h"
#include "timer.h"
#include "eeprom_manager.h"
#include "amk_printf.h"

#ifndef RGB_EFFECTS_NUM
    #define RGB_EFFECTS_NUM 1
#endif

#ifndef RGB_EFFECTS_DEBUG
#define RGB_EFFECTS_DEBUG 1
#endif

#if RGB_EFFECTS_DEBUG
#define rgb_effects_debug  amk_printf
#else
#define rgb_effects_debug(...)
#endif

#define DELAY_MIN   0
#define DELAY_DEFAULT 1500

#define SPEED_MIN 1
#define SPEED_MAX 15
#define SPEED_DEFAULT 7

#define HUE_MIN 0
#define HUE_MAX 255
#define HUE_DEFAULT HUE_MIN
#define HUE_STEP EFFECTS_HUE_STEP

#define SAT_MIN 0
#define SAT_MAX 255
#define SAT_DEFAULT SAT_MAX
#define SAT_STEP EFFECTS_SAT_STEP

#define VAL_MIN 0
#define VAL_MAX 255
#define VAL_DEFAULT VAL_MAX
#define VAL_STEP EFFECTS_VAL_STEP

#define RAINBOW_STEP_DEFAULT    32
#define BREATH_STEP_DEFAULT     32
#define CIRCLE_STEP_DEFAULT     2

enum rgb_effects_type {
    RGB_EFFECT_STATIC,
    RGB_EFFECT_BLINK,
    RGB_EFFECT_GRADIENT,
    RGB_EFFECT_RAINBOW,
    RGB_EFFECT_RANDOM,
    RGB_EFFECT_BREATH,
    RGB_EFFECT_WIPE,
    RGB_EFFECT_SCAN,
    RGB_EFFECT_CIRCLE,
    RGB_EFFECT_MAX
};

#define RGB_EFFECT_DEFAULT RGB_EFFECT_CIRCLE

typedef struct s_rgb_effects_state rgb_effects_state_t;
typedef void (*RGB_EFFECT_FUN)(rgb_effects_state_t*);

struct s_rgb_effects_state {
    rgb_effects_config_t    config;
    uint8_t                 index;
    uint8_t                 led_start;
    uint8_t                 led_count;
    uint32_t                last_ticks;
    uint32_t                counter;
    uint32_t                rainbow_step;
    uint32_t                breath_step;
    uint32_t                circle_step;
    bool                    wipe_on;
    RGB_EFFECT_FUN          effects[RGB_EFFECT_MAX];
};

static rgb_effects_state_t effects_state[RGB_EFFECTS_NUM];

// utilities
#define RANDOM_DISTANCE 17
static uint8_t get_random_hue(uint8_t hue) { return (rand() % HUE_MAX) + RANDOM_DISTANCE; }

static bool effects_config_valid(rgb_effects_config_t *config)
{
    if (config->mode >= RGB_EFFECT_MAX) {
        return false;
    }

    if (config->speed > SPEED_MAX || config->speed < SPEED_MIN) {
        return false;
    }

    if (config->sat == SAT_MIN) {
        return false;
    }

    if (config->val == VAL_MIN) {
        return false;
    }
    return true;
}

static void effects_set_color(rgb_effects_state_t *state, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t g_index = index + state->led_start;
    rgb_driver_t *driver = rgb_led_map(g_index);
    if (driver) {
        driver->set_color(driver, g_index, red, green, blue);
    }
}

static void effects_set_color_all(rgb_effects_state_t *state, uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < state->led_count; i++) {
        effects_set_color(state, i, red, green, blue);
    }
}

static void effects_mode_init(rgb_effects_state_t *state)
{
    switch(state->config.mode) {
        case RGB_EFFECT_STATIC:
            break;
        case RGB_EFFECT_BLINK:
            break;
        case RGB_EFFECT_GRADIENT:
            break;
        case RGB_EFFECT_RAINBOW:
            break;
        case RGB_EFFECT_RANDOM:
            break;
        case RGB_EFFECT_BREATH:
            break;
        case RGB_EFFECT_WIPE:
            state->wipe_on = true;
            state->counter = 0;
            effects_set_color_all(state, 0, 0, 0);
            break;
        case RGB_EFFECT_SCAN:
            state->counter = 0;
            effects_set_color_all(state, 0, 0, 0);
            break;
        case RGB_EFFECT_CIRCLE:
            break; 
        default:
            break;
    }
}

static uint32_t effects_delay(rgb_effects_state_t *state)
{
    switch(state->config.mode) {
        case RGB_EFFECT_STATIC:
            return DELAY_MIN;
        case RGB_EFFECT_BLINK:
            break;
        case RGB_EFFECT_GRADIENT:
            return DELAY_MIN;
        case RGB_EFFECT_RAINBOW:
            break;
        case RGB_EFFECT_RANDOM:
            break;
        case RGB_EFFECT_BREATH:
            break;
        case RGB_EFFECT_CIRCLE:
            break;
        default:
            break;
    }

    return DELAY_DEFAULT;
}

static bool effects_need_update(rgb_effects_state_t *state)
{
    //return timer_elapsed32(state->last_ticks)*state->config.speed >= effects_delay(state);
    return timer_elapsed32(state->last_ticks)*SPEED_DEFAULT>= effects_delay(state);
}

static void effects_update_timer(rgb_effects_state_t *state)
{
    state->last_ticks = timer_read32();
}

//effects
static void effects_mode_static(rgb_effects_state_t *state)
{
    effects_set_color_all(state, state->config.hue, state->config.sat, state->config.val);
}

static void effects_mode_blink(rgb_effects_state_t *state)
{
    if(state->counter % 2 == 1) {
        // on
        effects_set_color_all(state, state->config.hue, state->config.sat, state->config.val);
    }
    else {
        // off
        effects_set_color_all(state, 0, 0, 0);
    }
    state->counter++;
}

static void effects_mode_random(rgb_effects_state_t *state)
{
    uint8_t hue = state->config.hue;
    for (int i = 0; i < state->led_count; i++) {
        hue = get_random_hue(hue);
        effects_set_color_all(state, hue, state->config.sat, state->config.val);
    }
}

static void effects_mode_gradient(rgb_effects_state_t *state)
{
    uint8_t step = HUE_MAX / state->led_count;

    for (int i = 0; i < state->led_count; i++) {
        effects_set_color(state, i, state->config.hue + i*step, state->config.sat, state->config.val);
    }
    //effects_state.config.hue += step;
}

static void effects_mode_rainbow(rgb_effects_state_t *state)
{
    uint8_t step = HUE_MAX / state->led_count;
    for (int i = 0; i < state->led_count; i++) {
        effects_set_color(state, i, state->config.hue + i*step, state->config.sat, state->config.val);
    }
    state->config.hue += state->rainbow_step;
}

static void effects_mode_breath(rgb_effects_state_t *state)
{
    uint8_t breath = VAL_MAX / state->breath_step;

    effects_set_color_all(state, state->config.hue, state->config.sat, state->config.val);
    state->config.val += breath;
}

static void effects_mode_wipe(rgb_effects_state_t *state)
{
    if (state->wipe_on) {
        effects_set_color(state, state->counter, state->config.hue, state->config.sat, state->config.val);
    } else {
        effects_set_color(state, state->counter, 0, 0, 0);
    }
    state->counter++;
    if (state->counter == state->led_count) {
        state->counter = 0;
        state->wipe_on = !state->wipe_on;
        state->config.hue += HUE_MAX/state->led_count;
    }
}

static void effects_mode_scan(rgb_effects_state_t *state)
{
    uint8_t prev = (state->counter == 0) ? state->led_count - 1 : state->counter - 1;
    effects_set_color(state, prev, 0, 0, 0);
    effects_set_color(state, state->counter, state->config.hue, state->config.sat, state->config.val);
    state->counter = (state->counter + 1) % state->led_count;
    if (state->counter == 0) {
        state->config.hue += HUE_MAX/state->led_count;
    }
}

static void effects_mode_circle(rgb_effects_state_t *state)
{
    effects_set_color_all(state, state->config.hue, state->config.sat, state->config.val);

    state->config.hue += state->circle_step;
}

static void effects_set_hue(rgb_effects_state_t *state, uint8_t hue)
{
    state->config.hue = hue;
    eeconfig_update_rgb(&state->config, state->index);
}

static void effects_set_sat(rgb_effects_state_t *state, uint8_t sat)
{
    state->config.sat = sat;
    eeconfig_update_rgb(&state->config, state->index);
}

static void effects_set_val(rgb_effects_state_t *state, uint8_t val)
{
    state->config.val = val;
    eeconfig_update_rgb(&state->config, state->index);
}

static void effects_set_speed(rgb_effects_state_t *state, uint8_t speed)
{
    state->config.speed = !speed ? 1 : speed;
    eeconfig_update_rgb(&state->config, state->index);
}

static void effects_set_mode(rgb_effects_state_t *state, uint8_t mode)
{
    state->config.mode = mode;
    effects_mode_init(state);
    eeconfig_update_rgb(&state->config, state->index);
}

static void effects_set_enable(rgb_effects_state_t *state, uint8_t enable)
{
    state->config.enable = enable;
    eeconfig_update_rgb(&state->config, state->index);
    //if (state->config.enable)
    //    state->driver->init();
}

static void effects_update_default(rgb_effects_state_t *state)
{
    state->config.enable = 1;
    state->config.mode = RGB_EFFECT_DEFAULT;
    state->config.speed = SPEED_DEFAULT;
    state->config.hue = HUE_DEFAULT;
    state->config.sat = SAT_DEFAULT;
    state->config.val = VAL_DEFAULT;
    eeconfig_update_rgb(&state->config, state->index);
}

static void effects_state_init(rgb_effects_state_t * state)
{
    state->counter       = 0;
    state->wipe_on       = true;
    state->rainbow_step  = RAINBOW_STEP_DEFAULT;
    state->breath_step   = BREATH_STEP_DEFAULT;
    state->circle_step   = CIRCLE_STEP_DEFAULT;
    state->last_ticks    = timer_read32();
    srand(state->last_ticks);
    state->effects[RGB_EFFECT_STATIC]    = effects_mode_static;
    state->effects[RGB_EFFECT_BLINK]     = effects_mode_blink;
    state->effects[RGB_EFFECT_GRADIENT]  = effects_mode_gradient;
    state->effects[RGB_EFFECT_RAINBOW]   = effects_mode_rainbow;
    state->effects[RGB_EFFECT_RANDOM]    = effects_mode_random;
    state->effects[RGB_EFFECT_BREATH]    = effects_mode_breath;
    state->effects[RGB_EFFECT_WIPE]      = effects_mode_wipe;
    state->effects[RGB_EFFECT_SCAN]      = effects_mode_scan;
    state->effects[RGB_EFFECT_CIRCLE]    = effects_mode_circle;
    effects_mode_init(state);
}

// interface
rgb_effects_t rgb_effects_init(rgb_driver_t* driver, uint8_t index, uint8_t led_start, uint8_t led_count)
{
    rgb_effects_state_t *state = &effects_state[index];
    state->index = index;
    state->led_start = led_start;
    state->led_count = led_count;
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    eeconfig_read_rgb(&state->config, index);
    if ( !effects_config_valid(&state->config)){
        effects_update_default(state);
    }

    //state->driver = driver;
    //if (state->config.enable)
    //    state->driver->init();
    
    effects_state_init(state);

    return state;
}

void rgb_effects_uninit(rgb_effects_t effects)
{}

void rgb_effects_inc_hue(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_hue(state, state->config.hue + HUE_STEP);
}

void rgb_effects_dec_hue(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_hue(state, state->config.hue - HUE_STEP);
}

void rgb_effects_inc_sat(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_sat(state, state->config.sat + SAT_STEP);
}

void rgb_effects_dec_sat(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_sat(state, state->config.sat - SAT_STEP);
}

void rgb_effects_inc_val(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_val(state, state->config.val + VAL_STEP);
}

void rgb_effects_dec_val(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_val(state, state->config.val - VAL_STEP);
}

void rgb_effects_inc_speed(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_speed(state, ++state->config.speed);
}

void rgb_effects_dec_speed(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    effects_set_speed(state, --state->config.speed);
}

void rgb_effects_inc_mode(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    state->config.mode++;
    if (state->config.mode >= RGB_EFFECT_MAX) {
        state->config.mode = RGB_EFFECT_STATIC;
    }
    effects_set_mode(state, state->config.mode);
}

void rgb_effects_dec_mode(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    if (state->config.mode == 0) {
        state->config.mode = RGB_EFFECT_MAX - 1;
    } else {
        state->config.mode--;
    }

    effects_set_mode(state, state->config.mode);
}

void rgb_effects_toggle(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    state->config.enable = !state->config.enable;
    effects_set_enable(state, state->config.enable);

/*
    if (state->config.enable) {
        state->driver->init();
        rgb_effects_debug("effects: driver init\n");
    } else {
        #ifdef RGBLIGHT_EN_PIN
        state->driver->uninit();
        rgb_effects_debug("effects: driver uninit\n");
        #endif
    }
*/
}

void rgb_effects_task(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    if (state->config.enable) {
        if (effects_need_update(state) ) {
            state->effects[state->config.mode](state);
            effects_update_timer(state);
            //amk_printf("effects last ticks: %d\n", effects_state.last_ticks);
        }
        //state->driver->flush();
    } else {
        #ifndef RGBLIGHT_EN_PIN
        effects_set_color_all(state, 0, 0, 0);
        //state->driver->flush();
        #endif
    }
}

bool rgb_effects_enabled(rgb_effects_t effects)
{
    rgb_effects_state_t *state = (rgb_effects_state_t*)effects;
    return state->config.enable ? true :false;
}