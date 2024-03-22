/*
 * @file faukwaa.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "faukwaa.h"
#include "rgb_common.h"
#include "is31fl3236.h"
#include "rgb_driver.h"

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // left leds
    {0, OUT_25, OUT_26, OUT_27},
    {0, OUT_13, OUT_14, OUT_15},
    {0, OUT_16, OUT_17, OUT_18},
    {0, OUT_19, OUT_20, OUT_21},
    {0, OUT_22, OUT_23, OUT_24},

    // right leds
    {1, OUT_16, OUT_17, OUT_18},
    {1, OUT_19, OUT_20, OUT_21},
    {1, OUT_10, OUT_11, OUT_12},
    {1, OUT_13, OUT_14, OUT_15},

#if 1
    // middle leds
    {2, OUT_28, OUT_29, OUT_30},
    {2, OUT_4, OUT_5, OUT_6},
    {2, OUT_7, OUT_8, OUT_9},
    {2, OUT_10, OUT_11, OUT_12},

    // bottom leds
    {3, OUT_1, OUT_2, OUT_3},
    {3, OUT_34, OUT_35, OUT_36},
    {3, OUT_4, OUT_5, OUT_6},
    {3, OUT_7, OUT_8, OUT_9},
    {3, OUT_19, OUT_20, OUT_21},
    {3, OUT_16, OUT_17, OUT_18},
    {3, OUT_13, OUT_14, OUT_15},
    {3, OUT_10, OUT_11, OUT_12},
#endif
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 5},
    {RGB_DRIVER_IS31FL3236, 0x7E, 1, 5, 4},
    {RGB_DRIVER_IS31FL3236, 0x7A, 2, 9, 4},
    {RGB_DRIVER_IS31FL3236, 0x7C, 3, 13, 8},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 1, 4},
    {1, 5, 4},
    {2, 9, 4},
    {3,13, 4},
    {4,17, 4},
};

#define CAPS_LED_INDEX 0 

static void set_led_rgb(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_driver_t *driver = rgb_driver_get(g_rgb_leds[index].driver);

    if (driver) {
        driver->set_color_rgb(driver, index, red, green, blue);
    }
}

void rgb_led_pre_flush(void)
{
    if (host_keyboard_led_state().caps_lock) {
        set_led_rgb(CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
    } else {
        set_led_rgb(CAPS_LED_INDEX, 0, 0, 0);
    }
}