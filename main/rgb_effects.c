/**
 * rgb_effects.c
 * rgb effects implementation
 */

#include <stdlib.h>
#include <stdbool.h>

#include "rgb_effects.h"
#include "timer.h"
#include "eeprom_manager.h"

#define DELAY_MIN   0
#define DELAY_DEFAULT 1500

#define SPEED_MIN 0
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

#define GRADIENT_STEP_DEFAULT   32
#define BREATH_STEP_DEFAULT     32

enum rgb_effects_type {
    RGB_EFFECT_STATIC,
    RGB_EFFECT_BLINK,
    RGB_EFFECT_GRADIENT,
    RGB_EFFECT_RANDOM,
    RGB_EFFECT_BREATH,
    RGB_EFFECT_WIPE,
    RGB_EFFECT_SCAN,
    RGB_EFFECT_MAX
};

typedef union {
    uint32_t raw;
    struct __attribute__((__packed__)) {
        uint8_t enable : 1;
        uint8_t mode : 3;
        uint8_t speed : 4;
        uint8_t hue;
        uint8_t sat;
        uint8_t val;
    };
} rgb_effects_config_t;

typedef void (*RGB_EFFECT_FUN)(void);

typedef struct {
    rgb_effects_config_t    config;
    rgb_driver_t*           driver;
    uint32_t                last_ticks;
    uint32_t                counter;
    uint32_t                gradient_step;
    uint32_t                breath_step;
    bool                    wipe_on;
    RGB_EFFECT_FUN          effects[RGB_EFFECT_MAX];
} rgb_effects_state_t;

static rgb_effects_state_t effects_state;

// utilities
#define RANDOM_DISTANCE 17
static uint8_t get_random_hue(uint8_t hue) { return (rand() % HUE_MAX) + RANDOM_DISTANCE; }

static void effects_mode_init(void)
{
    switch(effects_state.config.mode) {
        case RGB_EFFECT_STATIC:
            break;
        case RGB_EFFECT_BLINK:
            break;
        case RGB_EFFECT_GRADIENT:
            break;
        case RGB_EFFECT_RANDOM:
            break;
        case RGB_EFFECT_BREATH:
            break;
        case RGB_EFFECT_WIPE:
            effects_state.wipe_on = true;
            effects_state.counter = 0;
            effects_state.driver->set_color_all(0, 0, 0);
            break;
        case RGB_EFFECT_SCAN:
            effects_state.counter = 0;
            effects_state.driver->set_color_all(0, 0, 0);
            break;
        default:
            break;
    }
}

static uint32_t effects_delay(void)
{
    switch(effects_state.config.mode) {
        case RGB_EFFECT_STATIC:
            return DELAY_MIN;
        case RGB_EFFECT_BLINK:
            break;
        case RGB_EFFECT_GRADIENT:
            return DELAY_MIN;
        case RGB_EFFECT_RANDOM:
            break;
        case RGB_EFFECT_BREATH:
            break;
        default:
            break;
    }

    return DELAY_DEFAULT;
}

static bool effects_need_update(void) { return timer_elapsed(effects_state.last_ticks)*effects_state.config.speed >= effects_delay(); }
static void effects_update_timer(void) { effects_state.last_ticks = timer_read(); }

//effects
static void effects_mode_static(void)
{
    effects_state.driver->set_color_all(effects_state.config.hue, effects_state.config.sat, effects_state.config.val);
}

static void effects_mode_blink(void)
{
    if(effects_state.counter % 2 == 1) {
        // on
        effects_state.driver->set_color_all(effects_state.config.hue, effects_state.config.sat, effects_state.config.val);
    }
    else {
        // off
        effects_state.driver->set_color_all(0, 0, 0);
    }
    effects_state.counter++;
}

static void effects_mode_random(void)
{
    uint8_t hue = effects_state.config.hue;
    for (int i = 0; i < EFFECTS_LED_NUM; i++) {
        hue = get_random_hue(hue);
        effects_state.driver->set_color_all(hue, effects_state.config.sat, effects_state.config.val);
    }
}

static void effects_mode_gradient(void)
{
    uint8_t step = HUE_MAX / effects_state.gradient_step;

    for (int i = 0; i < EFFECTS_LED_NUM; i++) {
        effects_state.driver->set_color(i, effects_state.config.hue + i*step, effects_state.config.sat, effects_state.config.val);
    }
    effects_state.config.hue += step;
}

static void effects_mode_breath(void)
{
    uint8_t breath = VAL_MAX / effects_state.breath_step;

    effects_state.driver->set_color_all(effects_state.config.hue, effects_state.config.sat, effects_state.config.val);
    effects_state.config.val += breath;
}

static void effects_mode_wipe(void)
{
    if (effects_state.wipe_on) {
        effects_state.driver->set_color(effects_state.counter, effects_state.config.hue, effects_state.config.sat, effects_state.config.val);
    } else {
        effects_state.driver->set_color(effects_state.counter, 0, 0, 0);
    }
    effects_state.counter++;
    if (effects_state.counter == EFFECTS_LED_NUM) {
        effects_state.counter = 0;
        effects_state.wipe_on = !effects_state.wipe_on;
    }
}

