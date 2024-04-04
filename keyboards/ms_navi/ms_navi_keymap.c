/**
 * @file ms_navi_keymap.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ms_navi.h"

#ifndef LAYOUT_current
#define LAYOUT_current LAYOUT_default
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0]=LAYOUT_current(
		QK_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL,    KC_BSPC, KC_PSCR, KC_PGDN, KC_PGUP,
		KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC,    KC_BSLS, KC_DEL,
		KC_CAPS,  KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,         KC_ENT, 
		KC_LSFT,   KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH,           KC_RSFT,            KC_UP,
		KC_LCTL, KC_LGUI, KC_LALT,                 KC_SPC,                      KC_RALT, KC_RGUI,  MO(1), KC_LEFT, KC_DOWN, KC_RGHT),

	[1]=LAYOUT_current(
		_______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_F24,  KC_F16, _______, _______,
		_______, RGB_TOG, RGB_MOD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		QK_BOOT, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, _______, _______,          _______, 
		_______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,           KC_VOLU,
		_______, _______, _______,                            KC_MPLY,                                     _______, _______, _______, KC_MPRV, KC_VOLD, KC_MNXT),
	[2]=LAYOUT_current(
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, 
		_______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
		_______, _______, _______,                            _______,                                     _______, _______, _______, _______, _______, _______),
	[3]=LAYOUT_current(
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, 
		_______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
		_______, _______, _______,                            _______,                                     _______, _______, _______, _______, _______, _______),
};
#else
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0]=LAYOUT_all(
		QK_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_BSPC, KC_GRV, KC_PSCR, KC_PGDN, KC_PGUP,
		KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC,    KC_BSLS, KC_DEL,
		KC_CAPS,  KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,         KC_ENT, 
		KC_LSFT,   KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH,   KC_RSFT, KC_RSFT, MO(1),           KC_UP,
		KC_LCTL, KC_LGUI, KC_LALT, KC_LALT,                KC_SPC,            KC_RALT, KC_RALT, KC_RGUI,  MO(1), KC_LEFT, KC_DOWN, KC_RGHT),

	[1]=LAYOUT_all(
		_______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_F24, KC_F24, KC_F16,  KC_F16, _______, _______,
		_______, RGB_TOG, RGB_MOD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		QK_BOOT, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, _______, _______,          _______, 
		_______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_VOLU,
		_______, _______, _______, _______,                   KC_MPLY,                            _______, _______, _______, _______, KC_MPRV, KC_VOLD, KC_MNXT),
};
#endif