/**
 * @file amk_snaptap.h
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "matrix.h"

#ifndef AMK_SNAPTAP_COUNT
#define AMK_SNAPTAP_COUNT   8
#endif

typedef struct
{
    uint16_t col;
    uint16_t row;
} snaptap_key_t;

enum snaptap_mode
{
    SNAPTAP_NONE,       // snaptap disabled
    SNAPTAP_DEPTH,      // activates the key pressed more and releae other
    SNAPTAP_TRIGGER,    // activates the key last triggered
    SNAPTAP_FIRST,      // first key takes the priority
    SNAPTAP_SECOND,     // second key takes the priority
    SNAPTAP_BOTH,       // released both if both pressed
    SNAPTAP_MAX,        // maximum mode of snaptap
};

typedef struct
{
    snaptap_key_t first;
    snaptap_key_t second;

    enum snaptap_mode mode;
}snaptap_pair_t;

void snaptap_init(void);
bool snaptap_process(matrix_row_t *raw);
void snaptap_update(uint8_t index, const snaptap_pair_t* snaptap);
void snaptap_update_config(uint8_t config);
void snaptap_toggle(void);

