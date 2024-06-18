/**
 * @file sd_nand.h
 * @brief nand with sdio interface
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "amk_hal.h"
#include "amk_error.h"

#define SD_NAND_BLOCK_SIZE  BLOCKSIZE

bool sd_nand_init(void);

uint32_t sd_nand_get_block_count(void);
uint32_t sd_nand_get_block_size(void);

amk_error_t sd_nand_read_blocks(uint32_t address, uint8_t *buffer, size_t block_count);
amk_error_t sd_nand_write_blocks(uint32_t address, const uint8_t* buffer, size_t block_count);

amk_error_t sd_nand_erase_chip(void);

void sd_nand_uninit(void);