/**
 * @file w25qxx.h
 *  for accessing w25q serials flash chip
 */

#pragma once

#include "amk_error.h"
#include "gpio_pin.h"
#include "spi.h"

typedef enum {
    W25Q128,
} w25qxx_type_t;

typedef struct {
    pin_t cs;
    spi_handle_t *spi;
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
amk_error_t w25qxx_write_byte(w25qxx_t* w25qxx, uint8_t data);
amk_error_t w25qxx_read_byte(w25qxx_t* w25qxx, uint8_t *data);
