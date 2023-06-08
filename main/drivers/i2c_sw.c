/**
 * @file i2c_sw.c
 * @author astro
 * @brief software i2c
 * 
 * @copyright Copyright (C) 2023
 */ 

#include <stdbool.h>

#include "generic_hal.h"
#include "i2c.h"
#include "wait.h"
#include "amk_gpio.h"

#ifndef I2C_SW_SCL_PIN
#error "Software I2c scl pin not defined"
#endif

#ifndef I2C_SW_SDA_PIN
#error "Software I2c sda pin not defined"
#endif

#define I2C_SW_H    1
#define I2C_SW_L    0
#define I2C_SW_OUT  1
#define I2C_SW_IN   0

static void i2c_sw_delay(void)
{
    wait_us(10);
}

static void i2c_sw_scl_out(uint8_t value)
{
    gpio_write_pin(I2C_SW_SCL_PIN, value);
}

static void i2c_sw_sda_dir(uint8_t value)
{
    if (value) {
        gpio_set_output_opendrain(I2C_SW_SDA_PIN);
    } else {
        gpio_set_input_floating(I2C_SW_SDA_PIN);
    }
}

static void i2c_sw_sda_out(uint8_t value)
{
    gpio_write_pin(I2C_SW_SDA_PIN, value);
}

static uint8_t i2c_sw_sda_in(void)
{
    return gpio_read_pin(I2C_SW_SDA_PIN);
}

static void i2c_sw_start(void)
{
    i2c_sw_sda_out(I2C_SW_H);
    i2c_sw_delay();
    i2c_sw_scl_out(I2C_SW_H);
    i2c_sw_delay();
    i2c_sw_sda_out(I2C_SW_L);
    i2c_sw_delay();
    i2c_sw_scl_out(I2C_SW_L);
    i2c_sw_delay();
}

static void i2c_sw_stop(void)
{
    i2c_sw_scl_out(I2C_SW_L);
    i2c_sw_delay();
    i2c_sw_sda_out(I2C_SW_L);
    i2c_sw_delay();
    i2c_sw_scl_out(I2C_SW_H);
    i2c_sw_delay();
    i2c_sw_sda_out(I2C_SW_H);
    i2c_sw_delay();
}

static void i2c_sw_ack(void)
{
    i2c_sw_scl_out(I2C_SW_L);
    i2c_sw_delay();
    i2c_sw_sda_out(I2C_SW_L);
    i2c_sw_delay();
    i2c_sw_scl_out(I2C_SW_H);
    i2c_sw_delay();
}

static void i2c_sw_nack(void)
{
    i2c_sw_scl_out(I2C_SW_L);
    i2c_sw_delay();
    i2c_sw_sda_out(I2C_SW_H);
    i2c_sw_delay();
    i2c_sw_scl_out(I2C_SW_H);
    i2c_sw_delay();
}

static uint8_t i2c_sw_wait_ack(void)
{
    uint8_t ret;

    i2c_sw_scl_out(I2C_SW_L);
    //i2c_sw_delay();
    i2c_sw_sda_out(I2C_SW_H);
    i2c_sw_delay();
    i2c_sw_scl_out(I2C_SW_H);
    i2c_sw_delay();

    i2c_sw_sda_dir(I2C_SW_IN);
    if (i2c_sw_sda_in()) {
        ret = 0;
    } else {
        ret = 1;
    }
    i2c_sw_sda_dir(I2C_SW_OUT);
    i2c_sw_scl_out(I2C_SW_L);

    return ret;
}

static uint8_t i2c_sw_write_byte(uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        i2c_sw_scl_out(I2C_SW_L);
        i2c_sw_delay();

        i2c_sw_sda_out((byte&0x80) ? I2C_SW_H:I2C_SW_L);
        i2c_sw_delay();
        byte <<= 1;

        i2c_sw_scl_out(I2C_SW_H);
        i2c_sw_delay();

    }

    i2c_sw_scl_out(I2C_SW_L);

    return i2c_sw_wait_ack();
}

