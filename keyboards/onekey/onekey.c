/*
 * onekey.c
 */

#include "onekey.h"
#include "rgb_common.h"
#include "rgb_driver.h"
#include "rgb_bar.h"
#include "is31fl3236.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},

    {1, OUT_36, OUT_35, OUT_34},
    {1, OUT_33, OUT_32, OUT_31},
    {1, OUT_30, OUT_29, OUT_28},
    {1, OUT_27, OUT_26, OUT_25},
    {1, OUT_24, OUT_23, OUT_22},
    {1, OUT_21, OUT_20, OUT_19},
    {1, OUT_18, OUT_17, OUT_16},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    { RGB_DRIVER_WS2812, 0, 0, 0, 4},
    { RGB_DRIVER_IS31FL3236, 0x78, 0, 4, 7},
};

rgb_bar_param_t g_rgb_bar_params[RGB_SEGMENT_NUM] = {
    {0, 0, 0, 4},
    {1, 1, 4, 7},
};