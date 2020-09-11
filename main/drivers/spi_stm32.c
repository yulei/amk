/**
 * spi_stm32.c
 */

#include "spi.h"

#include "generic_hal.h"

extern SPI_HandleTypeDef hspi1;

void spi_init(void)
{}

bool spi_ready(void)
{
    return true;
}

amk_i2c_error_t spi_send(const void *data, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, (uint8_t*)data, length, timeout);
    if (status != HAL_OK) {
        return AMK_SPI_ERROR;
    }
    return AMK_SUCCESS;
}

amk_i2c_error_t spi_recv(void* data, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, (uint8_t*)data, length, timeout);
    if (status != HAL_OK) {
        return AMK_SPI_ERROR;
    }
    return AMK_SUCCESS;
}

amk_i2c_error_t spi_xfer(const void *tx_buffer, void *rx_buffer, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)tx_buffer, (uint8_t*)rx_buffer, length, timeout);
    if (status != HAL_OK) {
        return AMK_SPI_ERROR;
    }
    return AMK_SUCCESS;
}

void spi_uninit(void)
{}