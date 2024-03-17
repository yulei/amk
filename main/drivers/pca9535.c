/**
 * @file pca9535.c
 */

#include "pca9535.h"
#include "i2c.h"
#include "amk_printf.h"

#ifndef PCA9535_DEBUG
#define PCA9535_DEBUG 0
#endif

#if PCA9535_DEBUG
#define pca9535_debug  amk_printf
#else
#define pca9535_debug(...)
#endif

#define PCA9535_INPUT_PORT0     0x00
#define PCA9535_INPUT_PORT1     0x01

#define PCA9535_OUTPUT_PORT0    0x02
#define PCA9535_OUTPUT_PORT1    0x03

#define PCA9535_POLARITY_PORT0  0x04
#define PCA9535_POLARITY_PORT1  0x05

#define PCA9535_CONF_PORT0      0x06
#define PCA9535_CONF_PORT1      0x07

#define TIMEOUT                 100

#ifndef PCA9535_I2C_ID
#define PCA9535_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;

void pca9535_init(void)
{
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(PCA9535_I2C_ID);
    }
}

static void write_port(uint8_t p, uint8_t d)
{
    ak_i2c_write_reg(i2c_inst, PCA9535_ADDR, p, &d, 1, TIMEOUT);
}

static uint8_t read_port(uint8_t port)
{
    uint8_t data = 0;
    ak_i2c_read_reg(i2c_inst, PCA9535_ADDR, port, &data, 1, TIMEOUT);
    return data;
}

void pca9535_write_config(PCA9535_PORT port, uint8_t data)
{
    switch(port) {
        case PCA9535_PORT0:
            write_port(PCA9535_CONF_PORT0, data);
            break;
        case PCA9535_PORT1:
            write_port(PCA9535_CONF_PORT1, data);
            break;
        default:
            pca9535_debug("PCA9535: unknown config port:%d\n", port);
            break;
    }
}

void pca9535_write_polarity(PCA9535_PORT port, uint8_t data)
{
    switch(port) {
        case PCA9535_PORT0:
            write_port(PCA9535_POLARITY_PORT0, data);
            break;
        case PCA9535_PORT1:
            write_port(PCA9535_POLARITY_PORT1, data);
            break;
        default:
            pca9535_debug("PCA9535: unknown polarity port:%d\n", port);
            break;
    }
}

void pca9535_write_port(PCA9535_PORT port, uint8_t data)
{
    switch(port) {
        case PCA9535_PORT0:
            write_port(PCA9535_OUTPUT_PORT0, data);
            break;
        case PCA9535_PORT1:
            write_port(PCA9535_OUTPUT_PORT1, data);
            break;
        default:
            pca9535_debug("PCA9535: unknown output port:%d\n", port);
            break;
    }
}

uint8_t pca9535_read_port(PCA9535_PORT port)
{
    switch(port) {
        case PCA9535_PORT0:
            return read_port(PCA9535_INPUT_PORT0);
        case PCA9535_PORT1:
            return read_port(PCA9535_INPUT_PORT1);
        default:
            pca9535_debug("PCA9535: unknown input port:%d\n", port);
            break;
    }

    return 0;
}
