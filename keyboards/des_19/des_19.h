/**
 * @file des_19.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    K101, K102, K103, K104, \
    K201, K202, K203, K204, \
    K301, K302, K303, K304, \
    K401, K402, K403, K404, \
    K501, K502, K503 \
) {\
    { K204, K302, K202, K401, K101}, \
    { K303, K402, K203, K501, K102}, \
    { K304, K403, K103, K502, K201}, \
    {KC_NO, K404, K104, K503, K301}, \
}
