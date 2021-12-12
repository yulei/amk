/**
 * @file falcon_ec.c
 */

#include "falcon_ec.h"
#include "led.h"
#include "amk_gpio.h"
#include "amk_printf.h"

void led_set(uint8_t led)
{
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        amk_printf("turn caps on\n");
        gpio_write_pin(CAPS_LED_PIN, 1);
    } else {
        amk_printf("turn caps off\n");
        gpio_write_pin(CAPS_LED_PIN, 0);
    }
}

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
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
#endif
