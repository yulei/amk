/**
 * @file amk_process_keycode.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "quantum.h"
#include "cm_misc.h"
#include "usb_interface.h"
#include "amk_eeprom.h"
#include "amk_keycode.h"
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

#ifdef USE_HS_USB
static void store_and_reset_usb(uint8_t polling)
{
    eeconfig_update_usb(polling);
    usb_connect(0);
    wait_ms(100);
    NVIC_SystemReset();
}
static bool process_hs_usb(uint16_t keycode, const keyrecord_t *record)
{
    switch(keycode) {
        case POLL_1K:
            if (record->event.pressed) {
                store_and_reset_usb(0);
            }
            return false;
        case POLL_2K:
            if (record->event.pressed) {
                store_and_reset_usb(1);
            }
            return false;
        case POLL_4K:
            if (record->event.pressed) {
                store_and_reset_usb(2);
            }
            return false;
        case POLL_8K:
            if (record->event.pressed) {
                store_and_reset_usb(3);
            }
            return false;
        default:
            break;
    }
    return true;
}
#endif

#ifdef STATE_SCAN_ENABLE
#include "amk_state.h"

static void store_and_update_debounce(uint8_t down, uint8_t up)
{
    uint8_t debounce = ((down&0x0F) << 4) | (up&0x0F);
    eeconfig_update_debounce(debounce);
    state_matrix_update_debounce(down, up);
}

static bool process_state_debounce(uint16_t keycode, const keyrecord_t *record)
{
    switch(keycode) {
        case DOWN_0MS ... DOWN_5MS:
        {
            uint8_t press = keycode - DOWN_0MS;
            if (press != state_matrix_get_debounce(true)) {
                uint8_t release = state_matrix_get_debounce(false);
                store_and_update_debounce(press, release);
            }
        }
        return false;
        case UP_0MS ... UP_5MS:
        {
            uint8_t release = keycode - UP_0MS;
            if (release != state_matrix_get_debounce(false)) {
                uint8_t press = state_matrix_get_debounce(true);
                store_and_update_debounce(press, release);
            }
        }
        return false;
        default:
            break;
    }

    return true;
}

#endif
static bool process_amk_keycode(uint16_t keycode, const keyrecord_t *record)
{
    switch(keycode) {
        default:
            break;
    }
    return true;
}

bool process_action_kb(keyrecord_t *record)
{
    uint16_t keycode = get_record_keycode(record, true);
#if defined(RGB_ENABLE)
    if (!process_rgb_amk(keycode, record)) {
        return false;
    }
#endif

#if defined(USE_HS_USB)
    if (!process_hs_usb(keycode, record)) {
        return false;
    }
#endif

#ifdef STATE_SCAN_ENABLE
    if (!process_state_debounce(keycode, record)) {
        return false;
    }
#endif

    process_amk_keycode(keycode, record);

    return true;
}
