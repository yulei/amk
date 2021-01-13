/**
 * rgb_matrix.c
 */

#include <stdlib.h>
#include <string.h>

#include "rgb_color.h"
#include "rgb_matrix.h"
#include "eeprom_manager.h"
#include "timer.h"
#include "is31.h"
#include "keyboard.h"

#define MATRIX_HUE_STEP 10
#define MATRIX_SAT_STEP 10
#define MATRIX_VAL_STEP 10
#define RANDOM_DISTANCE 17

#define DELAY_MIN   0
#define DELAY_DEFAULT 25500

#define SPEED_MIN 0
#define SPEED_MAX 255
#define SPEED_DEFAULT 120
#define SPEED_STEP 10

#define HUE_MIN 0
#define HUE_MAX 255
#define HUE_DEFAULT HUE_MIN
#define HUE_STEP MATRIX_HUE_STEP

#define SAT_MIN 0
#define SAT_MAX 255
#define SAT_DEFAULT SAT_MAX
#define SAT_STEP MATRIX_SAT_STEP

#define VAL_MIN 0
#define VAL_MAX 255
#define VAL_DEFAULT VAL_MAX
#define VAL_STEP MATRIX_VAL_STEP

#define GRADIENT_STEP_DEFAULT   32
#define BREATH_STEP_DEFAULT     32
#define INVALID_LED             0xFF

typedef enum {
    RM_EFFECT_STATIC,
    RM_EFFECT_BREATH,
    RM_EFFECT_GRADIENT,
    RM_EFFECT_RAINBOW,
    RM_EFFECT_GRADIENT_V,
    RM_EFFECT_RAINBOW_V,
    RM_EFFECT_ROTATE,
    RM_EFFECT_SNAKE,
    RM_EFFECT_KEYHIT,
    RM_EFFECT_RANDOM,
    RM_EFFECT_TEST,
#define RM_EFFECT_MAX RM_EFFECT_TEST
} rgb_matrix_effect_type_t;

typedef void (*RM_EFFECT_FUN)(void);

#define SNAKE_SIZE 3

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

typedef struct {
    rgb_matrix_config_t config;
    rgb_driver_t*       driver;
    union {
        snake_t         snake;
        keyhit_t        keyhit;
    } data;
    uint32_t            last_ticks;
    RM_EFFECT_FUN       effects[RM_EFFECT_MAX];
} rgb_matrix_state_t;

static rgb_matrix_state_t matrix_state;

static uint32_t update_delay(void)
{
    switch(matrix_state.config.mode) {
        case RM_EFFECT_RAINBOW:
            return DELAY_DEFAULT;
        default:
            break;
    }

    return DELAY_DEFAULT;
}

static bool rgb_matrix_need_update(void)
{
    return timer_elapsed32(matrix_state.last_ticks)*matrix_state.config.speed >= update_delay();
}

static uint8_t key_to_led(uint8_t row, uint8_t col);
static void rgb_matrix_update_timer(void) { matrix_state.last_ticks = timer_read32(); }

static uint8_t get_random_hue(uint8_t hue) { return (rand() % HUE_MAX) + RANDOM_DISTANCE; }

static void rgb_matrix_mode_preinit(void)
{
    matrix_state.config.sat = SAT_DEFAULT;
    matrix_state.config.val = VAL_DEFAULT;
}

static void rgb_matrix_mode_postinit(void)
{}

static void rgb_matrix_mode_init(void)
{
    rgb_matrix_mode_preinit();
    switch(matrix_state.config.mode) {
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
            matrix_state.data.snake.row = 0;
            matrix_state.data.snake.column = 0;
            matrix_state.data.snake.dir = 1;
            memset(&matrix_state.data.snake.leds_hue, 0, SNAKE_SIZE);
            memset(&matrix_state.data.snake.leds, INVALID_LED, SNAKE_SIZE);
            matrix_state.driver->set_color_all(0,0,0);
            break;
        case RM_EFFECT_KEYHIT:
            memset(&matrix_state.data.keyhit.vals, 0, RGB_MATRIX_LED_NUM);
            matrix_state.data.keyhit.val_step = 2*VAL_STEP;
            matrix_state.driver->set_color_all(0,0,0);
            break;
        case RM_EFFECT_RANDOM:
            break;
        default:
            break;
    }
    rgb_matrix_mode_postinit();
}
static void rgb_matrix_mode_test(void)
{
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        matrix_state.driver->set_color( i, get_random_hue(matrix_state.config.hue), matrix_state.config.sat, matrix_state.config.val);
    }
}

static void rgb_matrix_mode_static(void)
{
    matrix_state.driver->set_color_all(matrix_state.config.hue, matrix_state.config.sat, matrix_state.config.val);
}

static void rgb_matrix_mode_breath(void)
{
    matrix_state.driver->set_color_all(matrix_state.config.hue, matrix_state.config.sat, matrix_state.config.val);
    matrix_state.config.val += BREATH_STEP_DEFAULT;
}

