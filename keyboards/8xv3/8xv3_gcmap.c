/*
 * @file 8xv3_actionmap.c
 */

#include "8xv3.h"

#define GC_FN0 	ACTION_LAYER_TAP_KEY(1, KC_PAUS)
#define GC_FN1	ACTION_LAYER_MOMENTARY(1)
#define GC_FN2  ACTION_FUNCTION(AF_RGB_TOG)
#define GC_FN3	ACTION_FUNCTION(AF_RGB_MOD)
#define GC_FN4	ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)

GC_KEYMAPS = {
	[0]=LAYOUT_default(
		GC_ESC, GC_F1, GC_F2, GC_F3, GC_F4, GC_F5, GC_F6, GC_F7, GC_F8, GC_F9, GC_F10, GC_F11, GC_F12,   GC_PSCR, GC_SLCK, GC_FN0,

		GC_GRV, GC_1, GC_2, GC_3, GC_4, GC_5, GC_6, GC_7, GC_8, GC_9, GC_0, GC_MINS, GC_EQL,  GC_BSPC,   GC_INS, GC_HOME, GC_PGUP,
		GC_TAB, GC_Q, GC_W, GC_E, GC_R, GC_T, GC_Y, GC_U, GC_I, GC_O, GC_P, GC_LBRC, GC_RBRC, GC_BSLS,   GC_DEL, GC_END, GC_PGDN,
		GC_FN4,   GC_A, GC_S, GC_D, GC_F, GC_G, GC_H, GC_J, GC_K, GC_L, GC_SCLN, GC_QUOT,      GC_ENT,
		GC_LSFT,      GC_Z, GC_X, GC_C, GC_V, GC_B, GC_N, GC_M, GC_COMM, GC_DOT, GC_SLSH,     GC_RSFT,            GC_UP,
		GC_LCTL, GC_LGUI, GC_LALT,                 GC_SPC,                    GC_RALT,GC_FN1,GC_RCTL,  GC_LEFT, GC_DOWN, GC_RGHT),

	[1]=LAYOUT_default(
		GC_MUTE,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  GC_F24,          _______, _______, _______,

		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
		_______,  GC_FN2,  GC_FN3,  GC_F13, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
		GC_BTLD, _______, _______, _______, _______, _______, GC_LEFT, GC_DOWN,   GC_UP, GC_RGHT, _______, _______,          _______,
		_______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,                   GC_VOLU,
		_______, _______, _______,                            GC_MPLY,                                     _______, _______, _______,          GC_MPRV, GC_VOLD, GC_MNXT),
};

#if 0
const action_t fn_actions[] = {
    [0] = ACTION_LAYER_TAP_KEY(1, KC_PAUS),
    [1] = ACTION_LAYER_MOMENTARY(1),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
	[4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS),
};
#endif
