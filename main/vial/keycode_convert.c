/**
 * @file keycode_convert.c
 */

#include "keycode_convert.h"

// Can't use SAFE_RANGE here, it might change if someone adds
// new values to enum quantum_keycodes.
// Need to keep checking 0x5F10 is still in the safe range.
// TODO: merge this into quantum_keycodes
// Backlight keycodes are in range 0x5F00-0x5F0F
enum via_keycodes {
    FN_MO13 = 0x5F10,
    FN_MO23,
    MACRO00,
    MACRO01,
    MACRO02,
    MACRO03,
    MACRO04,
    MACRO05,
    MACRO06,
    MACRO07,
    MACRO08,
    MACRO09,
    MACRO10,
    MACRO11,
    MACRO12,
    MACRO13,
    MACRO14,
    MACRO15,
};

enum user_keycodes {
    USER00 = 0x5F80,
    USER01,
    USER02,
    USER03,
    USER04,
    USER05,
    USER06,
    USER07,
    USER08,
    USER09,
    USER10,
    USER11,
    USER12,
    USER13,
    USER14,
    USER15,
};

uint16_t amk_to_vial(uint16_t keycode)
{
    return keycode;
}

uint16_t vial_to_amk(uint16_t keycode)
{
    return keycode;
}