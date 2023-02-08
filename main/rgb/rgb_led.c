/**
 * @file rgb_led.c
 */

#include "rgb_led.h"
#include "rgb_driver.h"
#ifdef RGB_LINEAR_ENABLE
#include "rgb_linear.h"
#include "rgb_effect_linear.h"
#endif
#ifdef RGB_INDICATOR_ENABLE
#include "rgb_indicator.h"
#endif
#ifdef RGB_MATRIX_ENABLE
#include "rgb_matrix.h"
//#include "rgb_effect_matrix.h"
#include "rgb_effect_matrix_qmk.h"
#endif
#include "wait.h"
#include "amk_eeprom.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#include "qmk/lib8tion/lib8tion.h"

#ifndef RGB_LED_DEBUG
#define RGB_LED_DEBUG 1
#endif

#if RGB_LED_DEBUG
#define rgb_led_debug  amk_printf
#else
#define rgb_led_debug(...)
#endif

#ifdef RGB_LINEAR_ENABLE
    #define RGB_LINEAR_CONFIG_NUM       RGB_SEGMENT_NUM 
#else
    #define RGB_LINEAR_CONFIG_NUM       0
#endif

#ifdef RGB_INDICATOR_ENABLE
    #define RGB_INDICATOR_CONFIG_NUM    0 //RGB_INDICATOR_LED_NUM
#else
    #define RGB_INDICATOR_CONFIG_NUM    0
#endif

#ifdef RGB_MATRIX_ENABLE
    #define RGB_MATRIX_CONFIG_NUM       RGB_MATRIX_NUM
#else
    #define RGB_MATRIX_CONFIG_NUM       0
#endif

#ifndef RGB_CONFIG_NUM
#define RGB_LED_CONFIG_NUM              (RGB_LINEAR_CONFIG_NUM+RGB_INDICATOR_CONFIG_NUM+RGB_MATRIX_CONFIG_NUM)
#else
#define RGB_LED_CONFIG_NUM              RGB_CONFIG_NUM
#endif

rgb_config_t g_rgb_configs[RGB_LED_CONFIG_NUM];
static uint8_t rgb_config_cur;

static void rgb_led_set_power(bool on)
{
    if (on) {
#ifdef INDICATOR_SHDN_PIN 
        gpio_set_output_pushpull(INDICATOR_SHDN_PIN);
        gpio_write_pin(INDICATOR_SHDN_PIN, 1);
        wait_ms(1);
#endif

#ifdef RGBLIGHT_EN_PIN
        gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
        gpio_write_pin(RGBLIGHT_EN_PIN, 1);
#endif

    } else {
#ifdef RGBLIGHT_EN_PIN
        gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
        gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
    }
}

__attribute__((weak))
void rgb_led_init_pre(void)
{}

void rgb_led_init(void)
{ 
    rgb_led_init_pre();

    rgb_config_cur = 0;
    rgb_led_set_power(true);

    rgb_driver_init();

#ifdef RGB_LINEAR_ENABLE
    rgb_linear_init();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_init();
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_init();
#endif
}

__attribute__((weak))
void rgb_led_pre_flush(void)
{}

void rgb_led_task(void)
{
#ifdef RGB_LINEAR_ENABLE
    rgb_linear_task();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_task();
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_task();
#endif

#ifdef RGBLIGHT_EN_PIN
    if (rgb_led_is_on()) {
#endif
        rgb_led_pre_flush();
        for (int i = 0; i < RGB_DEVICE_NUM; i++) {
            rgb_driver_t *driver = rgb_driver_get(i);
            driver->flush(driver);
        }
#ifdef RGBLIGHT_EN_PIN
    } else {
        rgb_led_set_power(false);
    }
#endif
}

rgb_driver_t *rgb_led_map(uint8_t led_index)
{
    uint8_t driver = g_rgb_leds[led_index].driver;

    return rgb_driver_get(driver);
}

