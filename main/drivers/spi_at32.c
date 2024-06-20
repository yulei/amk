/**
 * @file spi_at32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "spi.h"
#include "amk_hal.h"
#include "amk_printf.h"

#ifndef SPI_DEBUG
#define SPI_DEBUG 1
#endif

#if SPI_DEBUG
#define spi_debug  amk_printf
#else
#define spi_debug(...)
#endif

#define TIMEOUT_DEFAULT 100

#ifdef USE_SPI1
extern spi_type* hspi1;
#endif

#ifdef USE_SPI2
#error "SPI2 not supported"
#endif

#ifdef USE_SPI3
#error "SPI3 not supported"
#endif

spi_handle_t ak_spi_init(spi_instance_t inst)
{
#ifdef USE_SPI1
    if (inst == SPI_INSTANCE_1) return hspi1;
#endif

    return NULL;
}

static uint8_t spi_write_byte(spi_type* spi, uint8_t data)
{
     while(spi_i2s_flag_get(spi, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(spi, data);

    while(spi_i2s_flag_get(spi, SPI_I2S_RDBF_FLAG) == RESET);
    return  spi_i2s_data_receive(spi);
}

bool ak_spi_ready(spi_handle_t spi)
{
    return true;
}

amk_error_t ak_spi_send_async(spi_handle_t spi, const void *data, size_t length)
{
    return ak_spi_send(spi, data, length);
}

amk_error_t ak_spi_recv_async(spi_handle_t spi, void* data, size_t length)
{
    return AMK_SPI_ERROR;
}

amk_error_t ak_spi_xfer_async(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    return AMK_SPI_ERROR;
}

amk_error_t ak_spi_send(spi_handle_t spi, const void *data, size_t length)
{
    spi_type* hspi = (spi_type*)spi;
    const uint8_t* cur = (const uint8_t*)data;
    while(length--) {
        spi_write_byte(hspi, *cur++);
    }

    return AMK_SUCCESS;
}

amk_error_t ak_spi_recv(spi_handle_t spi, void* data, size_t length)
{
    spi_type* hspi = (spi_type*)spi;
    uint8_t* cur = (uint8_t*)data;
    while(length--) {
        *cur = spi_write_byte(hspi, *cur);
        cur++;
    }

    return AMK_SUCCESS;
}

amk_error_t ak_spi_xfer(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    spi_type* hspi = (spi_type*)spi;
    const uint8_t* tx_cur = (const uint8_t*)tx_buffer;
    uint8_t* rx_cur = (uint8_t*)rx_buffer;

    while(length--) {
        *rx_cur++ = spi_write_byte(hspi, *tx_cur++);
    }

    return AMK_SUCCESS;
}

void ak_spi_uninit(spi_handle_t spi)
{}
