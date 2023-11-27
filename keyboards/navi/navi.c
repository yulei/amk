/**
 * @file navi.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "navi.h"
#include "is31fl3236.h"
#include "rgb_led.h"
#include "rgb_linear.h"

#ifdef RGB_ENABLE

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, OUT_36, OUT_35, OUT_34},
    {0, OUT_33, OUT_32, OUT_31},
    {0, OUT_30, OUT_29, OUT_28},
//    {0, OUT_27, OUT_26, OUT_25},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 3},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 3},
//    {1, 3, 1},
};

#include "host.h"
#include "led.h"

#define CAPS_LED_INDEX 0
#define SCROLL_LED_INDEX 1

void rgb_led_pre_flush(void)
{
    //if (!rgb_led_is_on()) {
        if (host_keyboard_led_state().caps_lock) {
            rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
        } else {
            if (!rgb_led_is_on()) rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0, 0, 0);
        }
        if (host_keyboard_led_state().scroll_lock) {
            rgb_linear_set_rgb(0, SCROLL_LED_INDEX, 0xFF, 0xFF, 0xFF);
        } else {
            if (!rgb_led_is_on()) rgb_linear_set_rgb(0, SCROLL_LED_INDEX, 0, 0, 0);
        }
    //}
}

#endif