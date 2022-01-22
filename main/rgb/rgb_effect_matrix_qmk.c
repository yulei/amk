/**
 * @file rgb_effect_matrix_qmk.c
 */

#include <string.h>
#include <stdlib.h>
#include "keyboard.h"
#include "rgb_effect_matrix_qmk.h"
#include "rgb_led.h"
#include "timer.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#ifndef RGB_EFFECTS_DEBUG
#define RGB_EFFECTS_DEBUG 1
#endif

#if RGB_EFFECTS_DEBUG
#define rgb_effects_debug  amk_printf
#else
#define rgb_effects_debug(...)
#endif

typedef struct s_rgb_matrix_state rgb_matrix_state_t;
struct s_rgb_matrix_state {
    rgb_config_t        *config;
    uint8_t             flags;
    uint8_t             index;
    uint8_t             led_start;
    uint8_t             led_num;
    uint32_t            g_rgb_timer;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    last_hit_t          g_last_hit_tracker;
#endif
#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
    uint8_t             g_rgb_frame_buffer[MATRIX_ROWS][MATRIX_COLS];
#endif
    bool                suspend_state;
    uint8_t             rgb_last_enable;
    uint8_t             rgb_last_effect;
    effect_params_t     rgb_effect_params;
    rgb_task_states     rgb_task_state;
#if RGB_DISABLE_TIMEOUT > 0
    uint32_t            rgb_anykey_timer;
#endif  // RGB_DISABLE_TIMEOUT > 0
    uint32_t            rgb_timer_buffer;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    last_hit_t          last_hit_buffer;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED
};

#define sync_timer_init()
#define sync_timer_clear()
#define sync_timer_update(t)
#define sync_timer_read() timer_read()
#define sync_timer_read32() timer_read32()
#define sync_timer_elapsed(t) timer_elapsed(t)
#define sync_timer_elapsed32(t) timer_elapsed32(t)

#ifndef RGB_MATRIX_CENTER
const led_point_t k_rgb_matrix_center = {112, 32};
#else
const led_point_t k_rgb_matrix_center = RGB_MATRIX_CENTER;
#endif

static void effect_set_color(rgb_matrix_state_t *state, uint8_t index, uint8_t hue, uint8_t sat, uint8_t val)
{
    uint8_t g_index = index + state->led_start;
    rgb_driver_t *driver = rgb_led_map(g_index);
    if (driver) {
        driver->set_color(driver, g_index, hue, sat, val);
    }
}

static void effect_set_color_all(rgb_matrix_state_t *state, uint8_t hue, uint8_t sat, uint8_t val)
{
    for (uint8_t i = 0; i < state->led_num; i++) {
        effect_set_color(state, i, hue, sat, val);
    }
}

static void effect_set_color_rgb(rgb_matrix_state_t *state, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t g_index = index + state->led_start;
    rgb_driver_t *driver = rgb_led_map(g_index);
    if (driver) {
        driver->set_color_rgb(driver, g_index, red, green, blue);
        //driver->set_color(driver, g_index, 255, 255, 255);
    }
}

static void effect_set_color_all_rgb(rgb_matrix_state_t *state, uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < state->led_num; i++) {
        effect_set_color_rgb(state, i, red, green, blue);
    }
}

__attribute__((weak)) uint8_t rgb_matrix_map_row_column_to_led_kb(uint8_t row, uint8_t column, uint8_t *led_i) { return 0; }

uint8_t rgb_matrix_map_row_column_to_led(uint8_t row, uint8_t column, uint8_t *led_i) {
    uint8_t led_count = rgb_matrix_map_row_column_to_led_kb(row, column, led_i);
    uint8_t led_index = g_led_config.matrix_co[row][column];
    if (led_index != NO_LED) {
        led_i[led_count] = led_index;
        led_count++;
    }
    return led_count;
}
// Generic effect runners
#include "qmk/runners/rgb_matrix_runners.inc"

// ------------------------------------------
// -----Begin rgb effect includes macros-----
#define RGB_MATRIX_EFFECT(name)
#define RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "qmk/rgb_matrix_effects.inc"
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif

#undef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#undef RGB_MATRIX_EFFECT
// -----End rgb effect includes macros-------
// ------------------------------------------