void rgb_led_config_init(void)
{
    rgb_led_debug("rgb_led_config_init()\n");
    for (uint8_t i = 0; i < RGB_LED_CONFIG_NUM; i++) {
        g_rgb_configs[i].enable = ENABLE_DEFAULT;
        g_rgb_configs[i].mode = MODE_DEFAULT;
        g_rgb_configs[i].hue = HUE_DEFAULT;
        g_rgb_configs[i].sat = SAT_DEFAULT;
        g_rgb_configs[i].val = VAL_DEFAULT;
        g_rgb_configs[i].speed = SPEED_DEFAULT;
        g_rgb_configs[i].index = i;
        eeconfig_write_rgb(&g_rgb_configs[i], i);
    }
}

void rgb_led_config_next(void)
{
    rgb_config_cur = (rgb_config_cur+1) % RGB_LED_CONFIG_NUM;
    rgb_led_debug("rgb config current: %d\n", rgb_config_cur);
}

bool rgb_led_config_enabled(void)
{
    return g_rgb_configs[rgb_config_cur].enable;
}

void rgb_led_config_toggle(void)
{
    bool all_off = !rgb_led_is_on();

    g_rgb_configs[rgb_config_cur].enable = !g_rgb_configs[rgb_config_cur].enable;
    eeconfig_update_rgb(&g_rgb_configs[rgb_config_cur], rgb_config_cur);

    if (all_off) {
        // all off to on, need re-init driver and turn led power
        rgb_led_set_power(true);
#ifdef  RGBLIGHT_EN_PIN
        rgb_driver_init();
#endif
    }

    if (!rgb_led_is_on()) {
        // turn off all leds
#ifdef  RGBLIGHT_EN_PIN
        rgb_driver_uninit();
#endif
        rgb_led_set_power(false);
    }
}

static void rgb_led_effect_init_mode(rgb_config_t *config)
{
    switch(config->type) {
#ifdef RGB_LINEAR_ENABLE
        case RGB_EFFECT_LINEAR:
            rgb_effect_linear_init_mode(config->data);
        break;
#endif
#ifdef RGB_INDICATOR_ENABLE
        case RGB_EFFECT_INDICATOR:
        break;
#endif
#ifdef RGB_MATRIX_ENABLE
        case RGB_EFFECT_MATRIX:
            rgb_effect_matrix_init_mode(config->data);
        break;
#endif
        default:
        break;
    }
}

static uint8_t rgb_led_effect_max(rgb_config_t *config)
{
    switch(config->type) {
#ifdef RGB_LINEAR_ENABLE
        case RGB_EFFECT_LINEAR:
            return RL_EFFECT_MAX; 
#endif
#ifdef RGB_INDICATOR_ENABLE
        case RGB_EFFECT_INDICATOR:
        break;
#endif
#ifdef RGB_MATRIX_ENABLE
        case RGB_EFFECT_MATRIX:
            return RM_EFFECT_MAX;
#endif
        default:
        break;
    }

    return 0;
}

static void rgb_led_config_set_param(uint8_t param, uint8_t value)
{
    bool update = false;
    switch(param) {
        case RGB_EFFECT_MODE: {
            if (g_rgb_configs[rgb_config_cur].mode != value) {
                g_rgb_configs[rgb_config_cur].mode = value;
                rgb_led_effect_init_mode(&g_rgb_configs[rgb_config_cur]);
                update = true;
            }
        } break;
        case RGB_EFFECT_SPEED: {
            if (g_rgb_configs[rgb_config_cur].speed != value) {
                g_rgb_configs[rgb_config_cur].speed = value;
                update = true;
            }
        } break;
        case RGB_EFFECT_HUE: {
            if (g_rgb_configs[rgb_config_cur].hue != value) {
                g_rgb_configs[rgb_config_cur].hue = value;
                update = true;
            }
        } break;
        case RGB_EFFECT_SAT: {
            if (g_rgb_configs[rgb_config_cur].sat != value) {
                g_rgb_configs[rgb_config_cur].sat = value;
                update = true;
            }
        } break;
        case RGB_EFFECT_VAL: {
            if (g_rgb_configs[rgb_config_cur].val != value) {
                g_rgb_configs[rgb_config_cur].val = value;
                update = true;
            }
        } break;
        default:
            break;
    }

    if (update) {
        eeconfig_update_rgb(&g_rgb_configs[rgb_config_cur], rgb_config_cur);
    }
}

