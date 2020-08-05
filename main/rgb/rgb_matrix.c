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

typedef struct {
    uint8_t enable;
    uint8_t mode;
    uint8_t speed;
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
} rgb_matrix_config_t;

typedef enum {
    RM_EFFECT_STATIC,
    RM_EFFECT_BREATH,
    RM_EFFECT_GRADIENT,
    RM_EFFECT_ROTATE,
    RM_EFFECT_SNAKE,
    RM_EFFECT_KEY_HIT,
    RM_EFFECT_TEST,
#define RM_EFFECT_MAX RM_EFFECT_TEST
} rgb_matrix_effect_type_t;

typdef void (*RM_EFFECT_FUN)();

typedef struct {
    rgb_matrix_config_t config;
    rgb_driver_t*       driver;
    uint32_t            last_ticks;
    RM_EFFECT_FUN effect_funs[RM_EFFECT_MAX];
} rgb_matrix_state_t;

static rgb_matrix_state_t matrix_state;

static void rgb_matrix_update_default(void)
{
    matrix_state.config.enable = 1;
    matrix_state.config.mode = RM_EFFECT_GRADIENT;
    matrix_state.config.speed = SPEED_DEFAULT;
    matrix_state.config.hue = HUE_DEFAULT;
    matrix_state.config.sat = SAT_DEFAULT;
    matrix_state.config.val = VAL_DEFAULT;
}

void rgb_matrix_init(rgb_driver_t *driver)
{
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
    } else {
        matrix_state.driver->set_color_all(0, 0, 0);
    }

    matrix_state.driver->flush();
}