#if defined(RGB_DISABLE_AFTER_TIMEOUT) && !defined(RGB_DISABLE_TIMEOUT)
#    define RGB_DISABLE_TIMEOUT (RGB_DISABLE_AFTER_TIMEOUT * 1200UL)
#endif

#ifndef RGB_DISABLE_TIMEOUT
#    define RGB_DISABLE_TIMEOUT 0
#endif

#if !defined(RGB_MATRIX_MAXIMUM_BRIGHTNESS) || RGB_MATRIX_MAXIMUM_BRIGHTNESS > UINT8_MAX
#    undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS UINT8_MAX
#endif

#if !defined(RGB_MATRIX_HUE_STEP)
#    define RGB_MATRIX_HUE_STEP 8
#endif

#if !defined(RGB_MATRIX_SAT_STEP)
#    define RGB_MATRIX_SAT_STEP 16
#endif

#if !defined(RGB_MATRIX_VAL_STEP)
#    define RGB_MATRIX_VAL_STEP 16
#endif

#if !defined(RGB_MATRIX_SPD_STEP)
#    define RGB_MATRIX_SPD_STEP 16
#endif

#if !defined(RGB_MATRIX_STARTUP_MODE)
#    ifdef ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#        define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_CYCLE_LEFT_RIGHT
#    else
// fallback to solid colors if RGB_MATRIX_CYCLE_LEFT_RIGHT is disabled in userspace
#        define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_SOLID_COLOR
#    endif
#endif

#if !defined(RGB_MATRIX_STARTUP_HUE)
#    define RGB_MATRIX_STARTUP_HUE 0
#endif

#if !defined(RGB_MATRIX_STARTUP_SAT)
#    define RGB_MATRIX_STARTUP_SAT UINT8_MAX
#endif

#if !defined(RGB_MATRIX_STARTUP_VAL)
#    define RGB_MATRIX_STARTUP_VAL RGB_MATRIX_MAXIMUM_BRIGHTNESS
#endif

#if !defined(RGB_MATRIX_STARTUP_SPD)
#    define RGB_MATRIX_STARTUP_SPD UINT8_MAX / 2
#endif

// globals
//rgb_config_t rgb_matrix_config;  // TODO: would like to prefix this with g_ for global consistancy, do this in another pr
//uint32_t     g_rgb_timer;
#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
//uint8_t g_rgb_frame_buffer[MATRIX_ROWS][MATRIX_COLS] = {{0}};
#endif  // RGB_MATRIX_FRAMEBUFFER_EFFECTS
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
//last_hit_t g_last_hit_tracker;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

// internals
//static bool            suspend_state     = false;
//static uint8_t         rgb_last_enable   = UINT8_MAX;
//static uint8_t         rgb_last_effect   = UINT8_MAX;
//static effect_params_t rgb_effect_params = {0, LED_FLAG_ALL, false};
//static rgb_task_states rgb_task_state    = SYNCING;
#if RGB_DISABLE_TIMEOUT > 0
//static uint32_t rgb_anykey_timer;
#endif  // RGB_DISABLE_TIMEOUT > 0

// double buffers
//static uint32_t rgb_timer_buffer;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
//static last_hit_t last_hit_buffer;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

// split rgb matrix
#if defined(RGB_MATRIX_ENABLE) && defined(RGB_MATRIX_SPLIT)
//const uint8_t k_rgb_matrix_split[2] = RGB_MATRIX_SPLIT;
#endif

//EECONFIG_DEBOUNCE_HELPER(rgb_matrix, EECONFIG_RGB_MATRIX, rgb_matrix_config);




static rgb_matrix_state_t matrix_state;

static bool effect_config_valid(rgb_config_t *config)
{
    
    if (config->mode == 0)
    {
        return false;
    }
    /*
    if (config->speed > SPEED_MAX || config->speed < SPEED_MIN) {
        return false;
    }

    if (config->sat == SAT_MIN) {
        return false;
    }

    if (config->val == VAL_MIN) {
        return false;
    }
    */
    return true;
}



