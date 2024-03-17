/**
 * mb85rcxx.c
 */

#include <stdbool.h>
#include "mb85rcxx.h"
#include "i2c.h"

#define MB85RC_PREFIX 0b1010
#define TIMEOUT 100

#define MB85RC_ADDR(x)      (uint8_t)((MB85RC_PREFIX<<4) | (((x)&0x0700) >> 7))
#define MB85RC_OFFSET(x)    (uint8_t)((x)&0x00FF)

#ifndef MB85_I2C_ID
#define MB85_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;

void mb85rcxx_write_byte(uint16_t addr, uint8_t data)
{
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(MB85_I2C_ID);
    }

    uint8_t dev_addr = MB85RC_ADDR(addr);
    uint8_t cmd[2];
    cmd[0] = MB85RC_OFFSET(addr);
    cmd[1] = data;
    ak_i2c_send(i2c_inst, dev_addr, cmd, 2, TIMEOUT);
}

uint8_t mb85rcxx_read_byte(uint16_t addr)
{
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(MB85_I2C_ID);
    }

    uint8_t dev_addr = MB85RC_ADDR(addr);
    uint8_t offset = MB85RC_OFFSET(addr);
    ak_i2c_send(i2c_inst, dev_addr, &offset, 1, TIMEOUT);
    uint8_t data = 0;
    ak_i2c_recv(i2c_inst, dev_addr, &data, 1, TIMEOUT);

    return data;
}