/**
 * rgb_matrix.c
 */

#include <stdlib.h>

#include "rgb_color.h"
#include "rgb_matrix.h"
#include "eeprom_manager.h"
#include "timer.h"
#include "is31.h"

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


typedef enum {
    RM_EFFECT_STATIC,
    RM_EFFECT_BREATH,
    RM_EFFECT_GRADIENT,
    RM_EFFECT_RAINBOW,
    RM_EFFECT_ROTATE,
    RM_EFFECT_SNAKE,
    RM_EFFECT_KEYHIT,
    RM_EFFECT_TEST,
#define RM_EFFECT_MAX RM_EFFECT_TEST
} rgb_matrix_effect_type_t;

typedef void (*RM_EFFECT_FUN)(void);

typedef struct {
    rgb_matrix_config_t config;
    rgb_driver_t*       driver;
    uint32_t            last_ticks;
    RM_EFFECT_FUN       effects[RM_EFFECT_MAX];
} rgb_matrix_state_t;

static rgb_matrix_state_t matrix_state;

static uint32_t update_delay(void)
{
    switch(matrix_state.config.mode) {
        default:
            break;
    }

    return DELAY_DEFAULT;
}

static bool rgb_matrix_need_update(void)
{
    return timer_elapsed32(matrix_state.last_ticks)*matrix_state.config.speed >= update_delay();
}

static void rgb_matrix_update_timer(void) { matrix_state.last_ticks = timer_read32(); }

static uint8_t get_random_hue(uint8_t hue) { return (rand() % HUE_MAX) + RANDOM_DISTANCE; }

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
    uint8_t step = HUE_MAX/(MATRIX_COLS/3);
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->x/step), matrix_state.config.sat, matrix_state.config.val);
    }
}

static void rgb_matrix_mode_rainbow(void)
{
    uint8_t step = HUE_MAX/7;
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        is31_led_attribute_t *attr = &g_rgb_matrix.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->x/step), matrix_state.config.sat, matrix_state.config.val);
    }

    matrix_state.config.hue += step;
}

static void rgb_matrix_mode_rotate(void)
{
    rgb_matrix_mode_test();
}

static void rgb_matrix_mode_snake(void)
{
    rgb_matrix_mode_test();
}

static void rgb_matrix_mode_keyhit(void)
{
    rgb_matrix_mode_test();
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

    matrix_state.driver        = driver;
    if (matrix_state.config.enable)
        matrix_state.driver->init();

    matrix_state.last_ticks    = timer_read32();
    srand(matrix_state.last_ticks);
    matrix_state.effects[RM_EFFECT_STATIC]     = rgb_matrix_mode_static;
    matrix_state.effects[RM_EFFECT_BREATH]     = rgb_matrix_mode_breath;
    matrix_state.effects[RM_EFFECT_GRADIENT]   = rgb_matrix_mode_gradient;
    matrix_state.effects[RM_EFFECT_RAINBOW]    = rgb_matrix_mode_rainbow;
    matrix_state.effects[RM_EFFECT_ROTATE]     = rgb_matrix_mode_rotate;
    matrix_state.effects[RM_EFFECT_SNAKE]      = rgb_matrix_mode_snake;
    matrix_state.effects[RM_EFFECT_KEYHIT]     = rgb_matrix_mode_keyhit;
    //effects_mode_init();
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
    //effects_mode_init();
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
        matrix_state.driver->flush();
    }
}