static void effect_set_enable(rgb_matrix_state_t *state, uint8_t enable)
{
    state->config->enable = enable;
    eeconfig_update_rgb(state->config, state->config->index);
}
/*
#define ENABLE_DEFAULT      1
#define MODE_DEFAULT        RGB_MATRIX_STARTUP_MODE
#define SPEED_DEFAULT       RGB_MATRIX_STARTUP_SPD
#define HUE_DEFAULT         RGB_MATRIX_STARTUP_HUE
#define SAT_DEFAULT         RGB_MATRIX_STARTUP_SAT
#define VAL_DEFAULT         RGB_MATRIX_STARTUP_VAL
*/

static void effect_update_default(rgb_matrix_state_t *state)
{
    state->config->enable   = 1;
    state->config->mode     = RGB_MATRIX_STARTUP_MODE;
    state->config->speed    = RGB_MATRIX_STARTUP_SPD;
    state->config->hue      = RGB_MATRIX_STARTUP_HUE;
    state->config->sat      = RGB_MATRIX_STARTUP_SAT;
    state->config->val      = RGB_MATRIX_STARTUP_VAL;
    state->flags            = LED_FLAG_ALL;
    eeconfig_update_rgb(state->config, state->config->index);
}


rgb_effect_t rgb_effect_matrix_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    rgb_matrix_state_t *state = &matrix_state; //&matrix_states[index];
    state->config = config;
    state->config->type = RGB_EFFECT_MATRIX;
    state->config->data = state;
    state->index = index;
    state->led_start = led_start;
    state->led_num = led_num;
    state->flags = LED_FLAG_ALL;
    state->g_rgb_timer = 0;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    memset(&state->g_last_hit_tracker, 0, sizeof(last_hit_t));
#endif
#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
    memset(&state->g_rgb_frame_buffer, 0, MATRIX_ROWS*MATRIX_COLS);
#endif
    state->suspend_state = false;
    state->rgb_last_enable = UINT8_MAX;
    state->rgb_last_effect = UINT8_MAX;
    state->rgb_effect_params = (effect_params_t){0, LED_FLAG_ALL, false};
    state->rgb_task_state = SYNCING;
#if RGB_DISABLE_TIMEOUT > 0
    state->rgb_anykey_timer = 0;
#endif  // RGB_DISABLE_TIMEOUT > 0
    state->rgb_timer_buffer = 0;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    memset(&state->last_hit_buffer, 0, sizeof(last_hit_t));
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    state->g_last_hit_tracker.count = 0;
    for (uint8_t i = 0; i < LED_HITS_TO_REMEMBER; ++i) {
        state->g_last_hit_tracker.tick[i] = UINT16_MAX;
    }

    state->last_hit_buffer.count = 0;
    for (uint8_t i = 0; i < LED_HITS_TO_REMEMBER; ++i) {
        state->last_hit_buffer.tick[i] = UINT16_MAX;
    }
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    eeconfig_read_rgb(state->config, state->config->index);
    rgb_effects_debug("RGB Matrix config: index:%d, enable:%d, mode:%d, speed:%d, hue:%d, sat:%d, val:%d\n", 
    index, state->config->enable, state->config->mode, state->config->speed, state->config->hue, state->config->sat, state->config->val);

    if ( !effect_config_valid(state->config)){
        rgb_effects_debug("RGB Matrix config invalid, update to default\n");
        effect_update_default(state);
    }

    return state;
}

bool rgb_effect_matrix_enabled(rgb_effect_t effect)
{
    rgb_matrix_state_t *state = (rgb_matrix_state_t*)effect;
    return state->config->enable ? true :false;
}

void rgb_effect_matrix_toggle(rgb_effect_t effect)
{
    rgb_matrix_state_t *state = (rgb_matrix_state_t*)effect;
    state->config->enable = !state->config->enable;
    effect_set_enable(state, state->config->enable);
}

static void rgb_task_timers(rgb_matrix_state_t *state) {
#if defined(RGB_MATRIX_KEYREACTIVE_ENABLED) || RGB_DISABLE_TIMEOUT > 0
    uint32_t deltaTime = sync_timer_elapsed32(state->rgb_timer_buffer);
#endif  // defined(RGB_MATRIX_KEYREACTIVE_ENABLED) || RGB_DISABLE_TIMEOUT > 0
    state->rgb_timer_buffer = sync_timer_read32();

    // Update double buffer timers
#if RGB_DISABLE_TIMEOUT > 0
    if (state->rgb_anykey_timer + deltaTime <= UINT32_MAX) {
        state->rgb_anykey_timer += deltaTime;
    }
#endif  // RGB_DISABLE_TIMEOUT > 0

    // Update double buffer last hit timers
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    uint8_t count = state->last_hit_buffer.count;
    for (uint8_t i = 0; i < count; ++i) {
        if (UINT16_MAX - deltaTime < state->last_hit_buffer.tick[i]) {
            state->last_hit_buffer.count--;
            continue;
        }
        state->last_hit_buffer.tick[i] += deltaTime;
    }
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED
}

