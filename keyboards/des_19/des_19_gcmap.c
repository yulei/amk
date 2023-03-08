/**
 * @file des_19_gcmap.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "des_19.h"

#ifdef VIAL_ENABLE
#define GC_FN0  ACTION_LAYER_MOMENTARY(1)
#define GC_FN1  ACTION_LAYER_TAP_KEY(1, KC_SPC)
#define GC_FN2  ACTION_FUNCTION(AF_RGB_TOG)
#define GC_FN3  ACTION_FUNCTION(AF_RGB_MOD)
#define GC_FN4  ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS)
#define GC_FN5  ACTION_FUNCTION(AF_EEPROM_RESET)
#else
#define GC_FN0  KC_FN0
#define GC_FN1  KC_FN1
#define GC_FN2  KC_FN2
#define GC_FN3  KC_FN3
#define GC_FN4  KC_FN4
#define GC_FN5  KC_FN5

const action_t fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPC),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
    [4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS),
    [5] = ACTION_FUNCTION(AF_EEPROM_RESET),
};
#endif

GC_KEYMAPS = {
    [0] = LAYOUT_default(
        GC_NLCK, GC_PSLS, GC_PAST, GC_PEQL,
          GC_P7,   GC_P8,   GC_P9, GC_PMNS,
          GC_P4,   GC_P5,   GC_P6, GC_PPLS,
          GC_P1,   GC_P2,   GC_P3, GC_PENT,
         GC_FN0,   GC_P0, GC_PDOT),

    [1] = LAYOUT_default(
        _______, _______, _______, _______,
        _______, _______, _______, _______, 
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______),
};
