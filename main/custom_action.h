/**
 * custom_action.h
 *      action function defintion for custom behavior
 */

#pragma once

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