static void effects_mode_scan(void)
{
    uint8_t prev = (effects_state.counter == 0) ? EFFECTS_LED_NUM - 1 : effects_state.counter - 1;
    effects_state.driver->set_color(prev, 0, 0, 0);
    effects_state.driver->set_color(effects_state.counter, effects_state.config.hue, effects_state.config.sat, effects_state.config.val);
    effects_state.counter = (effects_state.counter + 1) % EFFECTS_LED_NUM;
}

static void effects_set_hue(uint8_t hue)
{
    effects_state.config.hue = hue;
    eeconfig_update_kb(effects_state.config.raw);
}

static void effects_set_sat(uint8_t sat)
{
    effects_state.config.sat = sat;
    eeconfig_update_kb(effects_state.config.raw);
}

static void effects_set_val(uint8_t val)
{
    effects_state.config.val = val;
    eeconfig_update_kb(effects_state.config.raw);
}

static void effects_set_speed(uint8_t speed)
{
    effects_state.config.speed = !speed ? 1 : speed;
    eeconfig_update_kb(effects_state.config.raw);
}

static void effects_set_mode(uint8_t mode)
{
    effects_state.config.mode = mode;
    effects_mode_init();
    eeconfig_update_kb(effects_state.config.raw);
}

static void effects_set_enable(uint8_t enable)
{
    effects_state.config.enable = enable;
    eeconfig_update_kb(effects_state.config.raw);
}

static void effects_update_default(void)
{
    effects_state.config.enable = 0;
    effects_state.config.mode = RGB_EFFECT_STATIC;
    effects_state.config.speed = SPEED_DEFAULT;
    effects_state.config.hue = HUE_DEFAULT;
    effects_state.config.sat = SAT_DEFAULT;
    effects_state.config.val = VAL_DEFAULT;
    eeconfig_update_kb(effects_state.config.raw);
}

// interface
void rgb_effects_init(rgb_driver_t* driver)
{
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
        effects_update_default();
    } else {
        effects_state.config.raw = eeconfig_read_kb();
    }

    effects_state.driver        = driver;
    effects_state.counter       = 0;
    effects_state.wipe_on       = true;
    effects_state.gradient_step = GRADIENT_STEP_DEFAULT;
    effects_state.breath_step   = BREATH_STEP_DEFAULT;
    effects_state.last_ticks    = timer_read();
    srand(effects_state.last_ticks);
    effects_state.effects[RGB_EFFECT_STATIC]    = effects_mode_static;
    effects_state.effects[RGB_EFFECT_BLINK]     = effects_mode_blink;
    effects_state.effects[RGB_EFFECT_GRADIENT]  = effects_mode_gradient;
    effects_state.effects[RGB_EFFECT_RANDOM]    = effects_mode_random;
    effects_state.effects[RGB_EFFECT_BREATH]    = effects_mode_breath;
    effects_state.effects[RGB_EFFECT_WIPE]      = effects_mode_wipe;
    effects_state.effects[RGB_EFFECT_SCAN]      = effects_mode_scan;
    effects_mode_init();
}

void rgb_effects_inc_hue(void)
{
    effects_set_hue(effects_state.config.hue + HUE_STEP);
}

void rgb_effects_dec_hue(void)
{
    effects_set_hue(effects_state.config.hue - HUE_STEP);
}

void rgb_effects_inc_sat(void)
{
    effects_set_sat(effects_state.config.sat + SAT_STEP);
}

void rgb_effects_dec_sat(void)
{
    effects_set_sat(effects_state.config.sat - SAT_STEP);
}

void rgb_effects_inc_val(void)
{
    effects_set_val(effects_state.config.val + VAL_STEP);
}

void rgb_effects_dec_val(void)
{
    effects_set_val(effects_state.config.val - VAL_STEP);
}

void rgb_effects_inc_speed(void)
{
    effects_set_speed(++effects_state.config.speed);
}

void rgb_effects_dec_speed(void)
{
    effects_set_speed(--effects_state.config.speed);
}

void rgb_effects_inc_mode(void)
{
    effects_state.config.mode++;
    if (effects_state.config.mode >= RGB_EFFECT_MAX) {
        effects_state.config.mode = RGB_EFFECT_STATIC;
    }
    effects_set_mode(effects_state.config.mode);
}

void rgb_effects_dec_mode(void)
{
    if (effects_state.config.mode == 0) {
        effects_state.config.mode = RGB_EFFECT_MAX - 1;
    } else {
        effects_state.config.mode--;
    }

    effects_set_mode(effects_state.config.mode);
}

void rgb_effects_toggle(void)
{
    effects_state.config.enable = !effects_state.config.enable;
    effects_set_enable(effects_state.config.enable);
}

void rgb_effects_task(void)
{
    if (!effects_state.config.enable) {
        effects_state.driver->set_color_all(0, 0, 0);
    } else {
        if (effects_need_update() ) {
            effects_state.effects[effects_state.config.mode]();
            effects_update_timer();
        }
    }
}
