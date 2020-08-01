 /**
  * i2c.h
  *     i2c driver wrapper
  *
  */

#pragma once

#include "amk_error.h"

#ifndef I2C_INSTANCE_ID
    #define I2C_INSTANCE_ID     0
#endif

#ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN         18
#endif

#ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN         19
#endif

void i2c_init(void);
amk_i2c_error_t i2c_send(uint8_t addr, const void* data, size_t length, size_t timeout);
amk_i2c_error_t i2c_recv(uint8_t addr, void* data, size_t length, size_t timeout);
amk_i2c_error_t i2c_write_reg(uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout);
amk_i2c_error_t i2c_read_reg(uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout);
void i2c_uninit(void);