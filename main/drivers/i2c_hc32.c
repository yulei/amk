/**
 * @file i2c_hc32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generic_hal.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"

typedef struct {
    M4_I2C_TypeDef *handle;
    bool ready;
} i2c_instance_t;

#define I2C_BAUDRATE                    (400000ul)
#define I2C_TIMEOUT                     (0x10000ul)

#ifdef USE_I2C1
    #define I2C1_ID     M4_I2C1
    #define I2C1_SCL    I2C1_SCL_PIN
    #define I2C1_SDA    I2C1_SDA_PIN
    static i2c_instance_t m_i2c1 = {
        .ready = false,
    };
#endif

#ifdef USE_I2C2 
    #define I2C2_ID     M4_I2C2 
    #define I2C2_SCL    I2C2_SCL_PIN
    #define I2C2_SDA    I2C2_SDA_PIN
    static i2c_instance_t m_i2c2 = {
        .ready = false,
    };
#endif


bool i2c_ready(i2c_handle_t i2c)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    return inst->ready;
}

void i2c_inst_init(i2c_instance_t *inst, M4_I2C_TypeDef* i2c)
{
    if (i2c_ready(inst))
        return;

    if (i2c == I2C1_ID) {
        PORT_SetFunc(GET_PORT(I2C1_SCL), GET_PIN(I2C1_SCL), Func_I2c1_Scl, Disable);
        PORT_SetFunc(GET_PORT(I2C1_SDA), GET_PIN(I2C1_SDA), Func_I2c1_Sda, Disable);

        /* Enable I2C Peripheral*/
        PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2C1, Enable);


        I2C_DeInit(I2C1_ID);

        stc_i2c_init_t stcI2cInit;
        MEM_ZERO_STRUCT(stcI2cInit);
        stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
        stcI2cInit.u32Baudrate = I2C_BAUDRATE;
        stcI2cInit.u32SclTime = 0ul;
        float32_t fErr;
        en_result_t enRet = I2C_Init(I2C1_ID, &stcI2cInit, &fErr);
        if (enRet != Ok) {
            amk_printf("HC32 Failed to initialize I2C1: result=%d\n", enRet);
        } else {
            inst->handle = i2c;
            inst->ready = true;
        }

        I2C_BusWaitCmd(I2C1_ID, Enable);
    }
}

i2c_handle_t i2c_init(I2C_ID id)
{
#ifdef USE_I2C1
    if (id == I2C_INSTANCE_1) {
        i2c_inst_init(&m_i2c1, I2C1_ID);
        return &m_i2c1;
    }
#endif

#ifdef USE_I2C2 
    if (id == I2C_INSTANCE_2) {
        i2c_inst_init(&m_i2c2, I2C2_ID);
        return &m_i2c2;
    }
#endif

    return NULL;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    I2C_Cmd(inst->handle, Enable);

    I2C_SoftwareResetCmd(inst->handle, Enable);
    I2C_SoftwareResetCmd(inst->handle, Disable);
    en_result_t status = I2C_Start(inst->handle, I2C_TIMEOUT);
    if(Ok == status) {
        status = I2C_TransAddr(inst->handle, addr >> 1, I2CDirTrans, I2C_TIMEOUT);
        if(Ok == status) {
            status = I2C_TransData(inst->handle, data, length, I2C_TIMEOUT);
        }
    }

    I2C_Stop(inst->handle, I2C_TIMEOUT);
    I2C_Cmd(inst->handle, Disable);

    if (status == Ok) {
        return AMK_SUCCESS;
    }

    if (status == ErrorTimeout) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;

    I2C_Cmd(inst->handle, Enable);
    I2C_SoftwareResetCmd(inst->handle, Enable);
    I2C_SoftwareResetCmd(inst->handle, Disable);
    en_result_t status = I2C_Start(inst->handle, I2C_TIMEOUT);
    if(Ok == status) {
        if(1ul == length) {
            I2C_AckConfig(inst->handle, I2c_NACK);
        }

        status = I2C_TransAddr(inst->handle, addr>>1, I2CDirReceive, I2C_TIMEOUT);

        if(Ok == status) {
            status = I2C_MasterDataReceiveAndStop(inst->handle, data, length, I2C_TIMEOUT);
        }

        I2C_AckConfig(inst->handle, I2c_ACK);
    }

    if(Ok != status) {
        I2C_Stop(inst->handle, I2C_TIMEOUT);
    }

    I2C_Cmd(inst->handle, Disable);

    if (status == Ok) {
        return AMK_SUCCESS;
    }

    if (status == ErrorTimeout) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;

    I2C_Cmd(inst->handle, Enable);

    I2C_SoftwareResetCmd(inst->handle, Enable);
    I2C_SoftwareResetCmd(inst->handle, Disable);
    en_result_t status = I2C_Start(inst->handle, I2C_TIMEOUT);

    if(Ok == status) {
        status = I2C_TransAddr(inst->handle, addr>>1, I2CDirTrans, I2C_TIMEOUT);

        if(Ok == status) {
            status = I2C_TransData(inst->handle, &reg, 1, I2C_TIMEOUT);
            if(Ok == status) {
                status = I2C_TransData(inst->handle, data, length, I2C_TIMEOUT);
            }
        }
    }

    I2C_Stop(inst->handle, I2C_TIMEOUT);
    I2C_Cmd(inst->handle, Disable);

    if (status == Ok) {
        return AMK_SUCCESS;
    }
    if (status == ErrorTimeout) {
        return AMK_I2C_TIMEOUT;
    }

    return AMK_ERROR;
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;

    I2C_Cmd(inst->handle, Enable);
    I2C_SoftwareResetCmd(inst->handle, Enable);
    I2C_SoftwareResetCmd(inst->handle, Disable);
    en_result_t status = I2C_Start(inst->handle, I2C_TIMEOUT);
    if(Ok == status) {
        status = I2C_TransAddr(inst->handle, addr>>1, I2CDirTrans, I2C_TIMEOUT);

        if(Ok == status) {
            status = I2C_TransData(inst->handle, &reg, 1, I2C_TIMEOUT);
            if(Ok == status) {
                status = I2C_Restart(inst->handle, I2C_TIMEOUT);
                if(1 == length) {
                    I2C_AckConfig(inst->handle, I2c_NACK);
                }

                if(Ok == status) {
                    status = I2C_TransAddr(inst->handle, addr>>1, I2CDirReceive, I2C_TIMEOUT);
                    if(Ok == status) {
                        status = I2C_MasterDataReceiveAndStop(inst->handle, data, length, I2C_TIMEOUT);
                    }

                    I2C_AckConfig(inst->handle, I2c_ACK);
                }
            }
        }
    }

    if(Ok != status) {
        I2C_Stop(inst->handle, I2C_TIMEOUT);
    }

    I2C_Cmd(inst->handle, Disable);

    if (status == Ok) {
        return AMK_SUCCESS;
    }
    if (status == ErrorTimeout) {
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
    I2C_DeInit(inst->handle);
    inst->ready = false;
}

#if 0
amk_error_t i2c_send_async(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length)
{
    amk_error_t err = AMK_SUCCESS;
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_DMA(&inst->handle, addr, (void*)data, length);
    if (status == HAL_OK) {
        return AMK_SUCCESS;
    }
    if (status == HAL_BUSY) {
        return AMK_I2C_BUSY;
    }

    return AMK_ERROR;
}
#endif
