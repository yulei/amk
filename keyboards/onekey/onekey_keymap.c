/*
 * onekey_keymap.c
 */

#include "onekey.h"

//#define _______ KC_TRNS
//const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//    [0] = LAYOUT_default(KC_ESC)
//};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ext(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5)
};