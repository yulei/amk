 /**
  * i2c_stm32.c
  */

#include <stdbool.h>
#include <string.h>

#include "amk_hal.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifdef USE_I2C1
extern I2C_HandleTypeDef hi2c1;
static bool i2c1_busy = false;
#endif

#ifdef USE_I2C2
extern I2C_HandleTypeDef hi2c2;
static bool i2c2_busy = false;
#endif

bool i2c_ready(i2c_handle_t i2c)
{
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    
    return (inst->State == HAL_I2C_STATE_READY);
}

i2c_handle_t i2c_init(I2C_ID id)
{
#ifdef USE_I2C1
    if (id == I2C_INSTANCE_1) {
        return &hi2c1;
    }
#endif

#ifdef USE_I2C2 
    if (id == I2C_INSTANCE_2) {
        return &hi2c2;
    }
#endif

    return NULL;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(inst, addr, (void*)data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(inst, addr, data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(inst, addr, reg, 1, (void*)data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(inst, addr, reg, 1, data, length, timeout);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_I2C_ERROR_TIMEOUT) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

void i2c_uninit(i2c_handle_t i2c)
{
    if(!i2c_ready(i2c)) {
        return;
    }
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    HAL_I2C_DeInit(inst);
}

amk_error_t i2c_send_async(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length)
{
#ifdef USE_I2C1
    if (i2c1_busy) {
        if (i2c_ready(i2c)) {
            i2c1_busy = false;
            return AMK_SUCCESS;
        }
        return AMK_I2C_BUSY;
    }
#endif
    I2C_HandleTypeDef *inst = (I2C_HandleTypeDef*)i2c;
    HAL_I2C_Master_Transmit_DMA(inst, addr, (void*)data, length);
#ifdef USE_I2C1
    i2c1_busy = true;
#endif
    return AMK_I2C_BUSY;
}
