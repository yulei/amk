 /**
  * i2c.h
  *     i2c driver interface 
  *
  */

#pragma once

#include <stdbool.h>
#include "amk_error.h"

typedef enum {
    I2C_INSTANCE_1,
    I2C_INSTANCE_2,
} I2C_ID;

typedef void* i2c_handle_t;

i2c_handle_t i2c_init(I2C_ID id);
bool i2c_ready(i2c_handle_t i2c);
amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length, size_t timeout);
amk_error_t i2c_send_async(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length);
amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout);
amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout);
amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout);
void i2c_uninit(i2c_handle_t i2c);