static void rgb_task_sync(rgb_matrix_state_t *state) {
    //eeconfig_flush_rgb_matrix(false);
    // next task
    if (sync_timer_elapsed32(state->g_rgb_timer) >= RGB_MATRIX_LED_FLUSH_LIMIT) state->rgb_task_state = STARTING;
}

static void rgb_task_start(rgb_matrix_state_t *state) {
    // reset iter
    state->rgb_effect_params.iter = 0;

    // update double buffers
    state->g_rgb_timer = state->rgb_timer_buffer;
#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    state->g_last_hit_tracker = state->last_hit_buffer;
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

    // next task
    state->rgb_task_state = RENDERING;
}

static bool rgb_matrix_none(rgb_matrix_state_t *state) {
    if (!state->rgb_effect_params.init) {
        return false;
    }

    effect_set_color_all(state, 0, 0, 0);
    return false;
}

static void rgb_matrix_test(rgb_matrix_state_t *state) {
    // Mask out bits 4 and 5
    // Increase the factor to make the test animation slower (and reduce to make it faster)
    uint8_t factor = 10;
    switch ((state->g_rgb_timer & (0b11 << factor)) >> factor) {
        case 0: {
            effect_set_color_all_rgb(state, 20, 0, 0);
            break;
        }
        case 1: {
            effect_set_color_all_rgb(state, 0, 20, 0);
            break;
        }
        case 2: {
            effect_set_color_all_rgb(state, 0, 0, 20);
            break;
        }
        case 3: {
            effect_set_color_all_rgb(state, 20, 20, 20);
            break;
        }
    }
}

static void rgb_task_render(rgb_matrix_state_t *state, uint8_t effect) {
    bool rendering         = false;
    state->rgb_effect_params.init = (effect != state->rgb_last_effect) || (state->config->enable != state->rgb_last_enable);
    if (state->rgb_effect_params.flags != state->flags) {
        state->rgb_effect_params.flags = state->flags;
        effect_set_color_all(state, 0, 0, 0);
    }

    // each effect can opt to do calculations
    // and/or request PWM buffer updates.
    switch (effect) {
        case RGB_MATRIX_NONE:
            rendering = rgb_matrix_none(state);
            break;

// ---------------------------------------------
// -----Begin rgb effect switch case macros-----
#define RGB_MATRIX_EFFECT(name, ...)          \
    case RGB_MATRIX_##name:                   \
        rendering = name(state); \
        break;
#include "qmk/rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT

#if defined(RGB_MATRIX_CUSTOM_KB) || defined(RGB_MATRIX_CUSTOM_USER)
#    define RGB_MATRIX_EFFECT(name, ...)          \
        case RGB_MATRIX_CUSTOM_##name:            \
            rendering = name(state); \
            break;
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
#    undef RGB_MATRIX_EFFECT
#endif
            // -----End rgb effect switch case macros-------
            // ---------------------------------------------

        // Factory default magic value
        case UINT8_MAX: {
            rgb_matrix_test(state);
            state->rgb_task_state = FLUSHING;
        }
            return;
    }

    state->rgb_effect_params.iter++;

    // next task
    if (!rendering) {
        state->rgb_task_state = FLUSHING;
        if (!state->rgb_effect_params.init && effect == RGB_MATRIX_NONE) {
            // We only need to flush once if we are RGB_MATRIX_NONE
            state->rgb_task_state = SYNCING;
        }
    }
}

static void rgb_task_flush(rgb_matrix_state_t *state, uint8_t effect) {
    // update last trackers after the first full render so we can init over several frames
    state->rgb_last_effect = effect;
    state->rgb_last_enable = state->config->enable;

    // update pwm buffers
    //rgb_matrix_update_pwm_buffers();

    // next task
    state->rgb_task_state = SYNCING;
}

