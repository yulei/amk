 /**
  * i2c_stm32.c
  */

#include <stdbool.h>
#include <string.h>

#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "gpio_pin.h"

#ifndef I2C_INSTANCE_ID
    #define I2C_INSTANCE_ID     I2C1
#endif

#ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN         B8 
#endif

#ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN         B9
#endif


static I2C_HandleTypeDef i2c_handle;
static bool i2c_ready = false;

void i2c_init(void)
{
    i2c_handle.Instance = I2C_INSTANCE_ID;
    i2c_handle.Init.ClockSpeed = 400000;
    i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2c_handle.Init.OwnAddress1 = 0;
    i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c_handle.Init.OwnAddress2 = 0;
    i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&i2c_handle) != HAL_OK) {
        //Error_Handler();
    } else {
        i2c_ready = true;
    }
}

amk_i2c_error_t i2c_send(uint8_t addr, const void* data, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&i2c_handle, addr, (void*)data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_i2c_error_t i2c_recv(uint8_t addr, void* data, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&i2c_handle, addr, data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_i2c_error_t i2c_write_reg(uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&i2c_handle, addr, reg, 1, (void*)data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_i2c_error_t i2c_read_reg(uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&i2c_handle, addr, reg, 1, data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

void i2c_uninit(void)
{
}