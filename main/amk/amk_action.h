/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */


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
    AF_KC_LSPO,
    AF_KC_RSPC,
    AF_KC_LCPO,
    AF_KC_RCPC,
    AF_KC_LAPO,
    AF_KC_RAPC,
// via keycodes
    AF_FN_MO13,
    AF_FN_MO23,
    AF_MACRO00,
    AF_MACRO01,
    AF_MACRO02,
    AF_MACRO03,
    AF_MACRO04,
    AF_MACRO05,
    AF_MACRO06,
    AF_MACRO07,
    AF_MACRO08,
    AF_MACRO09,
    AF_MACRO10,
    AF_MACRO11,
    AF_MACRO12,
    AF_MACRO13,
    AF_MACRO14,
    AF_MACRO15,
} function_id_t;

// try to use 0xE8-0xEF
#define KC_CTMB     0xE8 
#define KC_MEH      KC_CTMB
#define KC_HYPER    0xE9
#define KC_GESC     0xEA 
#define KC_SFTENT   0xEB
#define KC_CTME     0xEC