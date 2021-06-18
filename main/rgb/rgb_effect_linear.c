/**
 * @file rgb_effect_linear.c
 * @brief linear rgb effects implementation
 */

#include <stdlib.h>
#include <stdbool.h>

#include "rgb_effect_linear.h"
#include "rgb_led.h"
#include "timer.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#ifndef RGB_EFFECT_LINEAR_NUM
    #define RGB_EFFECT_LINEAR_NUM RGB_SEGMENT_NUM
#endif

#ifndef RGB_EFFECTS_DEBUG
#define RGB_EFFECTS_DEBUG 0
#endif

#if RGB_EFFECTS_DEBUG
#define rgb_effects_debug  amk_printf
#else
#define rgb_effects_debug(...)
#endif

#define DELAY_MIN   0
#define DELAY_DEFAULT 1500

#define RAINBOW_STEP_DEFAULT    32
#define BREATH_STEP_DEFAULT     32
#define CIRCLE_STEP_DEFAULT     2

typedef struct s_rgb_linear_state rgb_linear_state_t;
typedef void (*RGB_EFFECT_FUN)(rgb_linear_state_t*);

struct s_rgb_linear_state {
    rgb_config_t            *config;
    RGB_EFFECT_FUN          effects[RL_EFFECT_MAX];
    uint32_t                last_ticks;
    uint32_t                counter;
    uint32_t                rainbow_step;
    uint32_t                breath_step;
    uint32_t                circle_step;
    bool                    wipe_on;
    uint8_t                 index;
    uint8_t                 led_start;
    uint8_t                 led_num;
};

static rgb_linear_state_t effects_state[RGB_EFFECT_LINEAR_NUM];


