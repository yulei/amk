/**
 * @file keycode_convert.c
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keycode_convert.h"
#include "actionmap.h"
#include "amk_action.h"

typedef struct {
    uint16_t vial_keycode;
    action_t amk_keycode;
} keycode_map_t;

static const keycode_map_t keycode_map[] = {
    {.vial_keycode= QK_RESET,           .amk_keycode = AC_BTLD},
    {.vial_keycode= GRAVE_ESC,          .amk_keycode = ACTION_KEY(KC_GESC)},
    {.vial_keycode= EEPROM_RESET,       .amk_keycode = ACTION_FUNCTION(AF_EEPROM_RESET)},
    {.vial_keycode= RGB_TOG,            .amk_keycode = ACTION_FUNCTION(AF_RGB_TOG)},
    {.vial_keycode= RGB_MODE_FORWARD,   .amk_keycode = ACTION_FUNCTION(AF_RGB_MOD)},
    {.vial_keycode= RGB_MODE_REVERSE,   .amk_keycode = ACTION_FUNCTION(AF_RGB_MOD)},
    {.vial_keycode= RGB_HUI,            .amk_keycode = ACTION_FUNCTION(AF_RGB_HUEI)},
    {.vial_keycode= RGB_HUD,            .amk_keycode = ACTION_FUNCTION(AF_RGB_HUED)},
    {.vial_keycode= RGB_SAI,            .amk_keycode = ACTION_FUNCTION(AF_RGB_SATI)},
    {.vial_keycode= RGB_SAD,            .amk_keycode = ACTION_FUNCTION(AF_RGB_SATD)},
    {.vial_keycode= RGB_VAI,            .amk_keycode = ACTION_FUNCTION(AF_RGB_VALI)},
    {.vial_keycode= RGB_VAD,            .amk_keycode = ACTION_FUNCTION(AF_RGB_VALD)},
    {.vial_keycode= RGB_SPI,            .amk_keycode = ACTION_FUNCTION(AF_RGB_SPDI)},
    {.vial_keycode= RGB_SPD,            .amk_keycode = ACTION_FUNCTION(AF_RGB_SPDD)},
    {.vial_keycode= FN_MO13,            .amk_keycode = ACTION_FUNCTION(AF_FN_MO13)},
    {.vial_keycode= FN_MO23,            .amk_keycode = ACTION_FUNCTION(AF_FN_MO23)},
    {.vial_keycode= MACRO00,            .amk_keycode = ACTION_FUNCTION(AF_MACRO00)},
    {.vial_keycode= MACRO01,            .amk_keycode = ACTION_FUNCTION(AF_MACRO01)},
    {.vial_keycode= MACRO02,            .amk_keycode = ACTION_FUNCTION(AF_MACRO02)},
    {.vial_keycode= MACRO03,            .amk_keycode = ACTION_FUNCTION(AF_MACRO03)},
    {.vial_keycode= MACRO04,            .amk_keycode = ACTION_FUNCTION(AF_MACRO04)},
    {.vial_keycode= MACRO05,            .amk_keycode = ACTION_FUNCTION(AF_MACRO05)},
    {.vial_keycode= MACRO06,            .amk_keycode = ACTION_FUNCTION(AF_MACRO06)},
    {.vial_keycode= MACRO07,            .amk_keycode = ACTION_FUNCTION(AF_MACRO07)},
    {.vial_keycode= MACRO08,            .amk_keycode = ACTION_FUNCTION(AF_MACRO08)},
    {.vial_keycode= MACRO09,            .amk_keycode = ACTION_FUNCTION(AF_MACRO09)},
    {.vial_keycode= MACRO10,            .amk_keycode = ACTION_FUNCTION(AF_MACRO10)},
    {.vial_keycode= MACRO11,            .amk_keycode = ACTION_FUNCTION(AF_MACRO11)},
    {.vial_keycode= MACRO12,            .amk_keycode = ACTION_FUNCTION(AF_MACRO12)},
    {.vial_keycode= MACRO13,            .amk_keycode = ACTION_FUNCTION(AF_MACRO13)},
    {.vial_keycode= MACRO14,            .amk_keycode = ACTION_FUNCTION(AF_MACRO14)},
    {.vial_keycode= MACRO15,            .amk_keycode = ACTION_FUNCTION(AF_MACRO15)},
};

static uint16_t map_keycode(uint16_t keycode, bool amk) 
{
    for (int i = 0; i < sizeof(keycode_map)/sizeof(keycode_map_t); i++) {
        bool matched = amk ? keycode_map[i].amk_keycode.code == keycode : keycode_map[i].vial_keycode == keycode;
        if (matched) {
            return amk ? keycode_map[i].vial_keycode : keycode_map[i].amk_keycode.code;
        }
    }

    return 0;
}

/* keycode to system usage */
static uint8_t usage_to_keycode(uint16_t usage) 
{
    switch(usage) {
        // system usage
        case SYSTEM_POWER_DOWN:
            return KC_SYSTEM_POWER;
        case SYSTEM_SLEEP:
            return KC_SYSTEM_SLEEP;
        case SYSTEM_WAKE_UP:
            return KC_SYSTEM_WAKE;
        // consumer usage
        case AUDIO_MUTE:
            return KC_AUDIO_MUTE;
        case AUDIO_VOL_UP:
            return KC_AUDIO_VOL_UP;
        case AUDIO_VOL_DOWN:
            return KC_AUDIO_VOL_DOWN;
        case TRANSPORT_NEXT_TRACK:
            return KC_MEDIA_NEXT_TRACK;
        case TRANSPORT_PREV_TRACK:
            return KC_MEDIA_PREV_TRACK;
        case TRANSPORT_FAST_FORWARD:
            return KC_MEDIA_FAST_FORWARD;
        case TRANSPORT_REWIND:
            return KC_MEDIA_REWIND;
        case TRANSPORT_STOP:
            return KC_MEDIA_STOP;
        case TRANSPORT_STOP_EJECT:
            return KC_MEDIA_EJECT;
        case TRANSPORT_PLAY_PAUSE:
            return KC_MEDIA_PLAY_PAUSE;
        case APPLAUNCH_CC_CONFIG:
            return KC_MEDIA_SELECT;
        case APPLAUNCH_EMAIL:
            return KC_MAIL;
        case APPLAUNCH_CALCULATOR:
            return KC_CALCULATOR;
        case APPLAUNCH_LOCAL_BROWSER:
            return KC_MY_COMPUTER;
        case APPCONTROL_SEARCH:
            return KC_WWW_SEARCH;
        case APPCONTROL_HOME:
            return KC_WWW_HOME;
        case APPCONTROL_BACK:
            return KC_WWW_BACK;
        case APPCONTROL_FORWARD:
            return KC_WWW_FORWARD;
        case APPCONTROL_STOP:
            return KC_WWW_STOP;
        case APPCONTROL_REFRESH:
            return KC_WWW_REFRESH;
        case APPCONTROL_BOOKMARKS:
            return KC_WWW_FAVORITES;
        case BRIGHTNESS_INCREMENT:
            return KC_BRIGHTNESS_INC;
        case BRIGHTNESS_DECREMENT:
            return KC_BRIGHTNESS_DEC;
        default:
            break;
    }

    return 0;
}

