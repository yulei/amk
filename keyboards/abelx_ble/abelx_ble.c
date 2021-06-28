/*
 * @file abelx_ble.c
 */

#include "abelx_ble.h"
#include "rgb_linear.h"
#include "aw9523b.h"
#include "rgb_driver.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, AW_P12, AW_P11, AW_P10},
    {0, AW_P01, AW_P00, AW_P13},
    {0, AW_P04, AW_P03, AW_P02},
    {0, AW_P07, AW_P06, AW_P05},

    {1,0,0,0},
    {1,1,1,1},
    {1,2,2,2},
    {1,3,3,3},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    { RGB_DRIVER_AW9523B, 0xB6, 0, 0, 4},
    { RGB_DRIVER_WS2812, 0, 0, 4, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 4},
    {1, 4, 4},
};
