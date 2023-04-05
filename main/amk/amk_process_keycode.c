/**
 * @file amk_process_keycode.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "quantum.h"
#include "amk_printf.h"

#ifndef AMK_KEYCODE_DEBUG
#define AMK_KEYCODE_DEBUG 1
#endif

#if AMK_KEYCODE_DEBUG
#define keycode_debug  amk_printf
#else
#define keycode_debug(...)
#endif

#ifdef RGB_ENABLE
#include "rgb_led.h"
__attribute__((weak))
void keyboard_set_rgb(bool on)
{
    keycode_debug("keyboard_set_rgb: %d\n", on);
    if (on) {
        if (!rgb_led_config_enabled()) {
            rgb_led_config_toggle();
        }
    } else {
        if (rgb_led_config_enabled()) {
            rgb_led_config_toggle();
        }
    }
}

static bool process_rgb_amk(uint16_t keycode, const keyrecord_t *record)
{
    // only handle key down
    if (record->event.pressed) {
        uint8_t shifted = get_mods() & MOD_MASK_SHIFT;
        switch (keycode) {
        case KC_F13:
           rgb_led_config_next();
           return false; 
        case RGB_TOG:
            keyboard_set_rgb(!rgb_led_config_enabled());
            keycode_debug("Toggle rgb: %d\n", rgb_led_config_enabled());
            return false;
        case RGB_MODE_FORWARD:
            if (shifted) {
                rgb_led_config_dec_param(RGB_EFFECT_MODE);
            } else {
                rgb_led_config_inc_param(RGB_EFFECT_MODE);
            }
            return false;
        case RGB_MODE_REVERSE:
            if (shifted) {
                rgb_led_config_inc_param(RGB_EFFECT_MODE);
            } else {
                rgb_led_config_dec_param(RGB_EFFECT_MODE);
            }
            return false;
        case RGB_HUI:
            if (shifted) {
                rgb_led_config_dec_param(RGB_EFFECT_HUE);
            } else {
                rgb_led_config_inc_param(RGB_EFFECT_HUE);
            }
            return false;
        case RGB_HUD:
            if (shifted) {
                rgb_led_config_inc_param(RGB_EFFECT_HUE);
            } else {
                rgb_led_config_dec_param(RGB_EFFECT_HUE);
            }
            return false;
        case RGB_SAI:
            if (shifted) {
                rgb_led_config_dec_param(RGB_EFFECT_SAT);
            } else {
                rgb_led_config_inc_param(RGB_EFFECT_SAT);
            }
            return false;
        case RGB_SAD:
            if (shifted) {
                rgb_led_config_inc_param(RGB_EFFECT_SAT);
            } else {
                rgb_led_config_dec_param(RGB_EFFECT_SAT);
            }
            return false;
        case RGB_VAI:
            if (shifted) {
                rgb_led_config_dec_param(RGB_EFFECT_VAL);
            } else {
                rgb_led_config_inc_param(RGB_EFFECT_VAL);
            }
            return false;
        case RGB_VAD:
            if (shifted) {
                rgb_led_config_inc_param(RGB_EFFECT_VAL);
            } else {
                rgb_led_config_dec_param(RGB_EFFECT_VAL);
            }
            return false;
        case RGB_SPI:
            if (shifted) {
                rgb_led_config_dec_param(RGB_EFFECT_SPEED);
            } else {
                rgb_led_config_inc_param(RGB_EFFECT_SPEED);
            }
            return false;
        case RGB_SPD:
            if (shifted) {
                rgb_led_config_inc_param(RGB_EFFECT_SPEED);
            } else {
                rgb_led_config_dec_param(RGB_EFFECT_SPEED);
            }
            return false;
        default:
            break;
        }
    }

    return true;
}
#endif

bool process_action_kb(keyrecord_t *record)
{
    uint16_t keycode = get_record_keycode(record, true);
#ifdef RGB_ENABLE
    if (!process_rgb_amk(keycode, record)) {
        return false;
    }
#else
    (void)keycode;
#endif

    return true;
}