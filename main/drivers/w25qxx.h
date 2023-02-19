/**
 * @file w25qxx.h
 * @brief for accessing w25q serials flash chip
 */

#pragma once

#include "amk_error.h"
#include "amk_gpio.h"
#include "spi.h"

typedef enum {
    W25Q128,
} w25qxx_type_t;

typedef struct {
    pin_t cs;
    spi_handle_t spi;
} w25qxx_config_t;

typedef struct {
    w25qxx_type_t type;
    uint32_t page_size;
    uint32_t page_count;
    uint32_t sector_size;
    uint32_t sector_count;
    w25qxx_config_t config;
} w25qxx_t;

w25qxx_t *w25qxx_init(w25qxx_config_t *config);
w25qxx_t *w25qxx_init_128(w25qxx_config_t *config);
w25qxx_t *w25qxx_current(void);
// to simplify the implementation, only support sector write
amk_error_t w25qxx_write_sector(w25qxx_t* w25qxx, uint32_t address, const uint8_t *data, uint32_t size);

amk_error_t w25qxx_read_sector(w25qxx_t* w25qxx, uint32_t address, uint8_t *data, uint32_t size);
amk_error_t w25qxx_read_bytes(w25qxx_t* w25qxx, uint32_t address, uint8_t *data, uint32_t size);

amk_error_t w25qxx_erase_chip(w25qxx_t* w25qxx);