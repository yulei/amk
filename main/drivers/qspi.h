/**
 * @file qspi.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "amk_error.h"

bool qspi_init(uint32_t map_addr);

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length);
amk_error_t qspi_write_sector(uint32_t address, const uint8_t* buffer, size_t length);

amk_error_t qspi_erase_chip(void);

void qspi_uninit(void);
