/**
 * @file amk_process_keycode.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "quantum.h"
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

#if !defined(NRF52840_XXAA) && !defined(NRF52)
#include "cm_misc.h"
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
            if (rgb_led_config_is_global()) {
                rgb_led_config_set_global(false);
            } else {
                rgb_led_config_next();
            }
            return false; 
        case KC_F14:
            if (!rgb_led_config_is_global()) {
                rgb_led_config_set_global(true);
            }
            return false; 
        case RGB_TOG:
            rgb_led_config_toggle();
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

#ifdef AMK_APC_ENABLE
#include "amk_apc.h"
__attribute__((weak))
void update_apcrt_profile_kb(void) {}

static void update_apcrt_profile(uint8_t profile)
{
    uint8_t config = amk_get_ms_config();
    uint8_t old = (config&AMK_PROFILE_MASK) >> AMK_PROFILE_OFFSET;
    if (old != profile) {
        config = (config & ~AMK_PROFILE_MASK) | ((profile&AMK_PROFILE_MASK) << AMK_PROFILE_OFFSET);
        amk_set_ms_config(config);
        update_apcrt_profile_kb()
    }
}

static bool process_apcrt(uint16_t keycode, const keyrecord_t *record)
{
    switch(keycode) {
        case PROFILE_0 ... PROFILE_3:
            if (record->event.pressed) {
                update_apcrt_profile(keycode - PROFILE_0);
            }
            return false;
        default:
            break;
    }

    return true;
}
#endif

#ifdef AMK_DKS_ENABLE
#include "amk_apc.h"
static void toggle_dks(void)
{
    uint8_t config = amk_get_ms_config();
    uint8_t new_state = ((config&AMK_DKS_MASK) >> AMK_DKS_OFFSET) > 0 ? 0 : 1;
    config = (config & ~AMK_DKS_MASK) | ((new_state&AMK_DKS_MASK) << AMK_DKS_OFFSET);
    amk_set_ms_config(config);
}

static bool process_dks(uint16_t keycode, const keyrecord_t *record)
{
    switch(keycode) {
        case DKS_TOGGLE:
            if (record->event.pressed) {
                toggle_dks();
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

#if defined(AMK_APC_ENABLE)
    if (!process_apcrt(keycode, record)) {
        return false;
    }
#endif

#if defined(AMK_DKS_ENABLE)
    if (!process_dks(keycode, record)) {
        return false;
    }
#endif

    process_amk_keycode(keycode, record);

    return true;
}
