/**
 * @file rgb_linear.c
 */

#include "rgb_linear.h"
#include "rgb_driver.h"
#include "rgb_effect_linear.h"

typedef struct {
    rgb_effect_t effects[RGB_SEGMENT_NUM];
} rgb_linear_t;

static rgb_linear_t rgb_linear;

void rgb_linear_init(void)
{
    // initialize rgb effects
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        rgb_linear.effects[i] = rgb_effect_linear_init(&g_rgb_configs[i], i, g_rgb_linear_params[i].led_start, g_rgb_linear_params[i].led_count);
    }
}

void rgb_linear_task(void)
{
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        rgb_effect_linear_task(rgb_linear.effects[i]);
    }
}

void rgb_linear_uninit(void)
{
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        rgb_effect_linear_uninit(rgb_linear.effects[i]);
    }
}