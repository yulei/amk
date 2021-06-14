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
#include "amk_keymap.h"
#include "amk_printf.h"
#include "wait.h"

#ifdef RGB_ENABLE
#include "rgb_led.h"
#endif

#if defined(NRF52) || defined(NRF52840_XXAA)
#include "common_config.h"
#endif

__attribute__((weak))
void keyboard_set_rgb(bool on)
{
#ifdef RGB_ENABLE
    amk_printf("keyboard_set_rgb: %d\n", on);
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
    if (!record->event.pressed) return; // do not process key released event

    switch (id) {
#ifdef RGB_ENABLE
        case AF_RGB_TOG:
            keyboard_set_rgb(!rgb_led_config_enabled());
            amk_printf("Toggle rgb: %d\n", rgb_led_config_enabled());
            break;
        case AF_RGB_MOD:
            rgb_led_config_inc_param(RGB_EFFECT_MODE);
            break;
        case AF_RGB_HUEI:
            rgb_led_config_inc_param(RGB_EFFECT_HUE);
            break;
        case AF_RGB_HUED:
            rgb_led_config_dec_param(RGB_EFFECT_HUE);
            break;
        case AF_RGB_SATI:
            rgb_led_config_inc_param(RGB_EFFECT_SAT);
            break;
        case AF_RGB_SATD:
            rgb_led_config_dec_param(RGB_EFFECT_SAT);
            break;
        case AF_RGB_VALI:
            rgb_led_config_inc_param(RGB_EFFECT_VAL);
            break;
        case AF_RGB_VALD:
            rgb_led_config_dec_param(RGB_EFFECT_VAL);
            break;
        case AF_RGB_SPDI:
            rgb_led_config_inc_param(RGB_EFFECT_SPEED);
            break;
        case AF_RGB_SPDD:
            rgb_led_config_dec_param(RGB_EFFECT_SPEED);
            break;
#endif
        case AF_EEPROM_RESET:
            eeconfig_init();
            break;
        default:
            break;
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
                    amk_printf("MODS_TAP: Tap: Cancel: add_mods\n");
                    // ad hoc: set 0 to cancel tap
                    record->tap.count = 0;
                } else {
                    amk_printf("MODS_TAP: Tap: register_code\n");
                    register_code(code);
                }
            }
        } else {
            unregister_mods(mods);
            if (tap_count > 0) {
                amk_printf("MODS_TAP: Tap: unregister_code\n");
                unregister_code(code);
            }
        }
        return true;
    } break;
    }
    return hook_process_action_main(record);
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
}