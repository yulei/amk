/*
 * pad_gcmap.c
 */

#include "pad.h"

#define GC_FN0	ACTION_LAYER_MOMENTARY(1)
#define GC_FN1  ACTION_FUNCTION(AF_RGB_TOG)
#define GC_FN2	ACTION_FUNCTION(AF_RGB_MOD)

GC_KEYMAPS = {
    [0] = LAYOUT_default(
         GC_ESC,   GC_FN0, GC_PSCR, GC_BSPC,
        GC_NLCK, GC_PSLS, GC_PAST, GC_PMNS,
          GC_P7,   GC_P8,   GC_P9, GC_PPLS,
          GC_P4,   GC_P5,   GC_P6, GC_PPLS,
          GC_P1,   GC_P2,   GC_P3, GC_PENT,
          GC_P0,   GC_P0, GC_PDOT, GC_PENT),
    [1] = LAYOUT_default(
        GC_BTLD, _______, _______, _______,
         GC_FN1,  GC_FN2,  GC_F13, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______),
};
