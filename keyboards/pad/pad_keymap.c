/*
 * pad_keymap.c
 */

#include "pad.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
         KC_ESC,   MO(1), KC_PSCR, KC_BSPC,
        KC_NLCK, KC_PSLS, KC_PAST, KC_PMNS,
          KC_P7,   KC_P8,   KC_P9, KC_PPLS,
          KC_P4,   KC_P5,   KC_P6, KC_PPLS,
          KC_P1,   KC_P2,   KC_P3, KC_PENT,
          KC_P0,   KC_P0, KC_PDOT, KC_PENT),
    [1] = LAYOUT_default(
          RESET, _______, _______, _______,
        RGB_TOG, RGB_MOD,  KC_F13, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______),
};
