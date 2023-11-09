/**
 * @file 8xv2_hs.h
 */

#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    K601,       K602, K603, K604, K605, K606, K607, K608, K609, K610, K611, K612, K613,          K614, K615, K616, \
  \
    K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114,          K115, K116, K117, \
    K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214,          K215, K216, K217, \
    K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312,       K314, \
    K401,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411,       K412,                K415, \
    K501, K502, K503,                   K506,                   K511, K512, K513,                K514, K515, K516 \
) { \
    {  K610, K609, K608,K607, K606, K605, K604, K603, K602,K601, K616, K615, K614, K613, K612, K611}, \
    {  K110, K109, K108,K107, K106, K105, K104, K103, K102,K101, K116, K115, K114, K113, K112, K111}, \
    {  K210, K209, K208,K207, K206, K205, K204, K203, K202,K201, K117, K215, K214, K213, K212, K211}, \
    {  K310, K309, K308,K307, K306, K305, K304, K303, K302,K301, K217, K216, K314,KC_NO, K312, K311}, \
    {  K409, K408, K407,K406, K405, K404, K403, K402,KC_NO,K401,KC_NO, K415,KC_NO, K412, K411, K410}, \
    { KC_NO,KC_NO,KC_NO,K506,KC_NO,KC_NO,KC_NO, K503, K502,K501, K516, K515, K514, K513, K512, K511}  \
}