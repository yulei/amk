/**
 * @file amk_keymap.h
 */

#pragma once

#include <stdint.h>

#define AMK_KEYMAP_MAX_LAYER 4

void amk_keymap_init(void);
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode);
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col);
