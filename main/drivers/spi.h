/**
 * spi.h
 * spi interface
 */

#pragma once

#include <stdbool.h>
#include "amk_error.h"

void spi_init(void);
bool spi_ready(void);
amk_i2c_error_t spi_send(const void *data, size_t length, size_t timeout);
amk_i2c_error_t spi_recv(void* data, size_t length, size_t timeout);
amk_i2c_error_t spi_xfer(const void *tx_bufer, void *rx_bufer, size_t length, size_t timeout);
void spi_uninit(void);