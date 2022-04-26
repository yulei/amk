/*
 * pad_ec_actionmap.c
 */

#include "pad_ec.h"

#define AC_FN0  ACTION_LAYER_MOMENTARY(1)

const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
         AC_ESC,  AC_FN0, AC_PSCR, AC_BSPC,
        AC_NLCK, AC_PSLS, AC_PAST, AC_PMNS,
          AC_P7,   AC_P8,   AC_P9,
          AC_P4,   AC_P5,   AC_P6, AC_PPLS,
          AC_P1,   AC_P2,   AC_P3, 
          AC_P0,          AC_PDOT, AC_PENT),

    [1] = LAYOUT_default(
        AC_BTLD, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, 
        _______, _______, _______, _______,
        _______, _______, _______, 
        _______,          _______, _______),
};
