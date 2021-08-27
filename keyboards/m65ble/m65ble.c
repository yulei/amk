/**
 * @file m65ble.c
 */

#include "led.h"
#include "action.h"
#include "rgb_led.h"
#include "rgb_linear.h"
#include "rgb_indicator.h"
#include "aw9106b.h"
#include "common_config.h"
#include "usb_interface.h"
#include "amk_gpio.h"
#include "amk_printf.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    // indicator leds
    {1, AW_DIM1, AW_DIM2, AW_DIM0},  // ESC
    {1, AW_DIM4, AW_DIM5, AW_DIM3},  // CAPS
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    { RGB_DRIVER_WS2812, 0, 0, 0, 4},
    { RGB_DRIVER_AW9106B, 0xB6, 0, 4, 2},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    { 0, 0, 4},
};

uint8_t g_rgb_indicator_index[RGB_INDICATOR_LED_NUM] = { 4,5};

#define CAPS_LED    1
#define ESC_LED     0

void matrix_init_kb(void)
{
}

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    rgb_led_prepare_sleep();
}

void led_set(uint8_t led)
{
#ifdef RGB_INDICATOR_ENABLE
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_indicator_set(CAPS_LED, 0xFF, 0xFF, 0xFF);
        amk_printf("turn caps on\n");
    } else {
        rgb_indicator_set(CAPS_LED, 0, 0, 0);
        amk_printf("turn caps off\n");
    }
#endif
}

static void toggle_command(uint8_t cmd)
{
    nrf_usb_send_report(NRF_REPORT_ID_COMMAND, &cmd, 1);
}

bool hook_process_action_kb(action_t *action)
{
    //uint8_t mods = get_mods();
    //if ( !(mods & (MOD_BIT(KC_LSFT))) && !(mods & (MOD_BIT(KC_RSFT))) ){
    //    return false;
    //}

    switch(action->key.code) {
        case KC_F19:
            toggle_command(CMD_TOGGLE_SCREEN);
            return true;
        case KC_F14: 
            toggle_command(CMD_TOGGLE_MSC);
            return true;

    default:
        break;
    }
    return false;
}