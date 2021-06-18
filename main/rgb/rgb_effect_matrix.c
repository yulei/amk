/**
 * @file rgb_effect_matrix.c
 */

#include <string.h>
#include <stdlib.h>
#include "keyboard.h"
#include "rgb_effect_matrix.h"
#include "rgb_led.h"
#include "timer.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#ifndef RGB_EFFECT_MATRIX_NUM
    #define RGB_EFFECT_MATRIX_NUM 1
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

#define GRADIENT_STEP_DEFAULT   17
#define BREATH_STEP_DEFAULT     32
#define INVALID_LED             0xFF

#define SNAKE_SIZE 3


typedef struct s_rgb_matrix_state rgb_matrix_state_t;
typedef void (*RM_EFFECT_FUN)(rgb_matrix_state_t*);


typedef struct {
    uint8_t row;
    uint8_t column;
    uint8_t dir;
    uint8_t leds_hue[SNAKE_SIZE];
    uint8_t leds[SNAKE_SIZE];
} snake_t;

typedef struct {
    uint8_t val_step;
    uint8_t vals[RGB_MATRIX_LED_NUM];
} keyhit_t;

struct s_rgb_matrix_state {
    rgb_config_t        *config;
    RM_EFFECT_FUN       effects[RM_EFFECT_MAX];
    uint32_t            last_ticks;
    uint8_t             index;
    uint8_t             led_start;
    uint8_t             led_num;
    union {
        snake_t         snake;
        keyhit_t        keyhit;
    } data;
};

static rgb_matrix_state_t matrix_states[RGB_EFFECT_MATRIX_NUM];

static uint8_t key_to_led(uint8_t row, uint8_t col)
{
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        if (g_rgb_led_attrs[i].r == row && g_rgb_led_attrs[i].c == col) {
            return i;
        }
    }

    return INVALID_LED;
}

