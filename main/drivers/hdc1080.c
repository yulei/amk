/**
 * @file hdc1080.c
 * @brief driver for hdc1080
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "hdc1080.h"
#include "amk_hal.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef HDC1080_DEBUG
#define HDC1080_DEBUG  1
#endif

#if HDC1080_DEBUG
#define hdc1080_debug  amk_printf
#else
#define hdc1080_debug(...)
#endif

#define HDC1080_ADDR        0x80
#define TEMPERATURE_REG     0x00
#define HUMIDITY_REG        0x01
#define CONFIGURATION_REG   0x01
#define SERIAL_1_REG        0xFB
#define SERIAL_2_REG        0xFC
#define SERIAL_3_REG        0xFD
#define MANUFACTURER_REG    0xFE
#define DEVICE_REG          0xFE

#define HDC1080_TIMEOUT     100

extern I2C_HandleTypeDef hi2c2;
#define hc_i2c  hi2c2

bool hdc1080_init(void)
{
    // wait for startup
    wait_ms(20);
    uint8_t buf[4];

    buf[0] = 0x10; // enable both temp&humi
    buf[1] = 0x00;

    HAL_StatusTypeDef status = HAL_ERROR;
    status = HAL_I2C_Mem_Write(&hc_i2c, HDC1080_ADDR, CONFIGURATION_REG, 1, buf, 2, HDC1080_TIMEOUT);
    if (status != HAL_OK) {
        hdc1080_debug("HDC1080: failed to write configuration, %d\n", status);
        return false;
    }

    return true;
}

bool hdc1080_read(uint8_t* temperature, uint8_t* humidity)
{

	uint8_t data = TEMPERATURE_REG;

    HAL_StatusTypeDef status = HAL_ERROR;

	status = HAL_I2C_Master_Transmit(&hc_i2c, HDC1080_ADDR, &data, 1, HDC1080_TIMEOUT);
    if (status != HAL_OK) {
        hdc1080_debug("HDC1080: failed to start measument, %d\n", status);
        return false;
    }

	// Delay here for conversion compelete, datasheet said is 7ms
	wait_ms(15);

	// Read temperature and humidity
    uint8_t buf[4];
	status = HAL_I2C_Master_Receive(&hc_i2c,HDC1080_ADDR, buf, 4, HDC1080_TIMEOUT);
    if (status != HAL_OK) {
        hdc1080_debug("HDC1080: failed to receive data, %d\n", status);
        return false;
    }

	uint16_t temp = ((buf[0]<<8)|buf[1]);
	uint16_t humi = ((buf[2]<<8)|buf[3]);

	*temperature = (uint8_t) ((temp/65536.0)*165.0)-40.0;
	*humidity = (uint8_t) ((humi/65536.0)*100.0);

    return true;
}

void hdc1080_shutdown(void)
{}