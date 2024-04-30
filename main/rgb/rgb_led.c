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
#include "rgb_matrix_stub.h"
#endif
#include "wait.h"
#include "timer.h"
#include "amk_eeprom.h"
#include "amk_gpio.h"
#include "amk_printf.h"
#include "amk_store.h"

#include "lib8tion.h"

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
    #define RGB_INDICATOR_CONFIG_NUM    0 //RGB_INDICATOR_NUM
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

// global led cache
struct amk_led amk_leds[RGB_LED_NUM];

rgb_cfg_t g_rgb_configs[RGB_LED_CONFIG_NUM];
static uint8_t rgb_config_cur;
static bool rgb_global = true;

static bool rgb_is_matrix(void)
{
#ifdef RGB_MATRIX_ENABLE
    return g_rgb_configs[rgb_config_cur].type == RGB_EFFECT_MATRIX;
#else
    return false;
#endif
}

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


#ifdef RGB_INIT_DELAY
#include "timer.h"
static bool delay_init = true;
static bool inited= false;
static uint32_t last_ticks = 0;
#endif

void rgb_led_init(void)
{ 
#ifdef RGB_INIT_DELAY
    if (delay_init) {
        last_ticks = timer_read32();
        return;
    }
#endif
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
    rgb_matrix_init_stub();
#endif
}

__attribute__((weak))
void rgb_led_pre_flush(void)
{}


#ifndef RGB_UPDATE_INTERVAL
#define RGB_UPDATE_INTERVAL     0
#endif

//#define RGB_PRINT_TASK_TICKS
void rgb_led_task(void)
{
    static uint32_t rgb_ticks = 0;
    if (timer_elapsed32(rgb_ticks) < RGB_UPDATE_INTERVAL) {
        return;
    }
    rgb_ticks = timer_read32();

#ifdef RGB_INIT_DELAY
    if (!inited) {
        if (timer_elapsed32(last_ticks) > RGB_INIT_DELAY) {
            delay_init = false;
            rgb_led_init();
            inited = true;
        }
        return;
    } 
#endif

#ifdef RGB_PRINT_TASK_TICKS
    uint32_t ticks = timer_read32();
#endif
#ifdef RGB_LINEAR_ENABLE
    rgb_linear_task();
#ifdef RGB_PRINT_TASK_TICKS
    rgb_led_debug("RGB: Linear Task: from %d to %d\n", ticks, timer_read32());
#endif
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_task_stub();
#endif

#ifdef RGB_INDICATOR_ENABLE
    rgb_indicator_task();
#endif

#ifdef RGBLIGHT_EN_PIN
    if (rgb_led_is_on()) {
#endif
        rgb_led_pre_flush();
#ifdef RGB_PRINT_TASK_TICKS
        ticks = timer_read32();
#endif
        static int last_index = 0;
        rgb_driver_t *driver = rgb_driver_get(last_index);
        if (driver->flush(driver)) {
            last_index = (last_index+1) % RGB_DEVICE_NUM;
        }

#ifdef RGB_PRINT_TASK_TICKS
        rgb_led_debug("RGB: Flush: from %d to %d\n", ticks, timer_read32());
#endif
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
    rgb_led_debug("rgb led config init\n");
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
#ifdef RGB_MATRIX_ENABLE
    if (rgb_is_matrix()) {
        return rgb_matrix_is_enabled() != 0;

    } else
#endif
    {
        return g_rgb_configs[rgb_config_cur].enable;
    }
}

void rgb_led_config_toggle_temp(void)
{
    bool all_off = !rgb_led_is_on();

#ifdef RGB_MATRIX_ENABLE
    if (rgb_is_matrix()) {
        rgb_matrix_toggle();
    } else
#endif
    {
        g_rgb_configs[rgb_config_cur].enable = !g_rgb_configs[rgb_config_cur].enable;
    }

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

bool rgb_led_config_is_global(void)
{
    return rgb_global;
}

void rgb_led_config_set_global(bool on)
{
    rgb_global = on;
}

void rgb_led_config_toggle(void)
{
    if (rgb_led_config_is_global()) {
        int temp = rgb_config_cur;
        for (int i = 0; i < RGB_LED_CONFIG_NUM; i++) {
            rgb_config_cur = i;
            rgb_led_config_toggle_temp();
            eeconfig_update_rgb(&g_rgb_configs[rgb_config_cur], rgb_config_cur);
        }
        rgb_config_cur = temp;
    } else {
        rgb_led_config_toggle_temp();
        eeconfig_update_rgb(&g_rgb_configs[rgb_config_cur], rgb_config_cur);
    }
}

static void rgb_led_effect_init_mode(rgb_cfg_t *config)
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
            //rgb_effect_matrix_init_mode(config->data);
        break;
#endif
        default:
        break;
    }
}

static uint8_t rgb_led_effect_max(rgb_cfg_t *config)
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
            return RGB_MATRIX_EFFECT_MAX;
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

