/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_error.h
 * @brief error codes defintion
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint32_t amk_error_t;

#define AMK_SUCCESS     0
#define AMK_ERROR       1

#define AMK_I2C_ERROR_START     0x1000
#define AMK_SPI_ERROR_START     0x1100

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