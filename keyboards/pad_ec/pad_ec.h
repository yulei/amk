/**
 * @file pad_ec.h
 */

#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    k102, k101, k103, k104, \
	k201, k202, k203, k204, \
	k301, k302, k303, k404, \
	k401, k402, k403, \
	k501, k502, k503, k604, \
	k601,       k603 \
) { \
    { k101, k201, k301, k401, k501}, \
    { k103, k202, k302, k402, k601}, \
    { k104, k203, k403, k502, k603}, \
    { k204, k303, k404, k503, k604}, \
    { k102,KC_NO,KC_NO,KC_NO,KC_NO}, \
}
