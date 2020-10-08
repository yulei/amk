/**
 * @file flash_store.h
 */

#pragma once

#include <stdint.h>

void flash_store_write(uint8_t key, const void* data, size_t size);
size_t flash_store_read(uint8_t key, void* data, size_t size);