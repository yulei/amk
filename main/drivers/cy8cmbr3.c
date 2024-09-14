/**
 * @file cy8cmbr3.c
 * @brief driver for cy8cmbr3xxxx
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "cy8cmbr3.h"

#include "amk_printf.h"

#ifndef CY8CMBR3_DEBUG
#define CY8CMBR3_DEBUG  1
#endif

#if CY8CMBR3_DEBUG
#define cmbr3_debug  amk_printf
#else
#define cmbr3_debug(...)
#endif

extern I2C_HandleTypeDef hi2c2;

#define hc_i2c  hi2c2

#define CY8CMBR3_KEY_MAX        16
#define CY8CMBR3_TIMEOUT        100

// Register address
#define CY8CMBR3_VER            (0x55)
#define CY8CMBR3_ADDR           (0x6E)  //0x37<<1
#define CY8CMBR3_BUTTON_ADDR    0xAA
#define CY8CMBR3_CTRL_CMD       0x86
#define CY8CMBR3_CALC_CRC       0x94
#define CY8CMBR3_SENSOR_DATA    0xba

// Command code
#define CMD_CMD_NULL                0x00
#define CMD_SAVE_CHECK_CRC          0x02
#define CMD_CALC_CRC                0x03
#define CMD_SLEEP                   0x07
#define CMD_CLEAR_LATCHED_STATUS    0x08
#define CMD_RESET_PROX0_FILTER      0x09
#define CMD_RESET_PROX1_FILTER	    0x0A
#define CMD_SW_RESET                0xFF

const unsigned char cy8xmbr3_config[128] = {
    0x0Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u,
    0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu,
    0x7Fu, 0x7Fu, 0x7Fu, 0x7Fu, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
    0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x01u, 0x58u,
    0x00u, 0x37u, 0x05u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xF5u, 0x8Du
};

bool cy8cmbr3_init(void)
{
    HAL_Delay(200);
    HAL_StatusTypeDef status = HAL_ERROR;
    uint8_t data = 0;
    uint16_t crc = 0;
    status = HAL_I2C_Mem_Read(&hc_i2c, CY8CMBR3_ADDR, CY8CMBR3_CALC_CRC, 1, (uint8_t*)&crc, 2, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to read CRC, error=%d\n", status);
        return false;
    } else {
        cmbr3_debug("CY8CMBR3: Current calculated CRC=0x%x\n", crc);
    }


    status = HAL_I2C_Mem_Write(&hc_i2c, CY8CMBR3_ADDR, 0x00, 1, (uint8_t*)&cy8xmbr3_config[0], 128, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to initialze cy8cmbr3, error=%d\n", status);
        return false;
    }

    data = CMD_SAVE_CHECK_CRC;
    status = HAL_I2C_Mem_Write(&hc_i2c, CY8CMBR3_ADDR, CY8CMBR3_CTRL_CMD, 1, &data, 1, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to save crc to cy8cmbr3, error=%d\n", status);
        return false;
    }
    HAL_Delay(300);
    status = HAL_I2C_Mem_Read(&hc_i2c, CY8CMBR3_ADDR, CY8CMBR3_CALC_CRC, 1, (uint8_t*)&crc, 2, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to readback CRC, error=%d\n", status);
        return false;
    } else {
        cmbr3_debug("CY8CMBR3: readback CRC=0x%x\n", crc);
    }

    cy8cmbr3_reset();
    return true;
}

bool cy8cmbr3_read(uint16_t* port)
{
    uint8_t reg = CY8CMBR3_BUTTON_ADDR;
    static uint32_t old_port = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hc_i2c, CY8CMBR3_ADDR, reg, 1, (uint8_t*)&port, 2, 100);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to read port data, error=%d\n", status);
        return false;
    } else {
        if (*port != 0) {
            if (old_port != *port) {
                cmbr3_debug("CY8CMBR3: port data is 0x%x\n", *port);
                old_port = *port;
            }
        }
    }
    return true;
}

bool cy8cmbr3_read_keys(uint8_t* keys, uint8_t start, uint8_t count)
{
    if ((start + count) > CY8CMBR3_KEY_MAX) {
        cmbr3_debug("CY8CMBR3: Read data out of range(start:%d, count:%d)\n", start, count);
        return false;
    }

    uint16_t buf[CY8CMBR3_KEY_MAX];
    HAL_StatusTypeDef status = HAL_ERROR;
    status = HAL_I2C_Mem_Read(&hc_i2c, CY8CMBR3_ADDR, CY8CMBR3_SENSOR_DATA+start, 1, (uint8_t*)&buf[0], count*2, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to read sensor data, error=%d\n", status);
        return false;
    }
    static uint8_t old[CY8CMBR3_KEY_MAX];
    for (int i = 0; i < count; i++) {
        keys[i] = (uint8_t)buf[i];
        if (old[i] != keys[i]) {
            cmbr3_debug("CY8CMBR3: Sensor Data(index:%d, old:%d, new:%d)\n", start+i, old[i], keys[i]);
            old[i] = keys[i];
        }
    }
    return true;
}

void cy8cmbr3_reset(void)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    uint8_t data = 0;

	data = CMD_SW_RESET;
    status = HAL_I2C_Mem_Write(&hc_i2c, CY8CMBR3_ADDR, CY8CMBR3_CTRL_CMD, 1, &data, 1, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to write RESET, error=%d\n", status);
    }

    HAL_Delay(300);
}

void cy8cmbr3_shutdown(void)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    uint8_t data = 0;

	data = CMD_SLEEP;
    status = HAL_I2C_Mem_Write(&hc_i2c, CY8CMBR3_ADDR, CY8CMBR3_CTRL_CMD, 1, &data, 1, CY8CMBR3_TIMEOUT);
    if (status != HAL_OK) {
        cmbr3_debug("CY8CMBR3: Faield to write SLEEP, error=%d\n", status);
    }
}