uint16_t amk_to_vial(uint16_t keycode)
{
    action_t action;
    action.code = keycode;
    uint16_t vial_kc = 0;
    switch (action.kind.id) {
        /* Key and Mods */
        case ACT_LMODS:
        case ACT_RMODS:
            {
                uint8_t mods = (action.kind.id == ACT_LMODS) ?  action.key.mods : action.key.mods<<4;
                vial_kc = (mods << 8) | action.key.code;
                if (vial_kc == KC_GESC) {
                    vial_kc = GRAVE_ESC;
                }
            }
            break;
        case ACT_LMODS_TAP:
        case ACT_RMODS_TAP:
            {
                uint8_t mods = (action.kind.id == ACT_LMODS_TAP) ?  action.key.mods : action.key.mods<<4;
                switch (action.key.code) {
                    case MODS_ONESHOT:
                        // Oneshot modifier
                        vial_kc = QK_ONE_SHOT_MOD | action.key.code;
                        break;
                    case MODS_TAP_TOGGLE:
                        // Todo
                        break;
                    default:
                        vial_kc = QK_MOD_TAP | (mods << 8) | action.key.code;
                        break;
                }
            }
            break;
        case ACT_USAGE:
            vial_kc = usage_to_keycode(action.usage.code);
            break;
        case ACT_MOUSEKEY:
            vial_kc = action.key.code;
            break;
        case ACT_LAYER:
            {
                uint8_t bits = action.layer_bitop.bits >> 1;
                if (bits > 2) bits = 3;
                uint8_t layer = action.layer_bitop.part*4 + bits;

                switch (action.layer_bitop.op) {
                    case OP_BIT_AND: 
                        break;
                    case OP_BIT_OR:
                        break;
                    case OP_BIT_XOR:
                        vial_kc = QK_TOGGLE_LAYER | (layer & 0xF) | (action.layer_bitop.on << 4);
                        break;
                    case OP_BIT_SET:
                        if (action.layer_bitop.on == 0) {
                            vial_kc = QK_DEF_LAYER | (layer & 0xF);
                        } else {
                            vial_kc = QK_TO | (layer & 0xF) | (action.layer_bitop.on << 4);
                        }
                }
            }
            break;
        case ACT_LAYER_TAP:
        case ACT_LAYER_TAP_EXT:
            switch (action.layer_tap.code) {
                case 0xc0 ... 0xdf:
                    /* layer On/Off with modifiers */
                    vial_kc = QK_LAYER_MOD | (action.layer_tap.val<<4) | (action.layer_tap.code&0x0F);
                    break;
                case OP_TAP_TOGGLE:
                    /* tap toggle */
                    vial_kc = QK_LAYER_TAP_TOGGLE | action.layer_tap.val;
                    break;
                case OP_ON_OFF:
                    vial_kc = QK_MOMENTARY | action.layer_tap.val;
                    break;
                case OP_OFF_ON:
                    // TODO
                    break;
                case OP_SET_CLEAR:
                    // TODO
                    break;
                default:
                    /* tap key */
                    vial_kc = QK_LAYER_TAP | (action.layer_tap.val << 8) | (action.layer_tap.code);
                    break;
            }
            break;
        /* Extentions */
        case ACT_MACRO:
        case ACT_COMMAND:
        case ACT_FUNCTION:
            vial_kc = map_keycode(keycode, true);
            break;
        default:
            break;
    }

    return vial_kc;
}

