/**
 * controller_keymap.c
 *  default keymap for the hhkb ble controller
 */

#include "controller.h"
#include "custom_action.h"

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]=LAYOUT_default(
        ESC,     1,     2, 3, 4, 5, 6, 7,    8,   9,    0, MINS,  EQL, BSLS, GRV,
        TAB,     Q,     W, E, R, T, Y, U,    I,   O,    P, LBRC, RBRC, BSPC,
        LCTL,    A,     S, D, F, G, H, J,    K,   L, SCLN, QUOT,       ENT,
        LSFT,    Z,     X, C, V, B, N, M, COMM, DOT, SLSH,       RSFT,  FN0,
                 LGUI,  LALT,        FN1,               RALT,  RCTL),
    [1]=LAYOUT_default(
        TRNS,   F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,  F10,  F11,  F12, TRNS, PSCR,
        BTLD,  FN2,  FN3, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,
        CAPS, TRNS, TRNS, TRNS, TRNS, TRNS, LEFT, DOWN,   UP,RIGHT, TRNS, TRNS,       TRNS,
        TRNS,  F21,  F22,  F23, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       TRNS,
              TRNS, TRNS,             TRNS,                   TRNS, TRNS),
};

/*
 * Fn action definition
 */
const action_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPC),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
};