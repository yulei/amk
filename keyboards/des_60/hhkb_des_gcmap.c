/**
 * @file falcon_ec_gcmap.c
 */

#include "hhkb_des.h"

#ifdef VIAL_ENABLE
#define GC_FN0  ACTION_LAYER_MOMENTARY(1)
#define GC_FN1  ACTION_LAYER_TAP_KEY(1, KC_SPC)
#define GC_FN2  ACTION_FUNCTION(AF_RGB_TOG)
#define GC_FN3  ACTION_FUNCTION(AF_RGB_MOD)
#define GC_FN4  ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)
#define GC_FN5  ACTION_FUNCTION(AF_EEPROM_RESET)
#else
#define GC_FN0  KC_FN0
#define GC_FN1  KC_FN1
#define GC_FN2  KC_FN2
#define GC_FN3  KC_FN3
#define GC_FN4  KC_FN4
#define GC_FN5  KC_FN5

const action_t fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPC),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
    [4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS),
    [5] = ACTION_FUNCTION(AF_EEPROM_RESET),
};
#endif

GC_KEYMAPS = {
    [0]=LAYOUT_default(
        GC_GESC,    GC_1,     GC_2, GC_3, GC_4, GC_5, GC_6, GC_7,    GC_8,   GC_9,    GC_0, GC_MINS,  GC_EQL, GC_BSLS, GC_GRV,
        GC_TAB,     GC_Q,     GC_W, GC_E, GC_R, GC_T, GC_Y, GC_U,    GC_I,   GC_O,    GC_P, GC_LBRC, GC_RBRC, GC_BSPC,
        GC_LCTL,    GC_A,     GC_S, GC_D, GC_F, GC_G, GC_H, GC_J,    GC_K,   GC_L, GC_SCLN, GC_QUOT,           GC_ENT,
        GC_LSFT,    GC_Z,     GC_X, GC_C, GC_V, GC_B, GC_N, GC_M, GC_COMM, GC_DOT, GC_SLSH,          GC_RSFT,  GC_FN0,
                 GC_LGUI,  GC_LALT,                 GC_SPC,                                          GC_RALT,  GC_RCTL),
    [1]=LAYOUT_default(
        _______,   GC_F1,   GC_F2,   GC_F3,   GC_F4,   GC_F5,   GC_F6,   GC_F7,   GC_F8,   GC_F9,  GC_F10,  GC_F11,  GC_F12,  GC_INS, GC_DEL,
        GC_CAPS, _______, _______, _______, _______, _______, _______, _______, GC_PSCR, _______, _______, GC_UP, _______, _______,
        _______, GC_VOLU, GC_VOLD, GC_MUTE, _______, _______, GC_PAST, GC_PSLS, GC_HOME, GC_PGUP, GC_LEFT, GC_RIGHT,          _______,
        _______, _______, _______, _______, _______, _______, GC_PPLS, GC_PMNS, GC_END, GC_PGDN, GC_DOWN,          _______, _______,
                 _______, _______,                   _______,                                                       _______,_______),
};

