/**
 * @file amk_keymap.c
 */

#include <string.h>

#include "amk_keymap.h"
#include "amk_eeprom.h"

#include "action.h"
#include "keymap.h"
#include "amk_printf.h"

#ifdef KEYMAP_CONFIG_ENABLE 

#ifndef ACTIONMAP_ENABLE
#error ACTIONMAP must be enabled with webconfig
#endif

static uint16_t amk_keymaps[AMK_KEYMAP_MAX_LAYER][MATRIX_ROWS][MATRIX_COLS];
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

__attribute__((weak))
void amk_keymap_init(void)
{
    memset(amk_keymaps, 0, sizeof(amk_keymaps));
    if (ee_keymap_is_valid()) {
        for(int layer = 0; layer < AMK_KEYMAP_MAX_LAYER; layer++) {
            for (int row = 0; row < MATRIX_ROWS; row++) {
                for (int col = 0; col < MATRIX_COLS; col++) {
                    amk_keymaps[layer][row][col] = ee_keymap_read_key(layer, row, col);
                }
            }
        }
    } else {
        for(int layer = 0; layer < AMK_KEYMAP_MAX_LAYER; layer++) {
            for (int row = 0; row < MATRIX_ROWS; row++) {
                for (int col = 0; col < MATRIX_COLS; col++) {
                    amk_keymaps[layer][row][col] = actionmaps[layer][row][col].code;
                    ee_keymap_write_key(layer, row, col, amk_keymaps[layer][row][col]);
                }
            }
        }
        ee_keymap_set_valid(true);
    }

    amk_printf("amk_keymap_init finished\n");
}

__attribute__((weak))
void amk_keymap_reset(void)
{
    memset(amk_keymaps, 0, sizeof(amk_keymaps));
    for(int layer = 0; layer < AMK_KEYMAP_MAX_LAYER; layer++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                amk_keymaps[layer][row][col] = actionmaps[layer][row][col].code;
                ee_keymap_write_key(layer, row, col, amk_keymaps[layer][row][col]);
            }
        }
    }
    ee_keymap_set_valid(true);
    amk_printf("amk_keymap_reset finished\n");
}

__attribute__((weak))
uint8_t amk_keymap_get_layer_count(void)
{
    return AMK_KEYMAP_MAX_LAYER;
}

__attribute__((weak))
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode)
{
    if (amk_keymaps[layer][row][col] == keycode) return;

    amk_printf("amk_keymap_set: layer=%d, row=%d, col=%d, key=0x%x\n", layer, row, col, keycode);
    amk_keymaps[layer][row][col] = keycode;
    ee_keymap_write_key(layer, row, col, amk_keymaps[layer][row][col]);
}

__attribute__((weak))
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col)
{
    amk_printf("amk_keymap_get: layer=%d, row=%d, col=%d, key=0x%x\n", layer, row, col, amk_keymaps[layer][row][col]);
    return amk_keymaps[layer][row][col];
}

__attribute__((weak))
void amk_keymap_get_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    ee_keymap_read_buffer(offset, size, data);
}

__attribute__((weak))
void amk_keymap_set_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{
    ee_keymap_write_buffer(offset, size, data);
}

#else
__attribute__((weak))
void amk_keymap_init(void)
{
    amk_printf("amk_keymap_init in disabled mode\n");
}

__attribute__((weak))
void amk_keymap_reset(void)
{
    amk_printf("amk_keymap_reset in disabled mode\n");
}

__attribute__((weak))
uint8_t amk_keymap_get_layer_count(void) { return 0; }

__attribute__((weak))
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode) {}

__attribute__((weak))
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col) { return 0; }

__attribute__((weak))
void amk_keymap_get_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{}

__attribute__((weak))
void amk_keymap_set_buffer(uint16_t offset, uint16_t size, uint8_t *data)
{}

#endif