 /**
  * i2c.h
  *     i2c driver interface 
  *
  */

#pragma once

#include <stdbool.h>
#include "amk_error.h"

void i2c_init(void);
bool i2c_ready(void);
amk_i2c_error_t i2c_send(uint8_t addr, const void *data, size_t length, size_t timeout);
amk_i2c_error_t i2c_recv(uint8_t addr, void* data, size_t length, size_t timeout);
amk_i2c_error_t i2c_write_reg(uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout);
amk_i2c_error_t i2c_read_reg(uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout);
void i2c_uninit(void);