static bool effects_config_valid(rgb_config_t *config)
{
    if (config->mode >= RL_EFFECT_MAX) {
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

static void effects_mode_init(rgb_linear_state_t *state)
{
    switch(state->config->mode) {
        case RL_EFFECT_STATIC:
            break;
        case RL_EFFECT_BLINK:
            break;
        case RL_EFFECT_GRADIENT:
            break;
        case RL_EFFECT_RAINBOW:
            break;
        case RL_EFFECT_RANDOM:
            break;
        case RL_EFFECT_BREATH:
            break;
        case RL_EFFECT_WIPE:
            state->wipe_on = true;
            state->counter = 0;
            effects_set_color_all(state, 0, 0, 0);
            break;
        case RL_EFFECT_SCAN:
            state->counter = 0;
            effects_set_color_all(state, 0, 0, 0);
            break;
        case RL_EFFECT_CIRCLE:
            break; 
        default:
            break;
    }
}

static uint32_t effects_delay(rgb_linear_state_t *state)
{
    switch(state->config->mode) {
        case RL_EFFECT_STATIC:
            return DELAY_MIN;
        case RL_EFFECT_BLINK:
            break;
        case RL_EFFECT_GRADIENT:
            return DELAY_MIN;
        case RL_EFFECT_RAINBOW:
            break;
        case RL_EFFECT_RANDOM:
            break;
        case RL_EFFECT_BREATH:
            break;
        case RL_EFFECT_CIRCLE:
            break;
        default:
            break;
    }

    return DELAY_DEFAULT;
}

static bool effects_need_update(rgb_linear_state_t *state)
{
    return timer_elapsed32(state->last_ticks)*state->config->speed >= effects_delay(state);
    //return timer_elapsed32(state->last_ticks)*SPEED_DEFAULT>= effects_delay(state);
}

static void effects_update_timer(rgb_linear_state_t *state)
{
    state->last_ticks = timer_read32();
}

//effects
static void effects_mode_static(rgb_linear_state_t *state)
{
    effects_set_color_all(state, state->config->hue, state->config->sat, state->config->val);
}

static void effects_mode_blink(rgb_linear_state_t *state)
{
    if(state->counter % 2 == 1) {
        // on
        effects_set_color_all(state, state->config->hue, state->config->sat, state->config->val);
    }
    else {
        // off
        effects_set_color_all(state, 0, 0, 0);
    }
    state->counter++;
}

static void effects_mode_random(rgb_linear_state_t *state)
{
    uint8_t hue = state->config->hue;
    for (int i = 0; i < state->led_num; i++) {
        hue = pick_hue();
        effects_set_color_all(state, hue, state->config->sat, state->config->val);
    }
}

static void effects_mode_gradient(rgb_linear_state_t *state)
{
    uint8_t step = HUE_MAX / state->led_num;

    for (int i = 0; i < state->led_num; i++) {
        effects_set_color(state, i, state->config->hue + i*step, state->config->sat, state->config->val);
    }
    //effects_state.config->hue += step;
}

static void effects_mode_rainbow(rgb_linear_state_t *state)
{
    uint8_t step = HUE_MAX / state->led_num;
    for (int i = 0; i < state->led_num; i++) {
        effects_set_color(state, i, state->config->hue + i*step, state->config->sat, state->config->val);
    }
    state->config->hue += state->rainbow_step;
}

static void effects_mode_breath(rgb_linear_state_t *state)
{
    uint8_t breath = VAL_MAX / state->breath_step;

    effects_set_color_all(state, state->config->hue, state->config->sat, state->config->val);
    state->config->val += breath;
}

static void effects_mode_wipe(rgb_linear_state_t *state)
{
    if (state->wipe_on) {
        effects_set_color(state, state->counter, state->config->hue, state->config->sat, state->config->val);
    } else {
        effects_set_color(state, state->counter, 0, 0, 0);
    }
    state->counter++;
    if (state->counter == state->led_num) {
        state->counter = 0;
        state->wipe_on = !state->wipe_on;
        state->config->hue += HUE_MAX/state->led_num;
    }
}

static void effects_mode_scan(rgb_linear_state_t *state)
{
    uint8_t prev = (state->counter == 0) ? state->led_num - 1 : state->counter - 1;
    effects_set_color(state, prev, 0, 0, 0);
    effects_set_color(state, state->counter, state->config->hue, state->config->sat, state->config->val);
    state->counter = (state->counter + 1) % state->led_num;
    if (state->counter == 0) {
        state->config->hue += HUE_MAX/state->led_num;
    }
}

static void effects_mode_circle(rgb_linear_state_t *state)
{
    effects_set_color_all(state, state->config->hue, state->config->sat, state->config->val);

    state->config->hue += state->circle_step;
}

static void effects_set_enable(rgb_linear_state_t *state, uint8_t enable)
{
    state->config->enable = enable;
    eeconfig_update_rgb(state->config, state->config->index);
}

static void effects_update_default(rgb_linear_state_t *state)
{
    state->config->enable   = ENABLE_DEFAULT;
    state->config->mode     = MODE_DEFAULT;
    state->config->speed    = SPEED_DEFAULT;
    state->config->hue      = HUE_DEFAULT;
    state->config->sat      = SAT_DEFAULT;
    state->config->val      = VAL_DEFAULT;
    eeconfig_update_rgb(state->config, state->config->index);
}

static void effects_state_init(rgb_linear_state_t *state)
{
    state->counter       = 0;
    state->wipe_on       = true;
    state->rainbow_step  = RAINBOW_STEP_DEFAULT;
    state->breath_step   = BREATH_STEP_DEFAULT;
    state->circle_step   = CIRCLE_STEP_DEFAULT;
    state->last_ticks    = timer_read32();
    srand(state->last_ticks);
    state->effects[RL_EFFECT_STATIC]    = effects_mode_static;
    state->effects[RL_EFFECT_BLINK]     = effects_mode_blink;
    state->effects[RL_EFFECT_GRADIENT]  = effects_mode_gradient;
    state->effects[RL_EFFECT_RAINBOW]   = effects_mode_rainbow;
    state->effects[RL_EFFECT_RANDOM]    = effects_mode_random;
    state->effects[RL_EFFECT_BREATH]    = effects_mode_breath;
    state->effects[RL_EFFECT_WIPE]      = effects_mode_wipe;
    state->effects[RL_EFFECT_SCAN]      = effects_mode_scan;
    state->effects[RL_EFFECT_CIRCLE]    = effects_mode_circle;
    effects_mode_init(state);
}

// interface
rgb_effect_t rgb_effect_linear_init(rgb_config_t *config, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    rgb_linear_state_t *state = &effects_state[index];
    state->config = config;
    state->config->type = RGB_EFFECT_LINEAR;
    state->config->data = state;
    state->index = index;
    state->led_start = led_start;
    state->led_num = led_num;
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    eeconfig_read_rgb(state->config, state->config->index);
    rgb_effects_debug("RGB config: index:%d, enable:%d, mode:%d, speed:%d, hue:%d, sat:%d, val:%d\n", 
    index, state->config->enable, state->config->mode, state->config->speed, state->config->hue, state->config->sat, state->config->val);

    if ( !effects_config_valid(state->config)){
        effects_update_default(state);
    }

    effects_state_init(state);

    return state;
}

void rgb_effect_linear_uninit(rgb_effect_t effect)
{}

void rgb_effect_linear_init_mode(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    effects_mode_init(state);
}

void rgb_effect_linear_toggle(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    state->config->enable = !state->config->enable;
    effects_set_enable(state, state->config->enable);
}

void rgb_effect_linear_task(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    if (state->config->enable) {
        if (effects_need_update(state) ) {
            state->effects[state->config->mode](state);
            effects_update_timer(state);
        }
    } else {
        effects_set_color_all(state, 0, 0, 0);
    }
}

bool rgb_effect_linear_enabled(rgb_effect_t effect)
{
    rgb_linear_state_t *state = (rgb_linear_state_t*)effect;
    return state->config->enable ? true :false;
}