/**
 * @file hhkbble.c
 */

#include <stdbool.h>

#include "gpio_pin.h"
#include "rgb_effects.h"
#include "aw9523b.h"
#include "amk_printf.h"
#include "led.h"

extern void keyboard_set_rgb(bool on);

void matrix_init_kb(void)
{
    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, 0);
}

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    keyboard_set_rgb(false);
    // turn off caps
    gpio_write_pin(CAPS_LED_PIN, 0);
    gpio_set_input_floating(CAPS_LED_PIN);
}

const rgb_led_t g_aw9523b_leds[RGB_LED_NUM] = {
    {AW9523B_P12_PWM, AW9523B_P11_PWM, AW9523B_P10_PWM},
    {AW9523B_P01_PWM, AW9523B_P00_PWM, AW9523B_P13_PWM},
    {AW9523B_P04_PWM, AW9523B_P03_PWM, AW9523B_P02_PWM},
    {AW9523B_P07_PWM, AW9523B_P06_PWM, AW9523B_P05_PWM},
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