static uint8_t i2c_sw_read_byte(uint8_t ack)
{
    uint8_t byte = 0;
    i2c_sw_scl_out(I2C_SW_L);
    i2c_sw_sda_dir(I2C_SW_IN);
    for (int i = 0; i < 8; i++) {
        i2c_sw_scl_out(I2C_SW_L);
        i2c_sw_delay();
        i2c_sw_scl_out(I2C_SW_H);
        i2c_sw_delay();
        byte |= i2c_sw_sda_in();
        byte <<= 1;
    }

    i2c_sw_scl_out(I2C_SW_L);
    i2c_sw_sda_dir(I2C_SW_OUT);

    if (!ack) {
        i2c_sw_nack();
    } else {
        i2c_sw_ack();
    }

    return byte;
}

typedef struct {
    bool ready;
} i2c_sw_t;

static i2c_sw_t i2c_sw = {
    .ready = false,
};

bool i2c_ready(i2c_handle_t i2c)
{
    i2c_sw_t* i2c_sw = (i2c_sw_t *)i2c;

    return i2c_sw->ready;
}

i2c_handle_t i2c_init(I2C_ID id)
{
    (void)id;

    gpio_set_output_opendrain(I2C_SW_SCL_PIN);

    i2c_sw_sda_dir(I2C_SW_OUT);

    i2c_sw_scl_out(I2C_SW_H);
    i2c_sw_sda_out(I2C_SW_H);
    i2c_sw.ready = true;

    return &i2c_sw;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    amk_error_t err = AMK_SUCCESS;
    uint8_t* cur = (uint8_t*)data;

    addr |= 0x00;

	i2c_sw_start();

	if (i2c_sw_write_byte(addr)) {
        while (length--) {
            if (!i2c_sw_write_byte(*cur++)) {
                err = AMK_ERROR;
                break;
            }
        }
	} else {
        err = AMK_ERROR;
    }

	i2c_sw_stop();

    return err;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    amk_error_t err = AMK_SUCCESS;
    uint8_t* cur = (uint8_t*)data;

    addr |= 0x01;

	i2c_sw_start();
	if (i2c_sw_write_byte(addr)) {
        while (length--) {
            if (length == 0) {
                *cur++ = i2c_sw_read_byte(0);
            } else {
                *cur++ = i2c_sw_read_byte(1);
            }
        }
	} else {
        err = AMK_ERROR;
    }

	i2c_sw_stop();

    return err;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    amk_error_t err = AMK_SUCCESS;
    uint8_t *cur = (uint8_t*)data;

    addr |= 0x00;

	i2c_sw_start();

	if (i2c_sw_write_byte(addr)) {
        if (i2c_sw_write_byte(reg)) {
            while (length--) {
                if (!i2c_sw_write_byte(*cur++)) {
                    err = AMK_ERROR;
                    break;
                }
            }
        } else {
            err = AMK_ERROR;
        }
    } else {
        err = AMK_ERROR;
    }

	i2c_sw_stop();

    return err;
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    amk_error_t err = AMK_SUCCESS;
    uint8_t *cur = (uint8_t*)data;

    addr |= 0x00;
	i2c_sw_start();
	if (i2c_sw_write_byte(addr)) {
        if (i2c_sw_write_byte(reg)) {
            if (length > 0) {
                addr |= 0x01;
                i2c_sw_start();
                if (i2c_sw_write_byte(addr)) {
                    while (length--) {
                        if (length == 0) {
                            *cur++ = i2c_sw_read_byte(0);
                        } else {
                            *cur++ = i2c_sw_read_byte(1);
                        }
                    }
                } else {
                    err = AMK_ERROR;
                }
            }
        } else {
            err = AMK_ERROR;
        }
    } else {
        err = AMK_ERROR;
    }


	i2c_sw_stop();

    return err;
}

void i2c_uninit(i2c_handle_t i2c)
{
    i2c_sw_t *i2c_sw = (i2c_sw_t*)i2c;
    i2c_sw->ready = false;
}
