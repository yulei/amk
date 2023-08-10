/**
  * @file des_60.h
  *
  */
#pragma once

#include "quantum.h"

#define LAYOUT_default( \
    K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, \
    K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, \
    K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312,       K313, \
    K401,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411, K412, K413, \
          K502, K503,                   K506,                   K508, K509 \
) {\
    {K301, K101, K103, K304, K105, K305, K108, K306, K311, K208, K309, K109, K112, K115, K312}, \
    {K302, K102, K203, K402, K106, K403, K206, K406, K410, K209, K310, K110, K113, K213, K313}, \
    {K303, K201, K204, K503, K107, K404, K207, K407, K411, K210, K408, K111, K114, K214, K412}, \
    {K401, K202, K104, K502, K205, K405, K307, K506, K508, K308, K409, K211, K212, K413, K509}, \
}