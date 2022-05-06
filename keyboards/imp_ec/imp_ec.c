/**
 * @file imp_ec.c
 */

#include "imp_ec.h"
#include "led.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
#include "is31fl3236.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
    {0, 4, 4, 4},
    {0, 5, 5, 5},
    {0, 6, 6, 6},
    {0, 7, 7, 7},
    {0, 8, 8, 8},
    {0, 9, 9, 9},
    {0,10,10,10},
    {0,11,11,11},
    {0,12,12,12},
    {0,13,13,13},
    {0,14,14,14},
    {0,15,15,15},
    {1, OUT_34, OUT_35, OUT_36},
    {1, OUT_31, OUT_32, OUT_33},
    {1, OUT_28, OUT_29, OUT_30},
    {1, OUT_25, OUT_26, OUT_27},
    {1, OUT_22, OUT_23, OUT_24},
    {1, OUT_19, OUT_20, OUT_21},
    {1, OUT_16, OUT_17, OUT_18},
    {1, OUT_13, OUT_14, OUT_15},
    {1, OUT_10, OUT_11, OUT_12},
    {1, OUT_7, OUT_8, OUT_9},
    {1, OUT_4, OUT_5, OUT_6},
    {1, OUT_1, OUT_2, OUT_3},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 0, 16},
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 16, 12},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 16},
    {1,16, 12},
};
#endif
