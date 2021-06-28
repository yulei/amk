/**
 * @file tca6424.h
 */

#include "tca6424.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef TCA6424_DEBUG
#define TCA6424_DEBUG 1
#endif

#if TCA6424_DEBUG
#define tca6424_debug  amk_printf
#else
#define tca6424_debug(...)
#endif

#define TCA6424_INPUT_PORT0     0x0
#define TCA6424_INPUT_PORT1     0x01
#define TCA6424_INPUT_PORT2     0x02

#define TCA6424_OUTPUT_PORT0    0x04
#define TCA6424_OUTPUT_PORT1    0x05
#define TCA6424_OUTPUT_PORT2    0x06

#define TCA6424_POLARITY_PORT0  0x08
#define TCA6424_POLARITY_PORT1  0x09
#define TCA6424_POLARITY_PORT2  0x0A

#define TCA6424_CONF_PORT0      0x0C
#define TCA6424_CONF_PORT1      0x0D
#define TCA6424_CONF_PORT2      0x0E

#define TIMEOUT                 100

#ifndef TCA6424_I2C_ID
#define TCA6424_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;
static uint8_t tca_addr = TCA6424_ADDR;

void tca6424_init(void)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(TCA6424_I2C_ID);
        tca6424_debug("tca6424: inited, %p\n", i2c_inst);
    }
}

static void write_port(uint8_t port, uint8_t data)
{
    amk_error_t error = i2c_write_reg(i2c_inst, tca_addr, port, &data, 1, TIMEOUT);
    
    if (error != AMK_SUCCESS) {
        tca6424_debug("tca6424: failed to write addr:0x%x, port:%d, data:%d, error:%d\n", tca_addr, port, data, error);
    }
}

static uint8_t read_port(uint8_t port)
{
    uint8_t data = 0;
    amk_error_t error = i2c_read_reg(i2c_inst, tca_addr, port, &data, 1, TIMEOUT);
    if (error != AMK_SUCCESS) {
        tca6424_debug("tca6424: failed to read port:%d, error:%d\n", port, error);
    }

    return data;
}

void tca6424_write_config(TCA6424_PORT port, uint8_t data)
{
    switch(port) {
        case TCA6424_PORT0:
            write_port(TCA6424_CONF_PORT0, data);
            break;
        case TCA6424_PORT1:
            write_port(TCA6424_CONF_PORT1, data);
            break;
        case TCA6424_PORT2:
            write_port(TCA6424_CONF_PORT2, data);
            break;
    }
}

void tca6424_write_polarity(TCA6424_PORT port, uint8_t data)
{
    switch(port) {
        case TCA6424_PORT0:
            write_port(TCA6424_POLARITY_PORT0, data);
            break;
        case TCA6424_PORT1:
            write_port(TCA6424_POLARITY_PORT1, data);
            break;
        case TCA6424_PORT2:
            write_port(TCA6424_POLARITY_PORT2, data);
            break;
    }
}

void tca6424_write_port(TCA6424_PORT port, uint8_t data)
{
    switch(port) {
        case TCA6424_PORT0:
            write_port(TCA6424_OUTPUT_PORT0, data);
            break;
        case TCA6424_PORT1:
            write_port(TCA6424_OUTPUT_PORT1, data);
            break;
        case TCA6424_PORT2:
            write_port(TCA6424_OUTPUT_PORT2, data);
            break;
    }
}

uint8_t tca6424_read_port(TCA6424_PORT port)
{
    switch(port) {
        case TCA6424_PORT0:
            return read_port(TCA6424_INPUT_PORT0);
        case TCA6424_PORT1:
            return read_port(TCA6424_INPUT_PORT1);
        case TCA6424_PORT2:
            return read_port(TCA6424_INPUT_PORT2);
    }

    return 0;
}
