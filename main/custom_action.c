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
#include "amk_printf.h"


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

#define KC_GESC KC_POST_FAIL

__attribute__((weak))
bool hook_process_action_main(keyrecord_t *record);

bool hook_process_action(keyrecord_t *record)
{
    keyevent_t event = record->event;

    if (IS_NOEVENT(event)) { return false; }

    action_t action = layer_switch_get_action(event);
    if (action.kind.id == ACT_MODS) {
        static uint8_t last_gesc = KC_ESC;
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
        }

    }
    return false;
}
