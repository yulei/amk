 /**
  * i2c_stm32.c
  */

#include <stdbool.h>
#include <string.h>

#include "generic_hal.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"

typedef struct {
    I2C_HandleTypeDef handle;
    bool ready;
} i2c_instance_t;

#ifdef I2C_USE_INSTANCE_1
    #define I2C1_ID     I2C1
    #define I2C1_SCL    I2C1_SCL_PIN
    #define I2C1_SDA    I2C1_SDA_PIN
    static i2c_instance_t m_i2c1 = {
        .ready = false,
    };
#endif



bool i2c_ready(i2c_handle_t i2c)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    return inst->ready;
}

static void i2c_inst_init(i2c_instance_t *inst, I2C_TypeDef *i2c)
{
    if (i2c_ready(inst))
        return;

#ifdef STM32F722xx
    inst->handle.Instance = I2C1;
    inst->handle.Init.Timing = 0x6000030D;
    inst->handle.Init.OwnAddress1 = 0;
    inst->handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    inst->handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    inst->handle.Init.OwnAddress2 = 0;
    inst->handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    inst->handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    inst->handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&inst->handle) != HAL_OK) {
        amk_printf("HAL_I2C_Init() failed\n");
    }
    /** Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&inst->handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        amk_printf("HAL_I2CEx_ConfigAnalogFilter() failed\n");
    }
    /** Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(&inst->handle, 0) != HAL_OK) {
        amk_printf("HAL_I2CEx_ConfigDigitalFilter() failed\n");
    }
#else
    inst->handle.Instance = i2c;
    inst->handle.Init.ClockSpeed = 400000;
    inst->handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    inst->handle.Init.OwnAddress1 = 0;
    inst->handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    inst->handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    inst->handle.Init.OwnAddress2 = 0;
    inst->handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    inst->handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&inst->handle) == HAL_OK) {
        inst->ready = true;
    } else {
        //Error_Handler();
    }
#endif
    inst->ready = true;
}

i2c_handle_t i2c_init(I2C_ID id)
{
    if (id == I2C_INSTANCE_1) {
        i2c_inst_init(&m_i2c1, I2C1_ID);
        return &m_i2c1;
    }

    return NULL;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&inst->handle, addr, (void*)data, length, timeout);
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
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&inst->handle, addr, data, length, timeout);
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
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&inst->handle, addr, reg, 1, (void*)data, length, timeout);
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
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&inst->handle, addr, reg, 1, data, length, timeout);
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
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    HAL_I2C_DeInit(&inst->handle);
    inst->ready = false;
}