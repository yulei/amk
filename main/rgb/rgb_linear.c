/**
 * @file rgb_linear.c
 */

#include "rgb_linear.h"
#include "rgb_driver.h"
#include "rgb_effect_linear.h"
#include "amk_store.h"

typedef struct {
    rgb_effect_t effects[RGB_SEGMENT_NUM];
} rgb_linear_t;

static rgb_linear_t rgb_linear;
// global led cache
struct amk_led amk_leds[RGB_LED_NUM];

void rgb_linear_init(void)
{
    // initialize rgb effects
    for (int i = 0; i < RGB_SEGMENT_NUM; i++) {
        uint8_t config = g_rgb_linear_params[i].config;
        g_rgb_configs[config].index = config;
        rgb_linear.effects[i] = rgb_effect_linear_init(&g_rgb_configs[config], i, g_rgb_linear_params[i].led_start, g_rgb_linear_params[i].led_num);
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

void rgb_linear_prepare_sleep(void)
{}

void rgb_linear_set_rgb(uint8_t index, uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
    if (index < RGB_SEGMENT_NUM) {
        rgb_effect_linear_set_rgb(rgb_linear.effects[index], led, r, g, b);
    }
}

void rgb_linear_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param)
{
    if (index < RGB_LED_NUM) {
        *hue = amk_leds[index].hue;
        *sat = amk_leds[index].sat;
        *val = amk_leds[index].val;
        *param = amk_leds[index].param;
    }
}

void rgb_linear_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param)
{
    if (index < RGB_LED_NUM) {
        bool changed = false;
        if (amk_leds[index].hue != hue) {
            amk_leds[index].hue = hue;
            changed = true;
        }
        if (amk_leds[index].sat != sat) {
            amk_leds[index].sat = sat;
            changed = true;
        }
        if (amk_leds[index].val != val) {
            amk_leds[index].val = val;
            changed = true;
        }
        if (amk_leds[index].param != param) {
            amk_leds[index].param = param;
            changed = true;
        }
        if (changed) {
            amk_store_set_led(index, &amk_leds[index]);
        }
    }
}
