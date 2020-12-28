/**
 * spi_stm32.c
 */

#include "spi.h"
#include "generic_hal.h"
#include "amk_printf.h"

#define TIMEOUT_DEFAULT 100

extern SPI_HandleTypeDef hspi1;

spi_handle_t spi_init(void)
{
    return &hspi1;
}

bool spi_ready(spi_handle_t spi)
{
    SPI_HandleTypeDef* hspi = (SPI_HandleTypeDef*)spi;
    return hspi->State == HAL_SPI_STATE_READY; 
}

amk_error_t spi_send_async(spi_handle_t spi, const void *data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(hspi, (uint8_t *)data, length);
        if (status != HAL_OK) {
            return AMK_SPI_ERROR;
        }
        return AMK_SUCCESS;
    } else {
        return AMK_SPI_BUSY;
    }
}

amk_error_t spi_recv_async(spi_handle_t spi, void* data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_Receive_DMA(hspi, (uint8_t*)data, length);
        if (status != HAL_OK) {
            return AMK_SPI_ERROR;
        } 
        return AMK_SUCCESS;
    } else {
        return AMK_SPI_BUSY;
    }
}

amk_error_t spi_xfer_async(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    if (spi_ready(spi)) {
        HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(hspi, (uint8_t*)tx_buffer, (uint8_t*)rx_buffer, length);
        if (status != HAL_OK) {
            return AMK_SPI_ERROR;
        }
        return AMK_SUCCESS;
    } else  {
        return AMK_SPI_BUSY;
    }
}

amk_error_t spi_send(spi_handle_t spi, const void *data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, (uint8_t *)data, length, TIMEOUT_DEFAULT);
    if (status != HAL_OK) {
        amk_printf("spi send error: %d\n", status);
        return AMK_SPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t spi_recv(spi_handle_t spi, void* data, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    HAL_StatusTypeDef status = HAL_SPI_Receive(hspi, (uint8_t*)data, length, TIMEOUT_DEFAULT);

    if (status != HAL_OK) {
        amk_printf("spi recv error: %d\n", status);
        return AMK_SPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t spi_xfer(spi_handle_t spi, const void *tx_buffer, void *rx_buffer, size_t length)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)spi;
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(hspi, (uint8_t*)tx_buffer, (uint8_t*)rx_buffer, length, TIMEOUT_DEFAULT);
    if (status != HAL_OK) {
        amk_printf("spi xfer error: %d\n", status);
        return AMK_SPI_ERROR;
    }

    return AMK_SUCCESS;
}

void spi_uninit(spi_handle_t spi)
{}

// SPI DMA callback
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    amk_printf("SPI DMA error:\n", hspi->ErrorCode);
}
