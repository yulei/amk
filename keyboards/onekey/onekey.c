/*
 * onekey.c
 */

#include "onekey.h"
#include "rgb_common.h"
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "is31fl3236.h"
#include "is31fl3731.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
/*
    {1, OUT_36, OUT_35, OUT_34},
    {1, OUT_33, OUT_32, OUT_31},
    {1, OUT_30, OUT_29, OUT_28},
    {1, OUT_27, OUT_26, OUT_25},
    {1, OUT_24, OUT_23, OUT_22},
    {1, OUT_21, OUT_20, OUT_19},
    {1, OUT_18, OUT_17, OUT_16},
*/
    {1, C1_1,   C3_2,   C4_2},
    {1, C1_2,   C2_2,   C4_3},
    {1, C1_3,   C2_3,   C3_3},
    {1, C1_4,   C2_4,   C3_4},
    {1, C1_5,   C2_5,   C3_5},
    {1, C1_6,   C2_6,   C3_6},
    {1, C1_7,   C2_7,   C3_7},
    {1, C1_8,   C2_8,   C3_8},

    {1, C9_1,   C8_1,   C7_1},
    {1, C9_2,   C8_2,   C7_2},
    {1, C9_3,   C8_3,   C7_3},
    {1, C9_4,   C8_4,   C7_4},
    {1, C9_5,   C8_5,   C7_5},
    {1, C9_6,   C8_6,   C7_6},
    {1, C9_7,   C8_7,   C6_6},
    {1, C9_8,   C7_7,   C6_7},

    {1, C1_9,   C3_10,  C4_10},
    {1, C1_10,  C2_10,  C4_11},
    {1, C1_11,  C2_11,  C3_11},
    {1, C1_12,  C2_12,  C3_12},

//    {1,0,0,0},
//    {1,0,0,0},
//    {1,0,0,0},
//    {1,0,0,0},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    { RGB_DRIVER_WS2812, 0, 0, 0, 4},
    { RGB_DRIVER_IS31FL3731, 0xE8, 0, 4, 20},
 //   { RGB_DRIVER_IS31FL3236, 0x78, 0, 4, 7},
};

rgb_linear_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 0, 4},
    {1, 1, 4, 16},
    {1, 2, 20, 4},
};