/*
 * pad_ec_gcmap.c
 */

#include "pad_ec.h"

#define GC_FN0  ACTION_LAYER_MOMENTARY(1)

GC_KEYMAPS = {
    [0] = LAYOUT_default(
         GC_FN0,  GC_ESC, GC_PSCR, GC_BSPC,
        GC_NLCK, GC_PSLS, GC_PAST, GC_PMNS,
          GC_P7,   GC_P8,   GC_P9,
          GC_P4,   GC_P5,   GC_P6, GC_PPLS,
          GC_P1,   GC_P2,   GC_P3, 
          GC_P0,          GC_PDOT, GC_PENT),

    [1] = LAYOUT_default(
        GC_BTLD, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, 
        _______, _______, _______, _______,
        _______, _______, _______, 
        _______,          _______, _______),
};
