/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_keymap.c
 */

#include <string.h>
#include "generic_hal.h"

#include "amk_keymap.h"
#include "amk_eeprom.h"

#include "action.h"
#include "keymap.h"
#include "amk_printf.h"

#ifndef AMK_KEYMAP_DEBUG
#define AMK_KEYMAP_DEBUG 0
#endif

#if AMK_KEYMAP_DEBUG
#define keymap_debug  amk_printf
#else
#define keymap_debug(...)
#endif

#ifdef KEYMAP_CONFIG_ENABLE 

#ifndef ACTIONMAP_ENABLE
#error ACTIONMAP must be enabled with online keymap config 
#endif

extern const action_t actionmaps[][MATRIX_ROWS][MATRIX_COLS];

action_t action_for_key(uint8_t layer, keypos_t key)
{
    return (action_t)amk_keymap_get(layer, key.row, key.col);
}

/* Macro */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    return MACRO_NONE;
}

__WEAK
void amk_keymap_init(void)
{
    if (!ee_keymap_is_valid()) {
        amk_keymap_reset();
    }

    if (!ee_macro_is_valid()) {
        amk_keymap_macro_reset();
    }

    keymap_debug("amk_keymap_init finished\n");
}

__WEAK
void amk_keymap_reset(void)
{
    for(int layer = 0; layer < EEKEYMAP_MAX_LAYER; layer++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                ee_keymap_write_key(layer, row, col, actionmaps[layer][row][col].code);
            }
        }
    }
    ee_keymap_set_valid(true);
    keymap_debug("amk_keymap_reset finished\n");
}

__WEAK
uint8_t amk_keymap_get_layer_count(void)
{
    return EEKEYMAP_MAX_LAYER;
}

__WEAK
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    uint16_t amk_code = amk_keymap_get(layer, row, col);

    if (amk_code == keycode) return;

    keymap_debug("amk_keymap_set: layer=%d, row=%d, col=%d, key=0x%x\n", layer, row, col, keycode);
    ee_keymap_write_key(layer, row, col, keycode);
}

__WEAK
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col)
{
    uint16_t amk_code = ee_keymap_read_key(layer, row, col);
    keymap_debug("amk_keymap_get: layer=%d, row=%d, col=%d, key=0x%x\n", layer, row, col, amk_code);
    return amk_code;
}

__WEAK
void amk_keymap_get_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    ee_keymap_read_buffer(offset, size, data);
}

__WEAK
void amk_keymap_set_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    ee_keymap_write_buffer(offset, size, data);
}

__WEAK
uint8_t amk_keymap_macro_get_count(void)
{
    return EEKEYMAP_MACRO_COUNT;
}

__WEAK
uint16_t amk_keymap_macro_get_buffer_size(void)
{
    return EEKEYMAP_MACRO_SIZE;
}

__WEAK
void amk_keymap_macro_get_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    ee_macro_read_buffer(offset, size, data);
}

__WEAK
void amk_keymap_macro_set_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    ee_macro_write_buffer(offset, size, data);
}

__WEAK
void amk_keymap_macro_reset(void)
{
    ee_macro_reset();
    ee_macro_set_valid(true);
    keymap_debug("amk_macro_reset finished\n");
}

#else

__WEAK
void amk_keymap_init(void)
{
    keymap_debug("amk_keymap_init in disabled mode\n");
}

__WEAK
void amk_keymap_reset(void)
{
    keymap_debug("amk_keymap_reset in disabled mode\n");
}

__WEAK
uint8_t amk_keymap_get_layer_count(void) { return 0; }

__WEAK
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode) {}

__WEAK
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col) { return 0; }

__WEAK
void amk_keymap_get_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{}

__WEAK
void amk_keymap_set_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{}

__WEAK
uint8_t  amk_keymap_macro_get_count(void)
{
    return 0;
}

__WEAK
uint16_t amk_keymap_macro_get_buffer_size(void)
{
    return 0;
}

__WEAK
void     amk_keymap_macro_get_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{}

__WEAK
void     amk_keymap_macro_set_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{}

__WEAK
void     amk_keymap_macro_reset(void)
{}

#endif