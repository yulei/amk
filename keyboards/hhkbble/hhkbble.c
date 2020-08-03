/**
 * @file hhkbble.c
 */

#include <stdbool.h>

#include "gpio_pin.h"
#include "rgb_effects.h"
#include "aw9523b.h"
#include "rtt.h"

void keyboard_set_rgb(bool on)
{
    rtt_printf("keyboard_set_rgb: %d\n", on);
    if (on) {
        if (!rgb_effects_enabled()) {
            rgb_effects_toggle();
        }
    } else {
        if (rgb_effects_enabled()) {
            rgb_effects_toggle();
        }
    }
}

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    keyboard_set_rgb(false);
}

const aw9523b_led g_aw9523b_leds[AW9523B_RGB_NUM] = {
    {AW9523B_P12_PWM, AW9523B_P11_PWM, AW9523B_P10_PWM},
    {AW9523B_P01_PWM, AW9523B_P00_PWM, AW9523B_P13_PWM},
    {AW9523B_P04_PWM, AW9523B_P03_PWM, AW9523B_P02_PWM},
    {AW9523B_P07_PWM, AW9523B_P06_PWM, AW9523B_P05_PWM},
};