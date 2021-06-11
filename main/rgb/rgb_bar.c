/**
 * @file rgb_bar.c
 */

#include "rgb_bar.h"
#include "rgb_driver.h"
#include "rgb_effects.h"

typedef struct {
    rgb_effects_t   effects[RGB_SEGMENT_NUM];
} rgb_bar_t;

rgb_bar_t rgb_bar;

void rgb_bar_init(void)
{
    // initialize rgb effects
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        rgb_driver_t *driver = rgb_driver_get(g_rgb_bar_params[i].device);
        rgb_bar.effects[i] = rgb_effects_init(driver, i, g_rgb_bar_params[i].led_start, g_rgb_bar_params[i].led_count);
    }
}

void rgb_bar_task(void)
{
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        rgb_effects_task(rgb_bar.effects[i]);
    }
}

void rgb_bar_uninit(void)
{
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        rgb_effects_uninit(rgb_bar.effects[i]);
    }
}