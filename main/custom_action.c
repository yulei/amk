/**
 * custom_action.c
 *      action function defintion for custom behavior
 */
#include "custom_action.h"
#include "eeconfig.h"
#include "rgb_effects.h"

#include "action.h"
#include "action_layer.h"
#include "action_util.h"
#include "keymap.h"
#include "amk_printf.h"
#include "wait.h"


__attribute__((weak))
void keyboard_set_rgb(bool on)
{
#ifdef RGB_EFFECTS_ENABLE
    amk_printf("keyboard_set_rgb: %d\n", on);
    if (on) {
        if (!rgb_effects_enabled()) {
            rgb_effects_toggle();
        }
    } else {
        if (rgb_effects_enabled()) {
            rgb_effects_toggle();
        }
    }
#endif
}

#ifdef RGB_EFFECTS_ENABLE
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    if (!record->event.pressed) return; // do not press key released event

    switch (id) {
        case AF_RGB_TOG:
            rgb_effects_toggle();
            keyboard_set_rgb(rgb_effects_enabled());
            amk_printf("Toggle rgb: %d\n", rgb_effects_enabled());
            break;
        case AF_RGB_MOD:
            rgb_effects_inc_mode();
            break;
        case AF_RGB_HUEI:
            rgb_effects_inc_hue();
            break;
        case AF_RGB_HUED:
            rgb_effects_dec_hue();
            break;
        case AF_RGB_SATI:
            rgb_effects_inc_sat();
            break;
        case AF_RGB_SATD:
            rgb_effects_dec_sat();
            break;
        case AF_RGB_VALI:
            rgb_effects_inc_val();
            break;
        case AF_RGB_VALD:
            rgb_effects_dec_val();
            break;
        case AF_EEPROM_RESET:
            eeconfig_init();
            break;
        default:
            break;
    }
}
#endif


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

bool hook_process_action_no(keyrecord_t *record)
{
    keyevent_t event = record->event;
    uint8_t tap_count = record->tap.count;

    if (IS_NOEVENT(event)) { return false; }

    action_t action = layer_switch_get_action(event);
    if (action.code == 0) {
        uint8_t layer = current_layer(event.key);
        uint8_t keycode = keymap_key_to_keycode(layer, event.key);
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