void rgb_led_config_inc_param(uint8_t param)
{
    uint8_t value;
    switch(param) {
        case RGB_EFFECT_MODE:
            value = (g_rgb_configs[rgb_config_cur].mode+1) % rgb_led_effect_max(&g_rgb_configs[rgb_config_cur]);
            rgb_led_config_set_param(RGB_EFFECT_MODE, value);
            break;
        case RGB_EFFECT_SPEED:
            value = qadd8(g_rgb_configs[rgb_config_cur].speed, SPEED_STEP);
            rgb_led_config_set_param(RGB_EFFECT_SPEED, value);
            break;
        case RGB_EFFECT_HUE:
            value = g_rgb_configs[rgb_config_cur].hue + HUE_STEP;
            rgb_led_config_set_param(RGB_EFFECT_HUE, value);
            break;
        case RGB_EFFECT_SAT:
            value = qadd8(g_rgb_configs[rgb_config_cur].sat, SAT_STEP);
            rgb_led_config_set_param(RGB_EFFECT_SAT, value);
            break;
        case RGB_EFFECT_VAL:
            value = qadd8(g_rgb_configs[rgb_config_cur].val, VAL_STEP);
            rgb_led_config_set_param(RGB_EFFECT_VAL, value);
            break;
        default:
            break;
    }
}

void rgb_led_config_dec_param(uint8_t param)
{
    uint8_t value;
    switch(param) {
        case RGB_EFFECT_MODE:
            value = g_rgb_configs[rgb_config_cur].mode > 0 ? g_rgb_configs[rgb_config_cur].mode - 1 : 0;
            rgb_led_config_set_param(RGB_EFFECT_MODE, value);
            break;
        case RGB_EFFECT_SPEED:
            value = qsub8(g_rgb_configs[rgb_config_cur].speed, SPEED_STEP);
            if (value < SPEED_MIN) value = SPEED_MIN;
            rgb_led_config_set_param(RGB_EFFECT_SPEED, value);
            break;
        case RGB_EFFECT_HUE:
            value = g_rgb_configs[rgb_config_cur].hue - HUE_STEP;
            rgb_led_config_set_param(RGB_EFFECT_HUE, value);
            break;
        case RGB_EFFECT_SAT:
            value = qsub8(g_rgb_configs[rgb_config_cur].sat, SAT_STEP);
            if(value < SAT_MIN) value = SAT_MIN;
            rgb_led_config_set_param(RGB_EFFECT_SAT, value);
            break;
        case RGB_EFFECT_VAL:
            value = qsub8(g_rgb_configs[rgb_config_cur].val, VAL_STEP);
            if(value < VAL_MIN) value = VAL_MIN;
            rgb_led_config_set_param(RGB_EFFECT_VAL, value);
            break;
        default:
            break;
    }
}

bool rgb_led_is_on(void)
{
    uint8_t on = 0;
    for (uint8_t i = 0; i < RGB_LED_CONFIG_NUM; i++) {
        on |= g_rgb_configs[i].enable;
    }

    return on ? true : false;
}

void rgb_led_set_all(bool on)
{
    for (uint8_t i = 0; i < RGB_LED_CONFIG_NUM; i++) {
        g_rgb_configs[i].enable = on ? 1 : 0;
    }

    if (!on) {
        // shutdown led
#ifdef RGBLIGHT_EN_PIN
        gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
        gpio_write_pin(RGBLIGHT_EN_PIN, 0);
#endif
    }
}

void rgb_led_prepare_sleep(void)
{
#ifdef RGB_LINEAR_ENABLE
    rgb_linear_prepare_sleep();
#endif
#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_prepare_sleep();
#endif
#ifdef RGB_MATRIX_ENABLE
#endif

    // shutdown drivers
    rgb_driver_prepare_sleep();
    // led power off
    rgb_led_set_power(false);
}