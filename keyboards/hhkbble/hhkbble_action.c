/**
 * hhkbble_action.c
 *  default action keymap for the hhkbble
 */

#include "hhkbble.h"
#include "actionmap.h"

#define _______ AC_TRNS
#define AC_FN0 ACTION_LAYER_MOMENTARY(1)
#define AC_FN1 ACTION_LAYER_TAP_KEY(1, KC_SPC)
#define AC_FN2 ACTION_FUNCTION(AF_RGB_TOG)
#define AC_FN3 ACTION_FUNCTION(AF_RGB_MOD)

const action_t PROGMEM actionmaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]=ACTION_default(
        AC_ESC,     AC_1,     AC_2, AC_3, AC_4, AC_5, AC_6, AC_7,    AC_8,   AC_9,    AC_0, AC_MINUS,  AC_EQL, AC_BSLS, AC_GRV,
        AC_TAB,     AC_Q,     AC_W, AC_E, AC_R, AC_T, AC_Y, AC_U,    AC_I,   AC_O,    AC_P, AC_LBRC, AC_RBRC, AC_BSPC,
        AC_LCTL,    AC_A,     AC_S, AC_D, AC_F, AC_G, AC_H, AC_J,    AC_K,   AC_L, AC_SCLN, AC_QUOT,           AC_ENT,
        AC_LSFT,    AC_Z,     AC_X, AC_C, AC_V, AC_B, AC_N, AC_M, AC_COMM, AC_DOT, AC_SLSH,          AC_RSFT,  AC_FN0,
                 AC_LGUI,  AC_LALT,                 AC_FN1,                                          AC_RALT,  AC_RCTL),
    [1]=ACTION_default(
        _______,   AC_F1,   AC_F2,   AC_F3,   AC_F4,   AC_F5,   AC_F6,   AC_F7,   AC_F8,   AC_F9,  AC_F10,  AC_F11,  AC_F12, _______, AC_PSCR,
        AC_BTLD,  AC_FN2,  AC_FN3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        AC_CAPS, AC_VOLU, AC_VOLD, _______, _______, _______, AC_LEFT, AC_DOWN,   AC_UP,AC_RIGHT, _______, _______,          _______,
        _______,  AC_F21,  AC_F22,  AC_F23,  AC_F24, _______, _______, _______, _______, _______, _______,          _______, _______,
                 _______, _______,                   _______,                                                       _______,_______),
    [2]=ACTION_default(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
                 _______, _______,                   _______,                                                       _______, _______),
    [3]=ACTION_default(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
                 _______, _______,                   _______,                                                       _______, _______),
};

const uint32_t keymaps_size = sizeof(actionmaps);