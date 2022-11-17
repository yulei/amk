/*
 * m65.c
 */

#include "m65.h"
#include "led.h"

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
    {0, 4, 4, 4},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 1, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 1, 4},
};
#endif

#ifdef RGB_INDICATOR_ENABLE
#include "led.h"
#include "rgb_indicator.h"
uint8_t g_rgb_indicator_index[RGB_INDICATOR_LED_NUM] = {0};
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