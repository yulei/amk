/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_eeprom.h
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "eeconfig.h"

#define EECONFIG_KEYBOARD           (uint32_t*)8
#define EECONFIG_KEYMAP_MAGIC       (uint32_t*)12
#define EECONFIG_RGB                (uint8_t*)16        // 6 * 8 bytes
#define EECONFIG_LAYOUT_OPTIONS     (uint8_t*)62        // 1 bytes
#define EECONFIG_DEVICE             (uint8_t*)63        // 1 bytes

#define EECONFIG_SIZE               64
#define EEPROM_SIZE                 2048

// eeconfig setting
uint32_t eeconfig_read_kb(void);
void eeconfig_write_kb(uint32_t data);
void eeconfig_update_kb(uint32_t data);

void eeconfig_read_rgb(void* rgb, uint8_t index);
void eeconfig_write_rgb(const void* rgb, uint8_t index);
void eeconfig_update_rgb(const void* rgb, uint8_t index);

uint8_t eeconfig_read_layout_options(void);
void eeconfig_write_layout_options(uint8_t);
void eeconfig_update_layout_options(uint8_t);

uint8_t eeconfig_read_device(void);
void eeconfig_write_device(uint8_t);
void eeconfig_update_device(uint8_t);

// eeprom read/write functions
uint8_t eeprom_read_byte(const uint8_t *Address);
void eeprom_write_byte(uint8_t *Address, uint8_t Value);
void eeprom_update_byte(uint8_t *Address, uint8_t Value);

uint16_t eeprom_read_word(const uint16_t *Address);
void eeprom_write_word(uint16_t *Address, uint16_t Value);
void eeprom_update_word(uint16_t *Address, uint16_t Value);

uint32_t eeprom_read_dword(const uint32_t *Address);
void eeprom_write_dword(uint32_t *Address, uint32_t Value);
void eeprom_update_dword(uint32_t *Address, uint32_t Value);

void eeprom_read_block(void *buf, const void *addr, size_t len);
void eeprom_write_block(const void *buf, void *addr, size_t len);
void eeprom_update_block(const void *buf, void *addr, size_t len);


//************************************
// eeprom keymap
//************************************
#define EEKEYMAP_START_ADDR         EECONFIG_SIZE
#ifndef EEKEYMAP_MAX_LAYER
#define EEKEYMAP_MAX_LAYER          4
#endif
#define EEKEYMAP_SIZE               (MATRIX_COLS*MATRIX_ROWS*EEKEYMAP_MAX_LAYER)

bool ee_keymap_is_valid(void);
void ee_keymap_set_valid(bool valid);
void ee_keymap_write_key(uint8_t layer, uint8_t row, uint8_t col, uint16_t key);
uint16_t ee_keymap_read_key(uint8_t layer, uint8_t row, uint8_t col);
void ee_keymap_write_buffer(uint16_t offset, uint16_t size, uint8_t *data);
void ee_keymap_read_buffer(uint16_t offset, uint16_t size, uint8_t *data);