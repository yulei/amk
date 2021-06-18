/**
 * @file amk_action.h
 * @brief action function defintion for custom behavior
 */

#pragma once

#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"

typedef enum {
    AF_NONE,
    AF_EEPROM_RESET,
    AF_RGB_TOG,
    AF_RGB_MOD,
    AF_RGB_HUEI,
    AF_RGB_HUED,
    AF_RGB_SATI,
    AF_RGB_SATD,
    AF_RGB_VALI,
    AF_RGB_VALD,
    AF_RGB_SPDI,
    AF_RGB_SPDD,
} function_id_t;

// try to use 0xE8-0xEF
#define KC_CTMB     0xE8 
#define KC_MEH      KC_CTMB
#define KC_HYPER    0xE9
#define KC_GESC     0xEA 
#define KC_SFTENT   0xEB
#define KC_CTME     0xEB