/**
 * @file m65_gcmap.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "m65.h"

#define GC_FN0 ACTION_LAYER_MOMENTARY(1)
#define GC_FN1 ACTION_FUNCTION(AF_RGB_TOG)
#define GC_FN2 ACTION_FUNCTION(AF_RGB_MOD)
#define GC_FN3 ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)
#define GC_FN4 ACTION_LAYER_TAP_KEY(1, KC_SPC)

GC_KEYMAPS = {
    [0] = LAYOUT_default(
        GC_GESC,    GC_1,     GC_2, GC_3, GC_4, GC_5, GC_6, GC_7,    GC_8,   GC_9,    GC_0, GC_MINS,  GC_EQL, GC_BSPC, 
        GC_TAB,     GC_Q,     GC_W, GC_E, GC_R, GC_T, GC_Y, GC_U,    GC_I,   GC_O,    GC_P, GC_LBRC, GC_RBRC, GC_BSLS, GC_PGUP,
        GC_FN3,     GC_A,     GC_S, GC_D, GC_F, GC_G, GC_H, GC_J,    GC_K,   GC_L, GC_SCLN, GC_QUOT,           GC_ENT, GC_PGDN,
        GC_LSFT,    GC_Z,     GC_X, GC_C, GC_V, GC_B, GC_N, GC_M, GC_COMM, GC_DOT, GC_SLSH,          GC_RSFT,   GC_UP,  GC_END,
        GC_LCTL, GC_LGUI,  GC_LALT,                 GC_FN4,                         GC_RALT,  GC_FN0, GC_LEFT, GC_DOWN, GC_RIGHT),
    [1] = LAYOUT_default(
        GC_GRV,   GC_F1,    GC_F2,   GC_F3,   GC_F4,   GC_F5,   GC_F6,   GC_F7,   GC_F8,   GC_F9,  GC_F10,  GC_F11,  GC_F12, _______,
        GC_BTLD, GC_F24,   GC_F16,   GC_F17,  GC_F18,  GC_F19, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, GC_FN1,   GC_FN2, _______, _______, _______, GC_LEFT, GC_DOWN,   GC_UP,GC_RIGHT, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                            _______,                   _______, _______, _______, _______, _______),
};
