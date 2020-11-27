/**
 * rgb_matrix.c
 */

#include "rgb_color.h"
#include "rgb_matrix.h"

#define DELAY_MIN   0
#define DELAY_DEFAULT 25500

#define SPEED_MIN 0
#define SPEED_MAX 255
#define SPEED_DEFAULT 120
#define SPEED_STEP 10

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


typedef enum {
    RM_EFFECT_STATIC,
    RM_EFFECT_BREATH,
    RM_EFFECT_GRADIENT,
    RM_EFFECT_RAINBOW,
    RM_EFFECT_ROTATE,
    RM_EFFECT_SNAKE,
    RM_EFFECT_KEY_HIT,
    RM_EFFECT_TEST,
#define RM_EFFECT_MAX RM_EFFECT_TEST
} rgb_matrix_effect_type_t;

typedef void (*RM_EFFECT_FUN)(void);

typedef struct {
    rgb_matrix_config_t config;
    rgb_driver_t*       driver;
    uint32_t            last_ticks;
    RM_EFFECT_FUN effect_funs[RM_EFFECT_MAX];
} rgb_matrix_state_t;

static rgb_matrix_state_t matrix_state;

static void rgb_matrix_update_default(void)
{
    matrix_state.config.enable  = 1;
    matrix_state.config.mode    = RM_EFFECT_GRADIENT;
    matrix_state.config.speed   = SPEED_DEFAULT;
    matrix_state.config.hue     = HUE_DEFAULT;
    matrix_state.config.sat     = SAT_DEFAULT;
    matrix_state.config.val     = VAL_DEFAULT;
}

static uint32_t update_delay(void)
{
    switch(matrix.config.mode) {
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
        matrix_state.driver->set_color( i, get_random_hue(), matrix_state.config.sat, matrix_state.config.val);
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
        rgb_matrix_led_attribute_t *attr = &g_rgb_matrix_leds.attributes[i];
        matrix_state.driver->set_color( i, matrix_state.config.hue + step*(attr->x/step), matrix_state.config.sat, matrix_state.config.val);
    }
}

static void rgb_matrix_mode_rainbow(void)
{
    uint8_t step = HUE_MAX/7;
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        rgb_matrix_led_attribute_t *attr = &g_rgb_matrix_leds.attributes[i];
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

void rgb_matrix_init(rgb_driver_t *driver)
{
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }
    eeconfig_read_rgb_matrix(&matrix_state.config);

    effects_state.driver        = driver;
    if (effects_state.config.enable)
        effects_state.driver->init();

    effects_state.counter       = 0;
    effects_state.wipe_on       = true;
    effects_state.rainbow_step  = RAINBOW_STEP_DEFAULT;
    effects_state.breath_step   = BREATH_STEP_DEFAULT;
    effects_state.last_ticks    = timer_read32();
    srand(effects_state.last_ticks);
    effects_state.effects[RM_EFFECT_STATIC]     = rgb_matrix_mode_static;
    effects_state.effects[RM_EFFECT_BREATH]     = rgb_matrix_mode_breath;
    effects_state.effects[RM_EFFECT_GRADIENT]   = rgb_matrix_mode_gradient;
    effects_state.effects[RM_EFFECT_RAINBOW]    = rgb_matrix_mode_rainbow;
    effects_state.effects[RM_EFFECT_ROTATE]     = rgb_matrix_mode_gradient;
    effects_state.effects[RM_EFFECT_SNAKE]      = rgb_matrix_mode_snake;
    effects_state.effects[RM_EFFECT_KEYHIT]     = rgb_matrix_mode_keyhit;
    effects_mode_init();

}

bool rgb_matrix_enabled(void)
{
    return matrix_state.config.enable ? true : false;
}

void rgb_matrix_inc_hue(void)
{}
void rgb_matrix_dec_hue(void)
{}
void rgb_matrix_inc_sat(void)
{}
void rgb_matrix_dec_sat(void)
{}
void rgb_matrix_inc_val(void)
{}
void rgb_matrix_dec_val(void)
{}
void rgb_matrix_inc_speed(void)
{}
void rgb_matrix_dec_speed(void)
{}
void rgb_matrix_inc_mode(void)
{}
void rgb_matrix_dec_mode(void)
{}

void rgb_matrix_toggle(void)
{
    matrix_state.config.enable = !matrix_state.config.enable;
}

void rgb_matrix_task(void)
{
    if (matrix_state.config.enable) {
        if (rgb_matrix_need_update() ) {
            matrix_state.effects[matrix_state.config.mode]();
            rgb_matrix_update_timer();
        }
    } else {
        matrix_state.driver->set_color_all(0, 0, 0);
    }

    matrix_state.driver->flush();
}