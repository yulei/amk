/**
 * custom_action.h
 *      action function defintion for custom behavior
 */

#pragma once

#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "action_code.h"

typedef enum {
    AF_NONE,
    AF_RGB_TOG,
    AF_RGB_MOD,
    AF_RGB_HUEI,
    AF_RGB_HUED,
    AF_RGB_SATI,
    AF_RGB_SATD,
    AF_RGB_VALI,
    AF_RGB_VALD,
    AF_EEPROM_RESET,
} function_id_t;

// try to use KC_FN24-KC_FN31
#define KC_MEH      KC_FN22
#define KC_HYPER    KC_FN23
#define KC_GESC     KC_FN24
#define KC_LCPO     KC_FN25
#define KC_RCPC     KC_FN26
#define KC_LSPO     KC_FN27
#define KC_RSPC     KC_FN28
#define KC_LAPO     KC_FN29
#define KC_RAPC     KC_FN30
#define KC_SFTENT   KC_FN31