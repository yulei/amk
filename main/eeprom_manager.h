/**
 * eeprom_manager.h
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "eeconfig.h"

#define EECONFIG_ADDRESS(base, offset) ((void*)((uint8_t*)(base)+(offset)))
#define EECONFIG_RGB_EFFECTS (void*)16
#define EECONFIG_RGB_MATRIX (void*)24

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
