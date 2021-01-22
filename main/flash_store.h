/**
 * @file flash_store.h
 */

#pragma once

#include <stdint.h>

void flash_store_write(uint8_t layer, const void* data, size_t size);
size_t flash_store_read(uint8_t layer, void* data, size_t size);

void flash_store_write_key(uint8_t layer, uint8_t row, uint8_t col, uint16_t key);
uint16_t flash_store_read_key(uint8_t layer, uint8_t row, uint8_t col);