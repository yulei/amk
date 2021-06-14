/**
 * @file rgb_led.c
 */

#include "rgb_led.h"
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "rgb_indicator.h"
#include "rgb_matrix.h"
#include "rgb_effect_linear.h"
#include "wait.h"
#include "amk_eeprom.h"
#include "amk_gpio.h"

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

#define RGB_LED_CONFIG_NUM              (RGB_LINEAR_CONFIG_NUM+RGB_INDICATOR_CONFIG_NUM+RGB_MATRIX_CONFIG_NUM)

rgb_config_t g_rgb_configs[RGB_LED_CONFIG_NUM];
static uint8_t rgb_config_cur;

void rgb_led_init(void)
{ 
    rgb_config_cur = 0;

#ifdef AW9106B_SHDN_PIN
    gpio_set_output_pushpull(AW9106B_SHDN_PIN);
    gpio_write_pin(AW9106B_SHDN_PIN, 1);
    wait_ms(1);
#endif

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
    for (int i = 0; i < RGB_DEVICE_NUM; i++) {
        rgb_driver_t *driver = rgb_driver_get(i);
        driver->flush(driver);
    }
}

rgb_driver_t *rgb_led_map(uint8_t led_index)
{
    uint8_t driver = g_rgb_leds[led_index].driver;

    return rgb_driver_get(driver);
}

void rgb_led_config_init(void)
{
    for (uint8_t i = 0; i < RGB_LED_CONFIG_NUM; i++) {
        g_rgb_configs[i].enable = ENABLE_DEFAULT;
        g_rgb_configs[i].mode = MODE_DEFAULT;
        g_rgb_configs[i].hue = HUE_DEFAULT;
        g_rgb_configs[i].sat = SAT_DEFAULT;
        g_rgb_configs[i].val = VAL_DEFAULT;
        g_rgb_configs[i].speed = SPEED_DEFAULT;
        eeconfig_write_rgb(&g_rgb_configs[i], i);
    }
}

void rgb_led_config_next(void)
{
    rgb_config_cur = (rgb_config_cur+1) % RGB_LED_CONFIG_NUM;
}

bool rgb_led_config_enabled(void)
{
    return g_rgb_configs[rgb_config_cur].enable;
}

void rgb_led_config_toggle(void)
{
    g_rgb_configs[rgb_config_cur].enable = !g_rgb_configs[rgb_config_cur].enable;
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
            value = (g_rgb_configs[rgb_config_cur].speed+SPEED_STEP) % SPEED_MAX;
            rgb_led_config_set_param(RGB_EFFECT_SPEED, value);
            break;
        case RGB_EFFECT_HUE:
            value = (g_rgb_configs[rgb_config_cur].hue+HUE_STEP) % HUE_MAX;
            rgb_led_config_set_param(RGB_EFFECT_HUE, value);
            break;
        case RGB_EFFECT_SAT:
            value = (g_rgb_configs[rgb_config_cur].sat+SAT_STEP) % SAT_MAX;
            rgb_led_config_set_param(RGB_EFFECT_SAT, value);
            break;
        case RGB_EFFECT_VAL:
            value = (g_rgb_configs[rgb_config_cur].val+VAL_STEP) % VAL_MAX;
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
            value = g_rgb_configs[rgb_config_cur].speed > SPEED_STEP ? g_rgb_configs[rgb_config_cur].speed - SPEED_STEP : SPEED_MIN;
            rgb_led_config_set_param(RGB_EFFECT_SPEED, value);
            break;
        case RGB_EFFECT_HUE:
            value = g_rgb_configs[rgb_config_cur].hue > HUE_STEP ? g_rgb_configs[rgb_config_cur].hue - HUE_STEP : HUE_MIN;
            rgb_led_config_set_param(RGB_EFFECT_HUE, value);
            break;
        case RGB_EFFECT_SAT:
            value = g_rgb_configs[rgb_config_cur].sat > SAT_STEP ? g_rgb_configs[rgb_config_cur].sat - SAT_STEP : SAT_MIN;
            rgb_led_config_set_param(RGB_EFFECT_SAT, value);
            break;
        case RGB_EFFECT_VAL:
            value = g_rgb_configs[rgb_config_cur].val > VAL_STEP ? g_rgb_configs[rgb_config_cur].val - VAL_STEP : VAL_MIN;
            rgb_led_config_set_param(RGB_EFFECT_VAL, value);
            break;
        default:
            break;
    }
}
