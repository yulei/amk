/**
 * @file corsa.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "corsa.h"
#include "led.h"
#include "is31fl3236.h"
#include "rgb_led.h"
#include "rgb_linear.h"

#ifdef RGB_ENABLE

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    // indicator leds
    {0, OUT_16, OUT_17, OUT_18},  // ESC
    {0, OUT_19, OUT_20, OUT_21},  // CAPS
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_IS31FL3236, 0x78, 0, 0, 2},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 2},
};

#define CAPS_LED_INDEX  1
#define ESC_LED_INDEX   0

void rgb_led_pre_flush(void)
{
    uint8_t led = host_keyboard_leds();
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0xFF, 0xFF, 0xFF);
    } else {
        rgb_linear_set_rgb(0, CAPS_LED_INDEX, 0, 0, 0);
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    if(layer_state_is(1)){
        b = 0xFF;
    } 

    if(layer_state_is(2)){
        r = 0xFF;
    } 

    if(layer_state_is(3)){
        g = 0xFF;
    } 
    rgb_linear_set_rgb(0, ESC_LED_INDEX, r, g, b);
}

#endif