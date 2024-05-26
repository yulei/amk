/**
  * @file navi_8k.h
  * @author astro
  *
  * @copyright Copyright (c) 2024
  */
#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, K116, K117,\
    K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, \
    K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312,       K313, \
    K401,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K412,       K416, \
    K501, K502, K503,                   K506,                         K507, K508, K510, K515, K516, K517 \
) { \
    {K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, K116}, \
    {K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, K117}, \
    {K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K313,KC_NO,KC_NO,KC_NO}, \
    {K401,KC_NO, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411, K412,KC_NO, K416, K517}, \
    {K501, K502, K503,KC_NO,KC_NO,KC_NO, K506,KC_NO,KC_NO,KC_NO,KC_NO, K507, K508, K510, K515, K516}  \
}
