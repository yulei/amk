/**
 * @file rgb_indicator.c
 * @author astor
 * 
 * @copyright Copyright (c) 2024
 */

#include "rgb_indicator.h"
#include "rgb_driver.h"
#include "amk_gpio.h"
#include "wait.h"
#include "host.h"
#include "led.h"
#include "rgb_led.h"
#include "amk_store.h"

#ifdef RGB_LINEAR_ENABLE
#include "rgb_linear.h"
#include "rgb_effect_linear.h"
#endif
#ifdef RGB_MATRIX_ENABLE
#include "rgb_matrix_stub.h"
#endif

typedef struct rgb_indicator_state {
    struct amk_led led;
} rgb_indicator_state_t;

static rgb_indicator_state_t  s_indicators[RGB_INDICATOR_NUM];
static led_t s_led;
static bool s_force_update;

void rgb_indicator_init(void)
{
    for (int i = 0;i < RGB_INDICATOR_NUM; i++) {
        amk_store_get_led(g_rgb_indicators[i].index, &s_indicators[i].led);
    }
    s_led.raw = 0;
    s_force_update = false;
}

static bool is_effect_enabled(uint8_t config)
{
#ifdef RGB_MATRIX_ENABLE
    if (g_rgb_configs[config].type == RGB_EFFECT_MATRIX) {
        return rgb_matrix_is_enabled() != 0;
    }
#endif
#ifdef RGB_LINEAR_ENABLE
    if (g_rgb_configs[config].type == RGB_EFFECT_LINEAR) {
        return g_rgb_configs[config].enable != 0;
    }
#endif
    return false;
}

void rgb_indicator_task(void)
{
    led_t cur = host_keyboard_led_state();
    bool updated = false;

    for (int i = 0; i < RGB_INDICATOR_NUM; i++) {
        struct rgb_indicator * rgb_led = &g_rgb_indicators[i];
        if (rgb_led->config != RGB_INDICATOR_STANDALONE) {
            if (is_effect_enabled(rgb_led->config)) {
                return;
            }
        }

        rgb_driver_t *driver = rgb_led_map(rgb_led->index);
        if (!driver) continue;

        if (rgb_led->type == RGB_INDICATOR_NUM_LOCK) {
            if (s_force_update || (cur.num_lock != s_led.num_lock)) {
                if (cur.num_lock) {
                    driver->set_color(driver, rgb_led->index, s_indicators[i].led.hue, s_indicators[i].led.sat, s_indicators[i].led.val);
                } else {
                    driver->set_color(driver, rgb_led->index, 0, 0, 0);
                }
                s_led.num_lock = cur.num_lock;
                updated = true;
            }
        }

        if (g_rgb_indicators[i].type == RGB_INDICATOR_CAPS_LOCK) {
            if (s_force_update || (cur.caps_lock != s_led.caps_lock)) {
                if (cur.caps_lock) {
                    driver->set_color(driver, rgb_led->index, s_indicators[i].led.hue, s_indicators[i].led.sat, s_indicators[i].led.val);
                } else {
                    driver->set_color(driver, rgb_led->index, 0, 0, 0);
                }
                s_led.caps_lock = cur.caps_lock;
                updated = true;
            }
        }
        if (g_rgb_indicators[i].type == RGB_INDICATOR_SCROLL_LOCK) {
            if (s_force_update || (cur.scroll_lock != s_led.scroll_lock)) {
                if (cur.scroll_lock) {
                    driver->set_color(driver, rgb_led->index, s_indicators[i].led.hue, s_indicators[i].led.sat, s_indicators[i].led.val);
                } else {
                    driver->set_color(driver, rgb_led->index, 0, 0, 0);
                }
                s_led.scroll_lock = cur.scroll_lock;
                updated = true;
            }
        }
        if (g_rgb_indicators[i].type == RGB_INDICATOR_COMPOSE) {
            if (s_force_update || (cur.compose != s_led.compose)) {
                if (cur.compose) {
                    driver->set_color(driver, rgb_led->index, s_indicators[i].led.hue, s_indicators[i].led.sat, s_indicators[i].led.val);
                } else {
                    driver->set_color(driver, rgb_led->index, 0, 0, 0);
                }
                s_led.compose = cur.compose;
                updated = true;
            }
        }
        if (g_rgb_indicators[i].type == RGB_INDICATOR_KANA) {
            if (s_force_update || (cur.kana != s_led.kana)) {
                if (cur.kana) {
                    driver->set_color(driver, rgb_led->index, s_indicators[i].led.hue, s_indicators[i].led.sat, s_indicators[i].led.val);
                } else {
                    driver->set_color(driver, rgb_led->index, 0, 0, 0);
                }
                s_led.kana= cur.kana;
                updated = true;
            }
        }
    }

    if (updated) {
        s_force_update = false;
    }
}

void rgb_indicator_uninit(void)
{
}

void rgb_indicator_prepare_sleep(void)
{
    #ifdef INDICATOR_SHDN_PIN
        gpio_set_output_pushpull(INDICATOR_SHDN_PIN);
        gpio_write_pin(INDICATOR_SHDN_PIN, 0);
        wait_ms(1);
    #endif
}

void rgb_indicator_flush(void)
{
}

void rgb_indicator_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param)
{
    for (int i = 0;i < RGB_INDICATOR_NUM; i++) {
        if (index == g_rgb_indicators[i].index) {
            *hue = s_indicators[i].led.hue;
            *sat = s_indicators[i].led.sat;
            *val = s_indicators[i].led.val;
            *param = s_indicators[i].led.param;
            break;
        }
    }
}

void rgb_indicator_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param)
{
    struct amk_led* led = NULL;
    for (int i = 0;i < RGB_INDICATOR_NUM; i++) {
        if (index == g_rgb_indicators[i].index) {
            led = &s_indicators[i].led;
            break;
        }
    }

    if (led) {
        if ((led->hue != hue) || (led->sat != sat) || (led->val != val) || (led->param != param)) {
            led->hue = hue;
            led->sat = sat;
            led->val = val;
            led->param = param;
            amk_store_set_led(index, led);
            s_force_update = true;
        }
    }
}