static void rgb_matrix_mode_gradient(void)
{
    uint8_t step = HUE_MAX/MATRIX_COLS;
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->x/step), matrix_state.config.sat, matrix_state.config.val);
    }
}

static void rgb_matrix_mode_gradient_v(void)
{
    uint8_t step = HUE_MAX/MATRIX_ROWS;
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->y/step), matrix_state.config.sat, matrix_state.config.val);
    }
}

static void rgb_matrix_mode_rainbow(void)
{
    uint8_t step = HUE_MAX/MATRIX_COLS;
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->x/step), matrix_state.config.sat, matrix_state.config.val);
    }

    matrix_state.config.hue += step;
}

static void rgb_matrix_mode_rainbow_v(void)
{
    uint8_t step = HUE_MAX/MATRIX_ROWS;
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->y/step), matrix_state.config.sat, matrix_state.config.val);
    }

    matrix_state.config.hue += step;
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

static void rgb_matrix_mode_rotate(void)
{
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, rotate_hue(attr->x, attr->y, matrix_state.config.hue), matrix_state.config.sat, matrix_state.config.val);
    }
    matrix_state.config.hue += HUE_STEP;
}

static void step_snake(void)
{
    if (matrix_state.data.snake.dir) {
        if (matrix_state.data.snake.row == MATRIX_ROWS-1) {
            matrix_state.data.snake.dir = !matrix_state.data.snake.dir;
            matrix_state.data.snake.column = (matrix_state.data.snake.column+1) % MATRIX_COLS;
        } else {
            matrix_state.data.snake.row++;
        }
    } else {
        if (matrix_state.data.snake.row == 0) {
            matrix_state.data.snake.dir = !matrix_state.data.snake.dir;
            matrix_state.data.snake.column = (matrix_state.data.snake.column+1) % MATRIX_COLS;
        } else {
            matrix_state.data.snake.row--;
        }
    }
    matrix_state.config.hue += HUE_STEP*3;
}

static void shift_snake(uint8_t cur)
{
    for (int i = SNAKE_SIZE-1; i > 0; i--) {
        matrix_state.data.snake.leds[i] = matrix_state.data.snake.leds[i-1];
        matrix_state.data.snake.leds_hue[i] = matrix_state.data.snake.leds_hue[i-1];
    }
    matrix_state.data.snake.leds[0] = cur;
    matrix_state.data.snake.leds_hue[0] = matrix_state.config.hue;
}

static void rgb_matrix_mode_snake(void)
{
    uint8_t cur = key_to_led(matrix_state.data.snake.row, matrix_state.data.snake.column);
    if (cur != INVALID_LED) {
        if (matrix_state.data.snake.leds[SNAKE_SIZE-1] != INVALID_LED) {
            matrix_state.driver->set_color(matrix_state.data.snake.leds[SNAKE_SIZE-1], 0, 0, 0);
        }
        shift_snake(cur);
        for (int i = 0; i < SNAKE_SIZE; i++) {
            if (matrix_state.data.snake.leds[i] != INVALID_LED) {
                matrix_state.driver->set_color(matrix_state.data.snake.leds[i], matrix_state.data.snake.leds_hue[i], matrix_state.config.sat, matrix_state.config.val);
            }
        }
    }
    step_snake();
}

void hook_matrix_change_rgb(keyevent_t event)
{
    if (IS_PRESSED(event)) {
        uint8_t cur = key_to_led(event.key.row, event.key.col);
        if ( cur != INVALID_LED) {
            matrix_state.data.keyhit.vals[cur] = VAL_MAX;
        }
    }
}

static void rgb_matrix_mode_keyhit(void)
{
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        matrix_state.driver->set_color(i, matrix_state.config.hue, matrix_state.config.sat, matrix_state.data.keyhit.vals[i]);
        if (matrix_state.data.keyhit.vals[i] > matrix_state.data.keyhit.val_step) {
            matrix_state.data.keyhit.vals[i] -= matrix_state.data.keyhit.val_step;
        } else {
            matrix_state.data.keyhit.vals[i] = 0;
        }
    }
}

void rgb_matrix_update_default(void)
{
    matrix_state.config.enable  = 1;
    matrix_state.config.mode    = RM_EFFECT_GRADIENT;
    matrix_state.config.speed   = SPEED_DEFAULT;
    matrix_state.config.hue     = HUE_DEFAULT;
    matrix_state.config.sat     = SAT_DEFAULT;
    matrix_state.config.val     = VAL_DEFAULT;
    eeconfig_write_rgb_matrix(&matrix_state.config);
}

