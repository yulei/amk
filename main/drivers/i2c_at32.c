/**
 * @file i2c_at32.c 
 * @author astro
 *
 * @copyright Copyright (c) 2024
 */ 

#include <stdbool.h>
#include <string.h>

#include "amk_hal.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"
#include "i2c_application.c"

#ifdef USE_I2C1
extern i2c_handle_type hi2c1;
//static bool i2c1_busy = false;
#endif

#define I2C_TIMEOUT         (SystemCoreClock/1000)

bool i2c_ready(i2c_handle_t i2c)
{
    return true;
}

i2c_handle_t i2c_init(I2C_ID id)
{
#ifdef USE_I2C1
    if (id == I2C_INSTANCE_1) {
        hi2c1.i2cx = I2C1;
        return &hi2c1;
    }
#endif

    return NULL;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    i2c_handle_type *inst = (i2c_handle_type*)i2c;

    i2c_status_type status = i2c_master_transmit(inst, addr, (void*)data, length, I2C_TIMEOUT*timeout);
    if (status == I2C_OK) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    i2c_handle_type *inst = (i2c_handle_type*)i2c;

    i2c_status_type status = i2c_master_receive(inst, addr, data, length, I2C_TIMEOUT*timeout);

    if (status == I2C_OK) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    i2c_handle_type *inst = (i2c_handle_type*)i2c;
    i2c_status_type status = i2c_memory_write(inst, I2C_MEM_ADDR_WIDIH_8, addr, reg, (uint8_t*)data, length, I2C_TIMEOUT*timeout);

    if (status == I2C_OK) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    i2c_handle_type *inst = (i2c_handle_type*)i2c;
    i2c_status_type status = i2c_memory_read(inst, I2C_MEM_ADDR_WIDIH_8, addr, reg, data, length, I2C_TIMEOUT*timeout);
    if (status == I2C_OK) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

void i2c_uninit(i2c_handle_t i2c)
{
    if(!i2c_ready(i2c)) {
        return;
    }

}

amk_error_t i2c_send_async(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length)
{
    return i2c_send(i2c, addr, data, length, 100);
}
