/**
 * @file hhkbble.c
 */

#include <stdbool.h>

#include "config.h"
#include "gpio_pin.h"
#include "wait.h"
#include "aw9523b.h"

void keyboard_set_rgb(bool on)
{
    if (on) {
        gpio_write_pin(RGBLIGHT_EN_PIN, 1);
        wait_ms(1);
        aw9523b_init(AW9523B_ADDR);
    } else {
        aw9523b_uninit(AW9523B_ADDR);
        gpio_write_pin(RGBLIGHT_EN_PIN, 0);
    }
}

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    keyboard_set_rgb(false);
}