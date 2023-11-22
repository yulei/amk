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
#include "is31fl3731.h"
#include "ws2812.h"
#include "rgb_driver.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
#if 0
    // 6 leds
    {0, OUT_24, OUT_23, OUT_22},
    {0, OUT_21, OUT_20, OUT_19},
    {0, OUT_18, OUT_17, OUT_16},
    {0, OUT_15, OUT_14, OUT_13},
    {0, OUT_12, OUT_11, OUT_10},
    {0,  OUT_9,  OUT_8,  OUT_7},
#else
    // 16 leds
    {0, C1_1,   C3_2,   C4_2},
    {0, C1_2,   C2_2,   C4_3},
    {0, C1_3,   C2_3,   C3_3},
    {0, C1_4,   C2_4,   C3_4},
    {0, C1_5,   C2_5,   C3_5},
    {0, C1_6,   C2_6,   C3_6},
    {0, C1_7,   C2_7,   C3_7},
    {0, C1_8,   C2_8,   C3_8},

    {0, C9_1,   C8_1,   C7_1},
    {0, C9_2,   C8_2,   C7_2},
    {0, C9_3,   C8_3,   C7_3},
    {0, C9_4,   C8_4,   C7_4},
    {0, C9_5,   C8_5,   C7_5},
    {0, C9_6,   C8_6,   C7_6},
    {0, C9_7,   C8_7,   C6_6},
    {0, C9_8,   C7_7,   C6_7},
#endif

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
    //{RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 6},
    {RGB_DRIVER_IS31FL3731, 0xE8, 0, 0, 16},
    {RGB_DRIVER_WS2812,     0, 0, 6, 7},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0,0,16},
    {1,16,7},
    //{0, 0, 3},
    //{1, 3, 3},
    //{2, 6, 7},
};

#endif