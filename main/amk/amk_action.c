/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_action.c
 * @brief action function defintion for custom behavior
 */
#include "amk_action.h"
#include "eeconfig.h"

#include "action.h"
#include "action_layer.h"
#include "action_util.h"
#include "keymap.h"
#include "amk_macro.h"
#include "amk_keymap.h"
#include "amk_printf.h"
#include "wait.h"

#ifdef RGB_ENABLE
#include "rgb_led.h"
#endif

#ifdef VIAL_ENABLE
#include "vial_macro.h"
#endif

#ifdef RF_ENABLE
#include "rf_driver.h"
#endif

#if defined(NRF52) || defined(NRF52840_XXAA)
#include "common_config.h"
#endif

#ifndef AMK_ACTION_DEBUG
#define AMK_ACTION_DEBUG 1
#endif

#if AMK_ACTION_DEBUG
#define action_debug  amk_printf
#else
#define action_debug(...)
#endif

__attribute__((weak))
void keyboard_set_rgb(bool on)
{
#ifdef RGB_ENABLE
    action_debug("keyboard_set_rgb: %d\n", on);
    if (on) {
        if (!rgb_led_config_enabled()) {
            rgb_led_config_toggle();
        }
    } else {
        if (rgb_led_config_enabled()) {
            rgb_led_config_toggle();
        }
    }
#endif
}

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    if (opt == FUNC_TAP) {
        uint8_t tap_count = record->tap.count;
        uint8_t mod = 0;
        uint8_t key = KC_NO;
        switch (id) {
            case AF_KC_LSPO:
                mod |= (MOD_LSFT>>8);
                key = KC_9;
                break;
            case AF_KC_RSPC:
                mod |= (MOD_RSFT>>8);
                key = KC_0;
                break;
            case AF_KC_LCPO:
                mod |= (MOD_LCTL>>8);
                key = KC_9;
                break;
            case AF_KC_RCPC:
                mod |= (MOD_RCTL>>8);
                key = KC_0;
                break;
            case AF_KC_LAPO:
                mod |= (MOD_LALT>>8);
                key = KC_9;
                break;
            case AF_KC_RAPC:
                mod |= (MOD_RALT>>8);
                key = KC_0;
                break;
            default:
                return;
        };

        if (record->event.pressed) {
            if (tap_count > 0) {
                if (record->tap.interrupted) {
                    action_debug("MODS_TAP: Tap: Cancel: add_mods\n");
                    // ad hoc: set 0 to cancel tap
                    record->tap.count = 0;
                    register_mods(mod);
                } else {
                    action_debug("MODS_TAP: Tap: register_code\n");
                    add_weak_mods(mod);
                    register_code(key);
                }
            } else {
                action_debug("MODS_TAP: No tap: add_mods\n");
                register_mods(mod);
            }
        } else {
            if (tap_count > 0) {
                action_debug("MODS_TAP: Tap: unregister_code\n");
                del_weak_mods(mod);
                unregister_code(key);
            } else {
                action_debug("MODS_TAP: No tap: add_mods\n");
                unregister_mods(mod);
            }
        }
    } else {
        switch (id) {
    #ifdef RGB_ENABLE
            case AF_RGB_TOG:
                if (!record->event.pressed) return;
                keyboard_set_rgb(!rgb_led_config_enabled());
                action_debug("Toggle rgb: %d\n", rgb_led_config_enabled());
                break;
            case AF_RGB_MOD:
                if (!record->event.pressed) return;
                rgb_led_config_inc_param(RGB_EFFECT_MODE);
                break;
            case AF_RGB_HUEI:
                if (!record->event.pressed) return;
                rgb_led_config_inc_param(RGB_EFFECT_HUE);
                break;
            case AF_RGB_HUED:
                if (!record->event.pressed) return;
                rgb_led_config_dec_param(RGB_EFFECT_HUE);
                break;
            case AF_RGB_SATI:
                if (!record->event.pressed) return;
                rgb_led_config_inc_param(RGB_EFFECT_SAT);
                break;
            case AF_RGB_SATD:
                if (!record->event.pressed) return;
                rgb_led_config_dec_param(RGB_EFFECT_SAT);
                break;
            case AF_RGB_VALI:
                if (!record->event.pressed) return;
                rgb_led_config_inc_param(RGB_EFFECT_VAL);
                break;
            case AF_RGB_VALD:
                if (!record->event.pressed) return;
                rgb_led_config_dec_param(RGB_EFFECT_VAL);
                break;
            case AF_RGB_SPDI:
                if (!record->event.pressed) return;
                rgb_led_config_inc_param(RGB_EFFECT_SPEED);
                break;
            case AF_RGB_SPDD:
                if (!record->event.pressed) return;
                rgb_led_config_dec_param(RGB_EFFECT_SPEED);
                break;
    #endif
    #ifdef RF_ENABLE
            case AF_RF_TOG:
                rf_driver_toggle();
                break;
    #endif

    #ifdef VIAL_ENABLE
            case AF_FN_MO13:
                if (record->event.pressed) {
                    layer_on(1);
                    vial_update_tri_layer(1, 2, 3);
                } else {
                    layer_off(1);
                    vial_update_tri_layer(1, 2, 3);
                }
                break;
            case AF_FN_MO23:
                if (record->event.pressed) {
                    layer_on(2);
                    vial_update_tri_layer(1, 2, 3);
                } else {
                    layer_off(2);
                    vial_update_tri_layer(1, 2, 3);
                }
                break;

            case AF_MACRO00:
            case AF_MACRO01:
            case AF_MACRO02:
            case AF_MACRO03:
            case AF_MACRO04:
            case AF_MACRO05:
            case AF_MACRO06:
            case AF_MACRO07:
            case AF_MACRO08:
            case AF_MACRO09:
            case AF_MACRO10:
            case AF_MACRO11:
            case AF_MACRO12:
            case AF_MACRO13:
            case AF_MACRO14:
            case AF_MACRO15:
                if (!record->event.pressed) return;
                amk_macro_play(id-AF_MACRO00);
                break;
    #endif
            case AF_EEPROM_RESET:
                eeconfig_init();
                break;
            default:
                break;
        }
    }    
}


