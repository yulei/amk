/**
 * mb85rcxx.c
 */

#include <stdbool.h>
#include "mb85rcxx.h"
#include "i2c.h"

#define ADDR_PREFIX 0b1010
#define TIMEOUT 100

static bool mb85rcxx_ready = false;

static inline uint8_t compose_addr(uint16_t addr)
{
    return (uint8_t)((ADDR_PREFIX<<4) | ((addr&0x0E00)>>8));
}

static inline uint8_t page_addr(uint16_t addr)
{
    return (uint8_t)(addr&0x00FF);
}

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

    uint8_t dev_addr = compose_addr(addr);
    uint8_t cmd[2];
    cmd[0] = page_addr(addr);
    cmd[1] = data;
    i2c_send(dev_addr, cmd, 2, TIMEOUT);
}

uint8_t mb85rcxx_read_byte(uint16_t addr)
{
    if (!mb85rcxx_ready) mb85rcxx_init();

    uint8_t dev_addr = compose_addr(addr);
    uint8_t reg = page_addr(addr);
    i2c_send(dev_addr, &reg, 1, TIMEOUT);
    uint8_t data = 0;
    i2c_recv(dev_addr, &data, 1, TIMEOUT);
    return data;
}