static void inc_param_index(uint8_t param, uint32_t index)
{
    uint32_t temp = rgb_config_cur;
    rgb_config_cur = index;
#ifdef RGB_MATRIX_ENABLE
    if (rgb_is_matrix()) {
            switch(param) {
            case RGB_EFFECT_MODE:
                rgb_matrix_step_stub();
                break;
            case RGB_EFFECT_SPEED:
                rgb_matrix_increase_speed();
                break;
            case RGB_EFFECT_HUE:
                rgb_matrix_increase_hue();
                break;
            case RGB_EFFECT_SAT:
                rgb_matrix_increase_sat();
                break;
            case RGB_EFFECT_VAL:
                rgb_matrix_increase_val();
                break;
            default:
                break;
        }
    } else
#endif
    {
        uint8_t value;
        switch(param) {
            case RGB_EFFECT_MODE:
                value = (g_rgb_configs[rgb_config_cur].mode+1) % rgb_led_effect_max(&g_rgb_configs[rgb_config_cur]);
                if (value == 0) { ++value;}
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
                if (value > VAL_MAX) value = VAL_MAX;
                rgb_led_config_set_param(RGB_EFFECT_VAL, value);
                break;
            default:
                break;
        }
    }
    rgb_config_cur = temp;
}

void rgb_led_config_inc_param(uint8_t param)
{
    if (rgb_led_config_is_global()){
        for (int i = 0; i < RGB_LED_CONFIG_NUM; i++) {
            inc_param_index(param, i);
        }
    } else {
        inc_param_index(param, rgb_config_cur);
    }
}

static void dec_param_index(uint8_t param, uint32_t index)
{
    uint32_t temp = rgb_config_cur;
    rgb_config_cur = index;
#ifdef RGB_MATRIX_ENABLE
    if (rgb_is_matrix()) {
            switch(param) {
            case RGB_EFFECT_MODE:
                rgb_matrix_step_reverse_stub();
                break;
            case RGB_EFFECT_SPEED:
                rgb_matrix_decrease_speed();
                break;
            case RGB_EFFECT_HUE:
                rgb_matrix_decrease_hue();
                break;
            case RGB_EFFECT_SAT:
                rgb_matrix_decrease_sat();
                break;
            case RGB_EFFECT_VAL:
                rgb_matrix_decrease_val();
                break;
            default:
                break;
        }
    } else
#endif
    {
        uint8_t value;
        switch(param) {
            case RGB_EFFECT_MODE:
                uint8_t mode_max = rgb_led_effect_max(&g_rgb_configs[rgb_config_cur]);
                value = (g_rgb_configs[rgb_config_cur].mode + mode_max - 1) % mode_max;
                if (value == 0) { value = mode_max - 1; }
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
                if (value > VAL_MAX) value = VAL_MAX;
                rgb_led_config_set_param(RGB_EFFECT_VAL, value);
                break;
            default:
                break;
        }
    }
    rgb_config_cur = temp;

}

void rgb_led_config_dec_param(uint8_t param)
{
    if (rgb_led_config_is_global()){
        for (int i = 0; i < RGB_LED_CONFIG_NUM; i++) {
            dec_param_index(param, i);
        }
    } else {
        dec_param_index(param, rgb_config_cur);
    }
}

bool rgb_led_is_on(void)
{
    uint8_t on = 0;
    for (uint8_t i = 0; i < RGB_LED_CONFIG_NUM; i++) {
        if (rgb_is_matrix()){
#ifdef RGB_MATRIX_ENABLE
            on |= rgb_matrix_is_enabled();
#endif
        } else {
            on |= g_rgb_configs[i].enable;
        }
    }

    return on ? true : false;
}

void rgb_led_set_all(bool on)
{
    for (uint8_t i = 0; i < RGB_LED_CONFIG_NUM; i++) {
        if (rgb_is_matrix()){
#ifdef RGB_MATRIX_ENABLE
            if (on) {
                rgb_matrix_enable();
            } else {
                rgb_matrix_disable();
            }
#endif
        } else {
            g_rgb_configs[i].enable = on ? 1 : 0;
        }
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
    rgb_matrix_prepare_sleep_stub();
#endif

    // shutdown drivers
    rgb_driver_prepare_sleep();
    // led power off
    rgb_led_set_power(false);
}


#ifdef RGB_LINEAR_ENABLE
uint8_t rgb_led_strip_get_mode(uint8_t index)
{
    if (index < RGB_LINEAR_CONFIG_NUM) {
        uint8_t config = g_rgb_linear_params[index].config;
        return g_rgb_configs[config].mode;
    }

    return 0;
}

void rgb_led_strip_set_mode(uint8_t index, uint8_t mode)
{
    if (index < RGB_LINEAR_CONFIG_NUM) {
        uint8_t config = g_rgb_linear_params[index].config;
        uint8_t temp = rgb_config_cur;
        rgb_config_cur = config;

        mode %= rgb_led_effect_max(&g_rgb_configs[rgb_config_cur]);
        rgb_led_config_set_param(RGB_EFFECT_MODE, mode);

        rgb_config_cur = temp;
    }
}

void rgb_led_strip_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param)
{
    rgb_linear_get_led(index, hue, sat, val, param);
}

void rgb_led_strip_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param)
{
    rgb_linear_set_led(index, hue, sat, val, param);
}

#else
uint8_t rgb_led_strip_get_mode(uint8_t index) {return 1;}
void rgb_led_strip_set_mode(uint8_t index, uint8_t mode) {}
void rgb_led_strip_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param) {}
void rgb_led_strip_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param) {}
#endif
