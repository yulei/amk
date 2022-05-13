/**
 * @file imp_ec_keymap.c
 */

#include "imp_ec.h"
#include "keymap.h"
#define AC_FN0 	ACTION_LAYER_TAP_KEY(1, KC_PAUS)
#define AC_FN1  ACTION_LAYER_TAP_KEY(1, KC_SPC)
#define AC_FN2  ACTION_FUNCTION(AF_RGB_TOG)
#define AC_FN3	ACTION_FUNCTION(AF_RGB_MOD)
#define AC_FN4	ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)
#define AC_FN5	ACTION_LAYER_MOMENTARY(1)

const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0]=LAYOUT_all(
		AC_FN2,

		AC_ESC, AC_F1, AC_F2, AC_F3, AC_F4, AC_F5, AC_F6, AC_F7, AC_F8, AC_F9, AC_F10, AC_F11, AC_F12, AC_F13,  AC_PSCR, AC_NLCK, AC_FN0,

		AC_GRV, AC_1, AC_2, AC_3, AC_4, AC_5, AC_6, AC_7, AC_8, AC_9, AC_0, AC_MINS, AC_EQL,  AC_BSPC,   AC_INS, AC_HOME, AC_PGUP,
		AC_TAB, AC_Q, AC_W, AC_E, AC_R, AC_T, AC_Y, AC_U, AC_I, AC_O, AC_P, AC_LBRC, AC_RBRC, AC_BSLS,   AC_DEL, AC_END, AC_PGDN,
		AC_FN4,   AC_A, AC_S, AC_D, AC_F, AC_G, AC_H, AC_J, AC_K, AC_L, AC_SCLN, AC_QUOT, AC_BSLS, AC_ENT, AC_ENT,
		AC_LSFT, AC_LSFT, AC_LGUI, AC_Z, AC_X, AC_C, AC_V, AC_B, AC_N, AC_M, AC_COMM, AC_DOT, AC_SLSH,      AC_RSFT,            AC_UP,
		AC_LCTL, AC_LGUI, AC_LALT,             AC_SPC,     AC_FN1,               AC_RALT,     AC_RALT,AC_RGUI,AC_RCTL,  AC_LEFT, AC_DOWN, AC_RGHT),

	[1]=LAYOUT_all(
		_______,

		AC_MUTE, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,

		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
		_______,  AC_FN2,  AC_FN3,  AC_F13, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
		AC_BTLD, _______, _______, _______, _______, _______, AC_LEFT, AC_DOWN,   AC_UP, AC_RGHT, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                   AC_VOLU,
		_______, _______, _______,                   AC_MPLY, AC_MPLY,                            _______, _______, _______, _______,          AC_MPRV, AC_VOLD, AC_MNXT),
};
