/**
 * @file corsa_ec.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112,       K113, K114, \
    K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, \
    K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312,       K313, K314, \
    K401,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411, K412, K413, K414, \
    K501, K502, K503,                   K506,                   K507, K508, K512, K513, K514  \
) { \
    {K301, K101, K102, K105, K107,KC_NO,KC_NO, K108, K110, K112,KC_NO, K214, K114, K507}, \
    {K401, K201, K103, K106, K205, K207, K208, K109, K111, K113, K212, K313, K215, K508}, \
    {K501, K202, K104, K204, K206, K306, K307, K209, K210, K211, K213, K410, K314, K512}, \
    {K502, K302, K203, K304, K305, K405, K407, K308, K309, K310, K312, K411, K413, K513}, \
    {K503, K402, K303, K403, K404, K406, K506, K408, K409,KC_NO, K311, K412, K414, K514}  \
}