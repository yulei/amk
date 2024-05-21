/**
 * @file falcon_8K.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "falcon_8k.h"
#include "amk_printf.h"

#include "ws2812.h"
#include "rgb_led.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 0, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 4},
};
