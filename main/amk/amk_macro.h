/**
 * @file amk_macro.h
 */

#pragma once

#include <stdint.h>

typedef struct {
    uint16_t     id;
    uint16_t     offset;
    uint32_t     delay;
} report_macro_t;

void amk_macro_play(uint8_t id);