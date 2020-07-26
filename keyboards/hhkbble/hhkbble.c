/**
 * @file hhkbble.c
 */

#include "hhkbble.h"

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    {{KC_CAPS}}, // test with KC_CAPS, KC_A, KC_BTLD
};

const action_t fn_actions[] = {};

#if 0
void keyboard_set_rgb(bool on)
{
    if (on) {
        nrf_gpio_pin_set(RGBLIGHT_EN_PIN);
        wait_ms(1);
        aw9523b_init(AW9523B_ADDR);
    } else {
        aw9523b_uninit(AW9523B_ADDR);
        nrf_gpio_pin_clear(RGBLIGHT_EN_PIN);
    }
}

void keyboard_prepare_sleep(void)
{
    // turn off rgb
    keyboard_set_rgb(false);
}

#endif
