/**
 * @file amk_error.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint32_t amk_error_t;

#define AMK_SUCCESS     0
#define AMK_ERROR       1

#define AMK_I2C_ERROR_START     0x1000
#define AMK_SPI_ERROR_START     0x1100
#define AMK_QSPI_ERROR_START    0x1200

enum {
    AMK_I2C_TIMEOUT = AMK_I2C_ERROR_START,
    AMK_I2C_ERROR,
    AMK_I2C_BUSY,
};

enum {
    AMK_SPI_TIMEOUT = AMK_SPI_ERROR_START,
    AMK_SPI_ERROR,
    AMK_SPI_BUSY,
};

enum {
    AMK_QSPI_TIMEOUT = AMK_QSPI_ERROR_START,
    AMK_QSPI_ERROR,
    AMK_QSPI_BUSY,
    AMK_QSPI_INVALID_PARAM,
};
