/*
 * pad_keymap.c
 */

#include "pad.h"

#define _______ KC_TRNS

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
         KC_ESC,   KC_FN0, KC_PSCR, KC_BSPC,
        KC_NLCK, KC_PSLS, KC_PAST, KC_PMNS,
          KC_P7,   KC_P8,   KC_P9, KC_PPLS,
          KC_P4,   KC_P5,   KC_P6, KC_PPLS,
          KC_P1,   KC_P2,   KC_P3, KC_PENT,
          KC_P0,   KC_P0, KC_PDOT, KC_PENT),
    [1] = LAYOUT_default(
        KC_BTLD, _______, _______, _______,
         KC_FN1,  KC_FN2,  KC_F13, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______),
};

const action_t fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_FUNCTION(AF_RGB_TOG),
    [2] = ACTION_FUNCTION(AF_RGB_MOD),
};