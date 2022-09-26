/*
 * @file meta.c
 */

#include "meta.h"
#include "led.h"
#include "rgb_common.h"
#include "rgb_driver.h"
#include "rgb_indicator.h"

#ifdef RGB_ENABLE
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0,0,0,0},
    // 16 leds
    {0,1,1,1},
    {0,2,2,2},
    {0,3,3,3},
    {0,4,4,4},
    {0,5,5,5},
    {0,6,6,6},
    {0,7,7,7},
    {0,8,8,8},
    {0,9,9,9},
    {0,10,10,10},
    {0,11,11,11},
    {0,12,12,12},
    {0,13,13,13},
    {0,14,14,14},
    {0,15,15,15},
    {0,16,16,16},
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812,     0, 0, 0, 17},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0,  1, 16},
};

uint8_t g_rgb_indicator_index[RGB_INDICATOR_LED_NUM] = { 0};

#define CAPS_LED    0
#endif

void indicator_led_set(uint8_t led)
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