void rgb_matrix_init(rgb_driver_t *driver)
{
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }

    eeconfig_read_rgb_matrix(&matrix_state.config);
    if (matrix_state.config.mode == 0xFF) {
        rgb_matrix_update_default();
    }

    matrix_state.driver = driver;
    if (matrix_state.config.enable)
        matrix_state.driver->init();

    matrix_state.last_ticks = timer_read32();
    srand(matrix_state.last_ticks);
    matrix_state.effects[RM_EFFECT_STATIC]     = rgb_matrix_mode_static;
    matrix_state.effects[RM_EFFECT_BREATH]     = rgb_matrix_mode_breath;
    matrix_state.effects[RM_EFFECT_GRADIENT]   = rgb_matrix_mode_gradient;
    matrix_state.effects[RM_EFFECT_RAINBOW]    = rgb_matrix_mode_rainbow;
    matrix_state.effects[RM_EFFECT_GRADIENT_V] = rgb_matrix_mode_gradient_v;
    matrix_state.effects[RM_EFFECT_RAINBOW_V]  = rgb_matrix_mode_rainbow_v;
    matrix_state.effects[RM_EFFECT_ROTATE]     = rgb_matrix_mode_rotate;
    matrix_state.effects[RM_EFFECT_SNAKE]      = rgb_matrix_mode_snake;
    matrix_state.effects[RM_EFFECT_KEYHIT]     = rgb_matrix_mode_keyhit;
    matrix_state.effects[RM_EFFECT_RANDOM]     = rgb_matrix_mode_test;
    rgb_matrix_mode_init();
}

bool rgb_matrix_enabled(void)
{
    return matrix_state.config.enable ? true : false;
}


static void rgb_matrix_set_hue(uint8_t hue)
{
    matrix_state.config.hue = hue;
    eeconfig_update_rgb_matrix(&matrix_state.config);
}

static void rgb_matrix_set_sat(uint8_t sat)
{
    matrix_state.config.sat = sat;
    eeconfig_update_rgb_matrix(&matrix_state.config);
}

static void rgb_matrix_set_val(uint8_t val)
{
    matrix_state.config.val = val;
    eeconfig_update_rgb_matrix(&matrix_state.config);
}

static void rgb_matrix_set_speed(uint8_t speed)
{
    matrix_state.config.speed = !speed ? 1 : speed;
    eeconfig_update_rgb_matrix(&matrix_state.config);
}

static void rgb_matrix_set_mode(uint8_t mode)
{
    matrix_state.config.mode = mode;
    rgb_matrix_mode_init();
    eeconfig_update_rgb_matrix(&matrix_state.config);
}

static void rgb_matrix_set_enable(uint8_t enable)
{
    matrix_state.config.enable = enable;
    eeconfig_update_rgb_matrix(&matrix_state.config);
}

void rgb_matrix_inc_hue(void)
{
    rgb_matrix_set_hue(matrix_state.config.hue + HUE_STEP);
}

void rgb_matrix_dec_hue(void)
{
    rgb_matrix_set_hue(matrix_state.config.hue - HUE_STEP);
}

void rgb_matrix_inc_sat(void)
{
    rgb_matrix_set_sat(matrix_state.config.sat + SAT_STEP);
}

void rgb_matrix_dec_sat(void)
{
    rgb_matrix_set_sat(matrix_state.config.sat - SAT_STEP);
}

void rgb_matrix_inc_val(void)
{
    rgb_matrix_set_val(matrix_state.config.val + VAL_STEP);
}

void rgb_matrix_dec_val(void)
{
    rgb_matrix_set_val(matrix_state.config.val - VAL_STEP);
}

void rgb_matrix_inc_speed(void)
{
    rgb_matrix_set_speed(matrix_state.config.speed + SPEED_STEP);
}

void rgb_matrix_dec_speed(void)
{
    rgb_matrix_set_speed(matrix_state.config.speed - SPEED_STEP);
}

void rgb_matrix_inc_mode(void)
{
    matrix_state.config.mode = (matrix_state.config.mode+1) % RM_EFFECT_MAX;
    rgb_matrix_set_mode(matrix_state.config.mode);
}

void rgb_matrix_dec_mode(void)
{
    matrix_state.config.mode = (matrix_state.config.mode-1) % RM_EFFECT_MAX;
    rgb_matrix_set_mode(matrix_state.config.mode);
}

void rgb_matrix_toggle(void)
{
    matrix_state.config.enable = !matrix_state.config.enable;

    rgb_matrix_set_enable(matrix_state.config.enable);
}

void rgb_matrix_task(void)
{
    if (matrix_state.config.enable) {
        if (rgb_matrix_need_update() ) {
            matrix_state.effects[matrix_state.config.mode]();
            rgb_matrix_update_timer();
        }
    } else {
        matrix_state.driver->set_color_all(0,0,0);
    }
    matrix_state.driver->flush();
}

static uint8_t key_to_led(uint8_t row, uint8_t col)
{
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        if (g_rgb_matrix.attributes[i].r == row && g_rgb_matrix.attributes[i].c == col) {
            return i;
        }
    }

    return INVALID_LED;
}