static bool effect_config_valid(rgb_config_t *config)
{
    if (config->mode >= RM_EFFECT_MAX) {
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

static void effect_set_enable(rgb_matrix_state_t *state, uint8_t enable)
{
    state->config->enable = enable;
    eeconfig_update_rgb(state->config, state->config->index);
}

static void effect_update_default(rgb_matrix_state_t *state)
{
    state->config->enable   = ENABLE_DEFAULT;
    state->config->mode     = MODE_DEFAULT;
    state->config->speed    = SPEED_DEFAULT;
    state->config->hue      = HUE_DEFAULT;
    state->config->sat      = SAT_DEFAULT;
    state->config->val      = VAL_DEFAULT;
    eeconfig_update_rgb(state->config, state->config->index);
}

static uint32_t effect_delay(rgb_matrix_state_t *state)
{
    switch(state->config->mode) {
        default:
            break;
    }

    return DELAY_DEFAULT;
}

static bool effect_need_update(rgb_matrix_state_t *state)
{
    return timer_elapsed32(state->last_ticks)*state->config->speed >= effect_delay(state);
}

static void effect_update_timer(rgb_matrix_state_t *state)
{
    state->last_ticks = timer_read32();
}

static void effect_mode_init(rgb_matrix_state_t *state)
{
    state->config->sat = SAT_DEFAULT;
    state->config->val = VAL_DEFAULT;
    switch(state->config->mode) {
        case RM_EFFECT_STATIC:
            break;
        case RM_EFFECT_BREATH:
            break;
        case RM_EFFECT_GRADIENT:
            break;
        case RM_EFFECT_GRADIENT_V:
            break;
        case RM_EFFECT_RAINBOW:
            break;
        case RM_EFFECT_RAINBOW_V:
            break;
        case RM_EFFECT_ROTATE:
            break;
        case RM_EFFECT_SNAKE:
            state->data.snake.row = 0;
            state->data.snake.column = 0;
            state->data.snake.dir = 1;
            memset(&state->data.snake.leds_hue, 0, SNAKE_SIZE);
            memset(&state->data.snake.leds, INVALID_LED, SNAKE_SIZE);
            effect_set_color_all(state,0,0,0);
            break;
        case RM_EFFECT_KEYHIT:
            memset(&state->data.keyhit.vals, 0, RGB_MATRIX_LED_NUM);
            state->data.keyhit.val_step = 2*VAL_STEP;
            effect_set_color_all(state, 0,0,0);
            break;
        case RM_EFFECT_RANDOM:
            break;
        default:
            break;
    }
}

static void rgb_matrix_mode_random(rgb_matrix_state_t *state)
{
    for (int i = 0; i < state->led_num; i++) {
        effect_set_color(state, i, pick_hue(), state->config->sat, state->config->val);
    }
}

static void rgb_matrix_mode_static(rgb_matrix_state_t *state)
{
    effect_set_color_all(state, state->config->hue, state->config->sat, state->config->val);
}

static void rgb_matrix_mode_breath(rgb_matrix_state_t *state)
{
    effect_set_color_all(state, state->config->hue, state->config->sat, state->config->val);
    state->config->val += BREATH_STEP_DEFAULT;
}

static void rgb_matrix_mode_gradient(rgb_matrix_state_t *state)
{
    uint8_t step = GRADIENT_STEP_DEFAULT;//HUE/MATRIX_COLS;
    for (int i = 0; i < state->led_num; i++) {
        rgb_led_attr_t *attr = &g_rgb_led_attrs[i];
        effect_set_color(state, i, state->config->hue + step*((attr->x*16)/255), state->config->sat, state->config->val);
    }
}

static void rgb_matrix_mode_gradient_v(rgb_matrix_state_t *state)
{
    uint8_t step = GRADIENT_STEP_DEFAULT;//HUE/MATRIX_COLS;
    for (int i = 0; i < state->led_num; i++) {
        rgb_led_attr_t *attr = &g_rgb_led_attrs[i];
        effect_set_color(state, i, state->config->hue + step*((attr->y*16)/255), state->config->sat, state->config->val);
    }
}

static void rgb_matrix_mode_rainbow(rgb_matrix_state_t *state)
{
    uint8_t step = GRADIENT_STEP_DEFAULT;//HUE/MATRIX_COLS;
    for (int i = 0; i < state->led_num; i++) {
        rgb_led_attr_t *attr = &g_rgb_led_attrs[i];
        effect_set_color(state, i, state->config->hue + step*((attr->x*16)/255), state->config->sat, state->config->val);
    }

    state->config->hue += step;
}

static void rgb_matrix_mode_rainbow_v(rgb_matrix_state_t *state)
{
    uint8_t step = GRADIENT_STEP_DEFAULT;//HUE/MATRIX_COLS;
    for (int i = 0; i < state->led_num; i++) {
        rgb_led_attr_t *attr = &g_rgb_led_attrs[i];
        effect_set_color(state, i, state->config->hue + step*((attr->y*16)/255), state->config->sat, state->config->val);
    }

    state->config->hue += step;
}

static uint8_t rotate_hue(uint8_t x, uint8_t y, uint8_t hue) 
{
    int32_t x_off = x > 128? x-128 : -(128-x);
    int32_t y_off = y > 128? y-128 : -(128-y);
    if (x_off > 0) {
        if (y_off > 0) {
            return hue + y_off;
        } else {
            return hue + 256 + y_off;
        }
    } else {
        if (y_off > 0) {
            return hue + 64 + y_off;
        } else {
            return hue + 128 + y_off;
        }
    }
}

static void rgb_matrix_mode_rotate(rgb_matrix_state_t *state)
{
    uint8_t step = GRADIENT_STEP_DEFAULT;//HUE/MATRIX_COLS;
    for (int i = 0; i < state->led_num; i++) {
        rgb_led_attr_t *attr = &g_rgb_led_attrs[i];
        effect_set_color(state, i, rotate_hue(attr->x, attr->y, state->config->hue), state->config->sat, state->config->val);
    }
    state->config->hue += step;
}

static void step_snake(rgb_matrix_state_t *state)
{
    if (state->data.snake.dir) {
        if (state->data.snake.row == MATRIX_ROWS-1) {
            state->data.snake.dir = !state->data.snake.dir;
            state->data.snake.column = (state->data.snake.column+1) % MATRIX_COLS;
        } else {
            state->data.snake.row++;
        }
    } else {
        if (state->data.snake.row == 0) {
            state->data.snake.dir = !state->data.snake.dir;
            state->data.snake.column = (state->data.snake.column+1) % MATRIX_COLS;
        } else {
            state->data.snake.row--;
        }
    }
    state->config->hue += HUE_STEP*3;
}

static void shift_snake(rgb_matrix_state_t *state, uint8_t cur)
{
    for (int i = SNAKE_SIZE-1; i > 0; i--) {
        state->data.snake.leds[i] = state->data.snake.leds[i-1];
        state->data.snake.leds_hue[i] = state->data.snake.leds_hue[i-1];
    }
    state->data.snake.leds[0] = cur;
    state->data.snake.leds_hue[0] = state->config->hue;
}

static void rgb_matrix_mode_snake(rgb_matrix_state_t *state)
{
    uint8_t cur = key_to_led(state->data.snake.row, state->data.snake.column);
    if (cur != INVALID_LED) {
        if (state->data.snake.leds[SNAKE_SIZE-1] != INVALID_LED) {
            effect_set_color(state, state->data.snake.leds[SNAKE_SIZE-1], 0, 0, 0);
        }
        shift_snake(state, cur);
        for (int i = 0; i < SNAKE_SIZE; i++) {
            if (state->data.snake.leds[i] != INVALID_LED) {
                effect_set_color(state, state->data.snake.leds[i], state->data.snake.leds_hue[i], state->config->sat, state->config->val);
            }
        }
    }
    step_snake(state);
}

void hook_matrix_change_rgb(rgb_matrix_state_t *state, keyevent_t event)
{
    if (IS_PRESSED(event)) {
        uint8_t cur = key_to_led(event.key.row, event.key.col);
        if ( cur != INVALID_LED) {
            state->data.keyhit.vals[cur] = VAL_MAX;
        }
    }
}

static void rgb_matrix_mode_keyhit(rgb_matrix_state_t *state)
{
    for (int i = 0; i < state->led_num; i++) {
        effect_set_color(state, i, state->config->hue, state->config->sat, state->data.keyhit.vals[i]);
        if (state->data.keyhit.vals[i] > state->data.keyhit.val_step) {
            state->data.keyhit.vals[i] -= state->data.keyhit.val_step;
        } else {
            state->data.keyhit.vals[i] = 0;
        }
    }
}

static void effect_state_init(rgb_matrix_state_t *state)
{
    state->last_ticks = timer_read32();
    srand(state->last_ticks);
    state->effects[RM_EFFECT_STATIC]     = rgb_matrix_mode_static;
    state->effects[RM_EFFECT_BREATH]     = rgb_matrix_mode_breath;
    state->effects[RM_EFFECT_GRADIENT]   = rgb_matrix_mode_gradient;
    state->effects[RM_EFFECT_RAINBOW]    = rgb_matrix_mode_rainbow;
    state->effects[RM_EFFECT_GRADIENT_V] = rgb_matrix_mode_gradient_v;
    state->effects[RM_EFFECT_RAINBOW_V]  = rgb_matrix_mode_rainbow_v;
    state->effects[RM_EFFECT_ROTATE]     = rgb_matrix_mode_rotate;
    state->effects[RM_EFFECT_SNAKE]      = rgb_matrix_mode_snake;
    state->effects[RM_EFFECT_KEYHIT]     = rgb_matrix_mode_keyhit;
    state->effects[RM_EFFECT_RANDOM]     = rgb_matrix_mode_random;
    effect_mode_init(state);
}

rgb_effect_t rgb_effect_matrix_init(rgb_config_t* config, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    rgb_matrix_state_t *state = &matrix_states[index];
    state->config = config;
    state->config->type = RGB_EFFECT_MATRIX;
    state->config->data = state;
    state->index = index;
    state->led_start = led_start;
    state->led_num = led_num;
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    eeconfig_read_rgb(state->config, state->config->index);
    rgb_effects_debug("RGB Matrix config: index:%d, enable:%d, mode:%d, speed:%d, hue:%d, sat:%d, val:%d\n", 
    index, state->config->enable, state->config->mode, state->config->speed, state->config->hue, state->config->sat, state->config->val);

    if ( !effect_config_valid(state->config)){
        effect_update_default(state);
    }

    effect_state_init(state);

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

void rgb_effect_matrix_task(rgb_effect_t effect)
{
    rgb_matrix_state_t *state = (rgb_matrix_state_t*)effect;
    if (state->config->enable) {
        if (effect_need_update(state) ) {
            state->effects[state->config->mode](state);
            effect_update_timer(state);
        }
    } else {
        effect_set_color_all(state, 0, 0, 0);
    }
}

void rgb_effect_matrix_init_mode(rgb_effect_t effect)
{
    rgb_matrix_state_t *state = (rgb_matrix_state_t*)effect;
    effect_mode_init(state);
}