void rgb_effect_matrix_task(rgb_effect_t effect_state)
{
    rgb_matrix_state_t *state = (rgb_matrix_state_t*)effect_state;
    rgb_task_timers(state);

    // Ideally we would also stop sending zeros to the LED driver PWM buffers
    // while suspended and just do a software shutdown. This is a cheap hack for now.
    bool suspend_backlight = state->suspend_state ||
#if RGB_DISABLE_TIMEOUT > 0
                             (state->rgb_anykey_timer > (uint32_t)RGB_DISABLE_TIMEOUT) ||
#endif  // RGB_DISABLE_TIMEOUT > 0
                             false;

    uint8_t effect = suspend_backlight || !state->config->enable ? 0 : state->config->mode;

    switch (state->rgb_task_state) {
        case STARTING:
            rgb_task_start(state);
            break;
        case RENDERING:
            rgb_task_render(state, effect);
            if (effect) {
                //rgb_matrix_indicators();
                //rgb_matrix_indicators_advanced(&rgb_effect_params);
            }
            break;
        case FLUSHING:
            rgb_task_flush(state, effect);
            break;
        case SYNCING:
            rgb_task_sync(state);
            break;
    }
}

void rgb_effect_matrix_init_mode(rgb_effect_t effect)
{
    //rgb_matrix_state_t *state = (rgb_matrix_state_t*)effect;
    //effect_mode_init(state);
}

static void process_rgb_matrix(rgb_matrix_state_t *state, uint8_t row, uint8_t col, bool pressed) {
#ifndef RGB_MATRIX_SPLIT
    //if (!is_keyboard_master()) return;
#endif
#if RGB_DISABLE_TIMEOUT > 0
    state->rgb_anykey_timer = 0;
#endif  // RGB_DISABLE_TIMEOUT > 0

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
    uint8_t led[LED_HITS_TO_REMEMBER];
    uint8_t led_count = 0;

#    if defined(RGB_MATRIX_KEYRELEASES)
    if (!pressed)
#    elif defined(RGB_MATRIX_KEYPRESSES)
    if (pressed)
#    endif  // defined(RGB_MATRIX_KEYRELEASES)
    {
        led_count = rgb_matrix_map_row_column_to_led(row, col, led);
    }

    if (state->last_hit_buffer.count + led_count > LED_HITS_TO_REMEMBER) {
        memcpy(&state->last_hit_buffer.x[0], &state->last_hit_buffer.x[led_count], LED_HITS_TO_REMEMBER - led_count);
        memcpy(&state->last_hit_buffer.y[0], &state->last_hit_buffer.y[led_count], LED_HITS_TO_REMEMBER - led_count);
        memcpy(&state->last_hit_buffer.tick[0], &state->last_hit_buffer.tick[led_count], (LED_HITS_TO_REMEMBER - led_count) * 2);  // 16 bit
        memcpy(&state->last_hit_buffer.index[0], &state->last_hit_buffer.index[led_count], LED_HITS_TO_REMEMBER - led_count);
        state->last_hit_buffer.count = LED_HITS_TO_REMEMBER - led_count;
    }

    for (uint8_t i = 0; i < led_count; i++) {
        uint8_t index                = state->last_hit_buffer.count;
        state->last_hit_buffer.x[index]     = g_led_config.point[led[i]].x;
        state->last_hit_buffer.y[index]     = g_led_config.point[led[i]].y;
        state->last_hit_buffer.index[index] = led[i];
        state->last_hit_buffer.tick[index]  = 0;
        state->last_hit_buffer.count++;
    }
#endif  // RGB_MATRIX_KEYREACTIVE_ENABLED

#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS) && defined(ENABLE_RGB_MATRIX_TYPING_HEATMAP)
    if (state->config->mode == RGB_MATRIX_TYPING_HEATMAP) {
        process_rgb_matrix_typing_heatmap(state, row, col);
    }
#endif  // defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS) && defined(ENABLE_RGB_MATRIX_TYPING_HEATMAP)
}

void hook_matrix_change_rgb(keyevent_t event)
{
    process_rgb_matrix(&matrix_state, event.key.row, event.key.col, event.pressed);
}