__attribute__((weak))
bool hook_process_action_main(keyrecord_t *record)
{
    return false;
}

static uint8_t current_layer(keypos_t key)
{
    action_t action = ACTION_TRANSPARENT;
    uint32_t layers = layer_state | default_layer_state;
    /* check top layer first */
    for (int8_t i = 31; i >= 0; i--) {
        if (layers & (1UL<<i)) {
            action = action_for_key(i, key);
            if (action.code != (action_t)ACTION_TRANSPARENT.code) {
                return i;
            }
        }
    }
    /* fall back to layer 0 */
    return 0;
}

bool hook_process_action(keyrecord_t *record)
{
    keyevent_t event = record->event;
    uint8_t tap_count = record->tap.count;

    if (IS_NOEVENT(event)) { return false; }

    action_t action = layer_switch_get_action(event);
    if (action.code == 0) {
        uint8_t layer = current_layer(event.key);
        #ifdef ACTIONMAP_ENABLE
        uint8_t keycode = action_for_key(layer, event.key).key.code;
        #else
        uint8_t keycode = keymap_key_to_keycode(layer, event.key);
        #endif
        if (keycode < KC_CTMB || keycode > KC_CTME) {
            return false;
        } else {
            action = (action_t)ACTION_KEY(keycode);
        }
    }
    switch (action.kind.id) {
    case ACT_LMODS:
    case ACT_RMODS: {
        static uint8_t last_gesc = KC_NO;
        if (action.key.code == KC_GESC) {
            if (event.pressed) {
                uint8_t mods = get_mods();
                if (   (mods & (MOD_BIT(KC_LSFT))) 
                    || (mods & (MOD_BIT(KC_RSFT)))
                    || (mods & (MOD_BIT(KC_LGUI)))
                    || (mods & (MOD_BIT(KC_RGUI)))
                    ) {
                    last_gesc = KC_GRAVE;
                } else {
                    last_gesc = KC_ESC;
                }
                register_code(last_gesc);
            } else {
                unregister_code(last_gesc);
            }

            return true;
        } else if (action.key.code == KC_MEH)  {
            uint8_t mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LSFT) | MOD_BIT(KC_LALT);
            if (event.pressed) {
                add_weak_mods(mods);
            } else {
                del_weak_mods(mods);
            }
            send_keyboard_report();
            return true;
        } else if (action.key.code == KC_HYPER) {
            uint8_t mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LSFT) | MOD_BIT(KC_LALT) | MOD_BIT(KC_LGUI);
            if (event.pressed) {
                add_weak_mods(mods);
            } else {
                del_weak_mods(mods);
            }
            send_keyboard_report();
            return true;
        }
        #ifdef RGB_ENABLE
        else if (action.key.code == KC_F13 && event.pressed){
           rgb_led_config_next();
           return true; 
        }
        #endif
    } break;
    case ACT_LMODS_TAP:
    case ACT_RMODS_TAP: {
        uint8_t mods = (action.kind.id == ACT_LMODS_TAP) ?  action.key.mods : action.key.mods<<4;
        uint8_t code = KC_NO;
        switch (action.key.code) {
        //case KC_LCPO:
        //case KC_LSPO:
        //case KC_LAPO:
        //    code = KC_9;
        //    break;
        //case KC_RCPC:
        //case KC_RSPC:
        //case KC_RAPC:
        //    code = KC_0;
        //    break;
        case KC_SFTENT:
            code = KC_ENT;
            break;
        default:
            return false; 
        }
        if (event.pressed) {
            register_mods(mods);
            if (tap_count > 0) {
                if (record->tap.interrupted) {
                    action_debug("MODS_TAP: Tap: Cancel: add_mods\n");
                    // ad hoc: set 0 to cancel tap
                    record->tap.count = 0;
                } else {
                    action_debug("MODS_TAP: Tap: register_code\n");
                    register_code(code);
                }
            }
        } else {
            unregister_mods(mods);
            if (tap_count > 0) {
                action_debug("MODS_TAP: Tap: unregister_code\n");
                unregister_code(code);
            }
        }
        return true;
    } break;
    }
    return hook_process_action_main(record);
}

__attribute__((weak))
void hook_matrix_change_kb(keyevent_t event)
{
}

void hook_matrix_change(keyevent_t event)
{
#if defined(NRF52) || defined(NRF52840_XXAA)
    if (!IS_NOEVENT(event)) {
        rf_driver.matrix_changed = 1;
    }
#endif

#ifdef RGB_MATRIX_ENABLE
    extern void hook_matrix_change_rgb(keyevent_t event);
    hook_matrix_change_rgb(event);
#endif

#ifdef TYPING_SPEED
    extern void hook_matrix_change_typing(keyevent_t event);
    hook_matrix_change_typing(event);
#endif

#ifdef SCREEN_ENABLE
    extern void hook_matrix_change_screen(keyevent_t event);
    hook_matrix_change_screen(event);
#endif

    hook_matrix_change_kb(event);
}

__attribute__((weak))
void hook_layer_change_kb(uint32_t layer_state)
{
}

void hook_layer_change(uint32_t layer_state)
{
    hook_layer_change_kb(layer_state);
}
