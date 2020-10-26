/**
 * spi.h
 * spi interface
 */

#pragma once

#include <stdbool.h>
#include "amk_error.h"

typedef void *spi_handle_t;

spi_handle_t spi_init(void);
bool spi_ready(spi_handle_t spi);
amk_error_t spi_send(spi_handle_t spi, const void *data, size_t length);
amk_error_t spi_recv(spi_handle_t spi, void* data, size_t length);
amk_error_t spi_xfer(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length);
void spi_uninit(spi_handle_t spi);