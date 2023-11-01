/**
 * @file spi_gd32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
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

#ifdef USE_SPI0
extern uint32_t hspi0;
#endif

#ifdef USE_SPI1
extern uint32_t hspi1;
#endif

#ifdef USE_SPI2
#error "SPI2 not supported"
#endif

#ifdef USE_SPI3
#error "SPI3 not supported"
#endif

spi_handle_t spi_init(spi_instance_t inst)
{
#ifdef USE_SPI0
    if (inst == SPI_INSTANCE_0) return &hspi0;
#endif

#ifdef USE_SPI1
    if (inst == SPI_INSTANCE_1) return &hspi1;
#endif

    return NULL;
}

static uint8_t spi_write_byte(uint32_t spi, uint8_t data)
{
    while (RESET == spi_i2s_flag_get(spi,SPI_FLAG_TBE));

    spi_i2s_data_transmit(spi,data);

    while(RESET == spi_i2s_flag_get(spi,SPI_FLAG_RBNE));

    return(spi_i2s_data_receive(spi));
}

static void spi_dma_tx(uint32_t spi, uint32_t dma, uint32_t channel, const void* tx_buf, uint32_t tx_len)
{
    spi_dma_disable(spi, SPI_DMA_TRANSMIT);
    spi_disable(spi);

    dma_parameter_struct dma_init_struct;

    dma_deinit(dma, channel);
    dma_struct_para_init(&dma_init_struct);
    
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(spi);
    dma_init_struct.memory_addr  = (uint32_t)tx_buf;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = tx_len;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(dma, channel, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(dma, channel);
    dma_memory_to_memory_disable(dma, channel);

    dma_channel_enable(dma, channel);

    spi_dma_enable(spi, SPI_DMA_TRANSMIT);
    spi_enable(spi);
}

bool spi_ready(spi_handle_t spi)
{
    //uint32_t* hspi = (uint32_t*)spi;

    if (dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF) == SET) {
        return false;
    } else {
        return true;
    }
}

amk_error_t spi_send_async(spi_handle_t spi, const void *data, size_t length)
{
    if (spi_ready(spi)) {
        uint32_t* hspi = (uint32_t*)spi;
        spi_dma_tx(*hspi, DMA0, DMA_CH2, data, length);
        return AMK_SUCCESS;
    } else {
        return AMK_SPI_BUSY;
    }
}

amk_error_t spi_recv_async(spi_handle_t spi, void* data, size_t length)
{
    return AMK_SPI_ERROR;
}

amk_error_t spi_xfer_async(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    return AMK_SPI_ERROR;
}

amk_error_t spi_send(spi_handle_t spi, const void *data, size_t length)
{
    uint32_t* hspi = (uint32_t*)spi;
    const uint8_t* cur = (const uint8_t*)data;
    while(length--) {
        spi_write_byte(*hspi, *cur++);
    }

    return AMK_SUCCESS;
}

amk_error_t spi_recv(spi_handle_t spi, void* data, size_t length)
{
    uint32_t* hspi = (uint32_t*)spi;
    uint8_t* cur = (uint8_t*)data;
    while(length--) {
        *cur = spi_write_byte(*hspi, *cur);
        cur++;
    }

    return AMK_SUCCESS;
}

amk_error_t spi_xfer(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    uint32_t* hspi = (uint32_t*)spi;
    const uint8_t* tx_cur = (const uint8_t*)tx_buffer;
    uint8_t* rx_cur = (uint8_t*)rx_buffer;

    while(length--) {
        *rx_cur++ = spi_write_byte(*hspi, *tx_cur++);
    }

    return AMK_SUCCESS;
}

void spi_uninit(spi_handle_t spi)
{}
