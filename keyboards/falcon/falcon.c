/**
 * @file falcon.c
 */

#include <stdbool.h>

#include "led.h"
#include "rgb_led.h"
#include "rgb_linear.h"
#include "aw9523b.h"
#include "amk_gpio.h"
#include "amk_printf.h"

static rgb_led_t ws2812_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, AW_P12, AW_P11, AW_P10},
    {0, AW_P01, AW_P00, AW_P13},
    {0, AW_P04, AW_P03, AW_P02},
    {0, AW_P07, AW_P06, AW_P05},
};

static rgb_device_t ws2812_device = {RGB_DRIVER_WS2812, 0, 0, 0, 4};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_AW9523B, 0xB6, 0, 0, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 4},
};

void rgb_led_init_pre(void)
{
    if (!aw9523b_available(0xB6)) {
        for (int i = 0; i < RGB_LED_NUM; i++) {
            g_rgb_leds[i].driver = ws2812_leds[i].driver;
            g_rgb_leds[i].r = ws2812_leds[i].r;
            g_rgb_leds[i].g = ws2812_leds[i].g;
            g_rgb_leds[i].b = ws2812_leds[i].b;
        }
        g_rgb_devices[0].type = ws2812_device.type;
        g_rgb_devices[0].addr = ws2812_device.addr;
        g_rgb_devices[0].index = ws2812_device.index;
        g_rgb_devices[0].led_start = ws2812_device.led_start;
        g_rgb_devices[0].led_num = ws2812_device.led_num;
    } 
}

void matrix_init_kb(void)
{
    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, 0);
}

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    rgb_led_prepare_sleep();

    // turn off caps
    gpio_write_pin(CAPS_LED_PIN, 0);
    //gpio_set_input_floating(CAPS_LED_PIN);
}

const rgb_led_t g_aw9523b_leds[RGB_LED_NUM] = {
};

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