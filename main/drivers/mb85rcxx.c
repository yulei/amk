/**
 * mb85rcxx.c
 */

#include <stdbool.h>
#include "mb85rcxx.h"
#include "i2c.h"

#define MB85RC_PREFIX 0b1010
#define TIMEOUT 100

static bool mb85rcxx_ready = false;

#define MB85RC_ADDR(x)      (uint8_t)((MB85RC_PREFIX<<4) | (((x)&0x0700) >> 7))
#define MB85RC_OFFSET(x)    (uint8_t)((x)&0x00FF)

void mb85rcxx_init(void)
{
    if (mb85rcxx_ready) return;

    i2c_init();
    mb85rcxx_ready = true;
}

void mb85rcxx_uninit(void)
{
    if (!mb85rcxx_ready) return;

    i2c_uninit();
    mb85rcxx_ready = false;
}

void mb85rcxx_write_byte(uint16_t addr, uint8_t data)
{
    if (!mb85rcxx_ready) mb85rcxx_init();

    uint8_t dev_addr = MB85RC_ADDR(addr);
    uint8_t cmd[2];
    cmd[0] = MB85RC_OFFSET(addr);
    cmd[1] = data;
    i2c_send(dev_addr, cmd, 2, TIMEOUT);
}

uint8_t mb85rcxx_read_byte(uint16_t addr)
{
    if (!mb85rcxx_ready) mb85rcxx_init();

    uint8_t dev_addr = MB85RC_ADDR(addr);
    uint8_t offset = MB85RC_OFFSET(addr);
    i2c_send(dev_addr, &offset, 1, TIMEOUT);
    uint8_t data = 0;
    i2c_recv(dev_addr, &data, 1, TIMEOUT);
    return data;
}