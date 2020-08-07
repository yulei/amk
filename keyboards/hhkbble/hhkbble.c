/**
 * @file hhkbble.c
 */

#include <stdbool.h>

#include "gpio_pin.h"
#include "rgb_effects.h"
#include "aw9523b.h"
#include "rtt.h"
#include "led.h"

extern void keyboard_set_rgb(bool on);

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    keyboard_set_rgb(false);
}

const aw9523b_led_t g_aw9523b_leds[AW9523B_LED_NUM] = {
    {AW9523B_P12_PWM, AW9523B_P11_PWM, AW9523B_P10_PWM},
    {AW9523B_P01_PWM, AW9523B_P00_PWM, AW9523B_P13_PWM},
    {AW9523B_P04_PWM, AW9523B_P03_PWM, AW9523B_P02_PWM},
    {AW9523B_P07_PWM, AW9523B_P06_PWM, AW9523B_P05_PWM},
};

void led_set(uint8_t led)
{
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        gpio_set_output_pushpull(CAPS_LED_PIN);
        gpio_write_pin(CAPS_LED_PIN, 1);
    } else {
        gpio_set_output_pushpull(CAPS_LED_PIN);
        gpio_write_pin(CAPS_LED_PIN, 0);
    }
}