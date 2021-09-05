/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_keymap.h
 */

#pragma once

#include <stdint.h>


void amk_keymap_init(void);
void amk_keymap_reset(void);
uint8_t amk_keymap_get_layer_count(void);
void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode);
uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col);

void amk_keymap_get_buffer(uint16_t offset, uint16_t size, uint8_t *data);
void amk_keymap_set_buffer(uint16_t offset, uint16_t size, uint8_t *data);

uint8_t amk_keymap_macro_get_count(void);
uint16_t amk_keymap_macro_get_buffer_size(void);
void amk_keymap_macro_get_buffer(uint16_t offset, uint16_t size, uint8_t *data);
void amk_keymap_macro_set_buffer(uint16_t offset, uint16_t size, uint8_t *data);
void amk_keymap_macro_reset(void);
