/*
 * m65_keymap.c
 */

#include "m65.h"

#define _______ KC_TRNS
const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
        KC_GESC,    KC_1,     KC_2, KC_3, KC_4, KC_5, KC_6, KC_7,    KC_8,   KC_9,    KC_0, KC_MINS,  KC_EQL, KC_BSPC, 
        KC_TAB,     KC_Q,     KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U,    KC_I,   KC_O,    KC_P, KC_LBRC, KC_RBRC, KC_BSLS, 
        KC_FN4,     KC_A,     KC_S, KC_D, KC_F, KC_G, KC_H, KC_J,    KC_K,   KC_L, KC_SCLN, KC_QUOT,           KC_ENT, KC_PGDN,
        KC_LSFT,    KC_Z,     KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH,          KC_RSFT,   KC_UP,  KC_END,
        KC_LCTL, KC_LGUI,  KC_LALT,                 KC_FN1,                         KC_RALT,  KC_FN0, KC_LEFT, KC_DOWN, KC_RIGHT),
    [1] = LAYOUT_default(
        KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, _______,
        KC_BTLD, KC_F24,   KC_F16,   KC_F17,  KC_F18,  KC_F19, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, KC_FN2,   KC_FN3, _______, _______, _______, KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                            _______,                   _______, _______, _______, _______, _______),
};

const action_t fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPC),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
    [4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS),
    [5] = ACTION_FUNCTION(AF_EEPROM_RESET),
};