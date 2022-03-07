/**
 * @file falcon_ec_keymap.c
 */

#include "falcon_ec.h"
#include "keymap.h"

/*
 * Fn action definition
 */
#ifdef ACTIONMAP_ENABLE
#define AC_FN0  ACTION_LAYER_MOMENTARY(1)
#define AC_FN1  ACTION_LAYER_TAP_KEY(1, KC_SPC)
#define AC_FN2  ACTION_FUNCTION(AF_RGB_TOG)
#define AC_FN3  ACTION_FUNCTION(AF_RGB_MOD)
#define AC_FN4  ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)
#define AC_FN5  ACTION_FUNCTION(AF_EEPROM_RESET)
#else
const action_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPC),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
    [4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS),
    [5] = ACTION_FUNCTION(AF_EEPROM_RESET),
};
#endif

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]=LAYOUT_default(
        AM(GESC),    AM(1),     AM(2), AM(3), AM(4), AM(5), AM(6), AM(7),    AM(8),   AM(9),    AM(0), AM(MINS),  AM(EQL), AM(BSLS), AM(GRV),
        AM(TAB),     AM(Q),     AM(W), AM(E), AM(R), AM(T), AM(Y), AM(U),    AM(I),   AM(O),    AM(P), AM(LBRC), AM(RBRC), AM(BSPC),
        AM(FN4),    AM(A),     AM(S), AM(D), AM(F), AM(G), AM(H), AM(J),    AM(K),   AM(L), AM(SCLN), AM(QUOT),           AM(ENT),
        AM(LSFT),    AM(Z),     AM(X), AM(C), AM(V), AM(B), AM(N), AM(M), AM(COMM), AM(DOT), AM(SLSH),          AM(RSFT),  AM(FN0),
                 AM(LGUI),  AM(LALT),                 AM(FN1),                                          AM(RALT),  AM(RCTL)),
    [1]=LAYOUT_default(
        _______,   AM(F1),   AM(F3),   AM(F3),   AM(F4),   AM(F5),   AM(F6),   AM(F7),   AM(F8),   AM(F9),  AM(F10),  AM(F11),  AM(F12), _______, AM(PSCR),
        AM(BTLD),  AM(FN2),  AM(FN3),  AM(FN5), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        AM(CAPS), AM(VOLU), AM(VOLD), AM(CALC), _______, _______, AM(LEFT), AM(DOWN),   AM(UP),AM(RIGHT), _______, _______,          _______,
        _______,  AM(F21),  AM(F22),  AM(F23),  AM(F24), _______, _______, _______, _______, _______, _______,          _______, _______,
                 _______, _______,                   _______,                                                       _______,_______),
};
