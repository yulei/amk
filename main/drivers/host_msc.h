/**
 * @file host_msc.h
 * @brief host msc driver interface
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "amk_hal.h"
#include "amk_error.h"

bool host_msc_init(void);

uint32_t host_msc_get_block_count(void);
uint32_t host_msc_get_block_size(void);

amk_error_t host_msc_read_blocks(uint32_t block_no, uint8_t *buffer, size_t block_count);
amk_error_t host_msc_write_blocks(uint32_t block_no, const uint8_t* buffer, size_t block_count);

amk_error_t host_msc_erase_chip(void);

void host_msc_uninit(void);