static action_t convert_basic(uint16_t keycode)
{
    action_t ac = ACTION_NO;
    switch (keycode) {
        case KC_A ... KC_EXSEL:
        case KC_LCTRL ... KC_RGUI:
            ac = (action_t)ACTION_KEY(keycode);
            break;
        case KC_SYSTEM_POWER ... KC_SYSTEM_WAKE:
            ac = (action_t)ACTION_USAGE_SYSTEM(KEYCODE2SYSTEM(keycode));
            break;
        case KC_AUDIO_MUTE ... KC_BRIGHTNESS_DEC:
            ac = (action_t)ACTION_USAGE_CONSUMER(KEYCODE2CONSUMER(keycode));
            break;
        case KC_MS_UP ... KC_MS_ACCEL2:
            ac = (action_t)ACTION_MOUSEKEY(keycode);
            break;
        case KC_TRNS:
            ac = (action_t)ACTION_TRANSPARENT;
            break;
        case KC_BOOTLOADER:
            ac = (action_t)ACTION_COMMAND(COMMAND_BOOTLOADER, 0);
            break;
        default:
            break;
    }

    return ac;
}

uint16_t vial_to_amk(uint16_t keycode)
{
    action_t amk_kc = AC_NO;
    switch( keycode) {
        case QK_BASIC ... QK_BASIC_MAX:
            amk_kc = convert_basic(keycode);
        break;
        case QK_MODS  ...  QK_MODS_MAX: {
            amk_kc = (action_t)ACTION_MODS_KEY(keycode >> 8, keycode & 0xFF);
        } break;
        case QK_FUNCTION ... QK_FUNCTION_MAX:
        // not handled
        break; 
        case QK_MACRO ... QK_MACRO_MAX:
        // not handled
        break;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX: {
            amk_kc = (action_t)ACTION_LAYER_TAP_KEY((keycode >> 0x8) & 0xF, keycode & 0xFF);
        }
        break;
        case QK_TO ... QK_TO_MAX: {
            amk_kc = (action_t)ACTION_LAYER_SET(keycode & 0xF, (keycode >> 0x4) & 0x3);
        }break;
        case QK_MOMENTARY ... QK_MOMENTARY_MAX: {
            amk_kc = (action_t)ACTION_LAYER_MOMENTARY(keycode & 0xFF);
        }break;
        case QK_DEF_LAYER ... QK_DEF_LAYER_MAX: {
            amk_kc = (action_t)ACTION_DEFAULT_LAYER_SET(keycode & 0xFF);
        }break;
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX: {
            amk_kc = (action_t) ACTION_LAYER_TOGGLE(keycode & 0xFF);
        }break;
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX: {
            // not handled
        }break;
        case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX: {
            amk_kc = (action_t)ACTION_MODS_ONESHOT(keycode & 0xFF);
        }break;
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
        // not handled
        break;
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX: {
            amk_kc = (action_t)ACTION_LAYER_TAP_TOGGLE(keycode & 0xFF);
        }break;
        case QK_LAYER_MOD ... QK_LAYER_MOD_MAX: {
            amk_kc = (action_t)ACTION_LAYER_MODS((keycode >> 4) & 0xF, keycode & 0xF);
        }break;
        case QK_MOD_TAP ... QK_MOD_TAP_MAX: {
            amk_kc = (action_t)ACTION_MODS_TAP_KEY((keycode >> 0x8) & 0x1F, keycode & 0xFF);
        }break;
        case QK_RESET ... SAFE_RANGE: {
            amk_kc.code = map_keycode(keycode, false);
        }break;

        default:
        break;
    }

    return amk_kc.code;
}

// the keycode stored in buffer was in big endian 
//#define COMPOSE_KEYCODE(x, y) ((((uint16_t)(x))&0xFF) | ((((uint16_t)(y))&0xFF) << 8))
#define COMPOSE_KEYCODE(x, y) (((((uint16_t)(x))&0xFF)<<8) | ((((uint16_t)(y))&0xFF) ))

void convert_keycode_buffer(uint8_t *keycodes, uint8_t size, bool amk)
{
    uint8_t *cur = keycodes;
    for (uint8_t i = 0; i < size/2; i++){
        uint16_t source = COMPOSE_KEYCODE(cur[i*2], cur[i*2+1]);
        uint16_t target = amk ? amk_to_vial(source) : vial_to_amk(source);
        cur[i*2] = (uint8_t)((target>>8) & 0xFF);
        cur[i*2+1] = (uint8_t)(target & 0xFF);
    }
}