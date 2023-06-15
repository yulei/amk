/**
 * @file 8xv3_ec.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "8xv3_ec.h"

#ifdef RGB_ENABLE
#include "rgb_common.h"
#include "is31fl3236.h"
#include "ws2812.h"
#include "rgb_driver.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // 6 leds
    {0, OUT_24, OUT_23, OUT_22},
    {0, OUT_21, OUT_20, OUT_19},
    {0, OUT_18, OUT_17, OUT_16},
    {0, OUT_15, OUT_14, OUT_13},
    {0, OUT_12, OUT_11, OUT_10},
    {0,  OUT_9,  OUT_8,  OUT_7},

    // 7 leds
    {1,0,0,0},
    {1,1,1,1},
    {1,2,2,2},
    {1,3,3,3},
    {1,4,4,4},
    {1,5,5,5},
    {1,6,6,6},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 6},
    {RGB_DRIVER_WS2812,     0, 0, 6, 7},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 3},
    {1, 3, 3},
    {2, 6, 7},
};

#endif