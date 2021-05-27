/**
 * eeprom_manager.h
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "eeconfig.h"

#define EECONFIG_KEYBOARD           (uint32_t*)16
#define EECONFIG_KEYMAP_MAGIC       (uint32_t*)20
#define EECONFIG_RGB                (uint8_t*)24        // 6 bytes
#define EECONFIG_RGB_MATRIX         (uint8_t*)30        // 6 bytes
#define EECONFIG_LAYOUT_OPTIONS     (uint8_t*)36        // 1 bytes
#define EECONFIG_DEVICE             (uint8_t*)37        // 1 bytes

#define EECONFIG_SIZE               64
#define EEPROM_SIZE                 2048

uint32_t eeconfig_read_kb(void);
void eeconfig_write_kb(uint32_t data);
void eeconfig_update_kb(uint32_t data);

void eeconfig_read_rgb(void* rgb);
void eeconfig_write_rgb(const void* rgb);
void eeconfig_update_rgb(const void* rgb);

void eeconfig_read_rgb_matrix(void* rgb);
void eeconfig_write_rgb_matrix(const void* rgb);
void eeconfig_update_rgb_matrix(const void* rgb);

uint8_t eeconfig_read_layout_options(void);
void eeconfig_write_layout_options(uint8_t);
void eeconfig_update_layout_options(uint8_t);

uint8_t eeconfig_read_device(void);
void eeconfig_write_device(uint8_t);
void eeconfig_update_device(uint8_t);

// wrapper functions
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

#define EEKEYMAP_START_ADDR         EECONFIG_SIZE
#define EEKEYMAP_SIZE               (EEPROM_SIZE-EECONFIG_SIZE)

bool ee_keymap_is_valid(void);
void ee_keymap_set_valid(bool valid);
void ee_keymap_write_key(uint8_t layer, uint8_t row, uint8_t col, uint16_t key);
uint16_t ee_keymap_read_key(uint8_t layer, uint8_t row, uint8_t col);
void ee_keymap_write_buffer(uint16_t offset, uint16_t size, uint8_t *data);
void ee_keymap_read_buffer(uint16_t offset, uint16_t size, uint8_t *data);