/**
 * @file aw9106b.c
 * @brief driver for aw9106b
 */

#include <stdbool.h>

#include "aw9106b.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef AW9106B_DEBUG
#define AW9106B_DEBUG 0
#endif

#if AW9106B_DEBUG
#define aw9106b_debug  amk_printf
#else
#define aw9106b_debug(...)
#endif

#define AW9106B_CTL         0x11
#define AW9106B_CPMD_A      0x12
#define AW9106B_CPMD_B      0x13
#define AW9106B_EN_BRE      0x14

#define AW9106B_PWM_BASE    0x20
#define AW9106B_DIM0        0x20
#define AW9106B_DIM1        0x21
#define AW9106B_DIM2        0x22
#define AW9106B_DIM3        0x23
#define AW9106B_DIM4        0x24
#define AW9106B_DIM5        0x25

#define AW9106B_RESET       0x7F
#define AW9106B_PWM_SIZE    6

#ifndef AW9106B_NUM
    #define AW9106B_NUM         2
#endif
#define TIMEOUT             100

typedef struct {
    i2c_led_t   i2c_led;
    uint8_t     pwm_buffer[AW9106B_PWM_SIZE];
    bool        pwm_dirty;
    bool        ready;
} aw9106b_t;

static aw9106b_t aw9106b_drivers[AW9106B_NUM];

#ifndef AW9106B_I2C_ID
#define AW9106B_I2C_ID     I2C_INSTANCE_1
#endif
static i2c_handle_t i2c_inst;

i2c_led_t *aw9106b_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(AW9106B_I2C_ID);
    }

    aw9106b_t *driver = &aw9106b_drivers[index];
    driver->i2c_led.addr = addr;
    driver->i2c_led.index = index;
    driver->i2c_led.led_start = led_start;
    driver->i2c_led.led_num = led_num;
    driver->i2c_led.data = driver;

    uint8_t data = 0;
    amk_error_t status = AMK_SUCCESS;
    status = i2c_write_reg(i2c_inst, addr, AW9106B_RESET, &data, 1, TIMEOUT);
    if (status) aw9106b_debug("failed to reset aw9106b, status:%d\n", status);

    wait_ms(1);
    // set max led current
    data = 0x03; // 37mA/4
    i2c_write_reg(i2c_inst, addr, AW9106B_CTL, &data, 1, TIMEOUT);
    if (status) aw9106b_debug("failed to set aw9106b current, status:%d\n", status);
    // set port to led mode
    data = 0;
    i2c_write_reg(i2c_inst, addr, AW9106B_CPMD_A, &data, 1, TIMEOUT);
    if (status) aw9106b_debug("failed to set aw9106b pin A mode, status:%d\n", status);
    i2c_write_reg(i2c_inst, addr, AW9106B_CPMD_B, &data, 1, TIMEOUT);
    if (status) aw9106b_debug("failed to set aw9106b pin B mode, status:%d\n", status);
    // clear pwm buff
    for (uint8_t i = 0; i < AW9106B_PWM_SIZE; i++) {
        driver->pwm_buffer[i] = 0;
    }

    driver->pwm_dirty = false;
    driver->ready = true;
    aw9106b_debug("AW9106B inited: addr:%d, index:%d\n", addr, index);
    return &driver->i2c_led;
}

void aw9106b_set_color(i2c_led_t *driver, int index, uint8_t red, uint8_t green, uint8_t blue)
{
    aw9106b_t * aw9106b = (aw9106b_t*)driver->data;

    if (index >= aw9106b->i2c_led.led_num) 
    {
        aw9106b_debug("AW9106B: set_color out of range: index=%d, max=%d\n", index, aw9106b->i2c_led.led_num);
        return;
    }

    rgb_led_t *led = &g_rgb_leds[index];
    if (aw9106b->pwm_buffer[led->r] != red) {
        aw9106b->pwm_buffer[led->r] = red;
        aw9106b->pwm_dirty = true;
    }
    if (aw9106b->pwm_buffer[led->g] != green) {
        aw9106b->pwm_buffer[led->g] = green;
        aw9106b->pwm_dirty = true;
    }
    if (aw9106b->pwm_buffer[led->b] != blue) {
        aw9106b->pwm_buffer[led->b] = blue;
        aw9106b->pwm_dirty = true;
    }
}

void aw9106b_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < driver->led_num; i++) {
        aw9106b_set_color(driver, i, red, green, blue);
    }
}

void aw9106b_update_buffers(i2c_led_t *driver)
{
    amk_error_t status = 0;

    aw9106b_t * aw9106b = (aw9106b_t*)driver->data;
    if (aw9106b->ready && aw9106b->pwm_dirty) {
        status = i2c_write_reg(i2c_inst, aw9106b->i2c_led.addr, AW9106B_DIM0, &aw9106b->pwm_buffer[AW_DIM0], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->i2c_led.addr, AW9106B_DIM1, &aw9106b->pwm_buffer[AW_DIM1], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM1 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->i2c_led.addr, AW9106B_DIM2, &aw9106b->pwm_buffer[AW_DIM2], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM2 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->i2c_led.addr, AW9106B_DIM3, &aw9106b->pwm_buffer[AW_DIM3], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM3 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->i2c_led.addr, AW9106B_DIM4, &aw9106b->pwm_buffer[AW_DIM4], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM4 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->i2c_led.addr, AW9106B_DIM5, &aw9106b->pwm_buffer[AW_DIM5], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM5 color status:%d\n", status);

        aw9106b->pwm_dirty = false;
    }
}

void aw9106b_uninit(i2c_led_t *driver)
{
    aw9106b_t * aw9106b = (aw9106b_t*)driver->data;
    if (!aw9106b->ready) return;

    aw9106b->ready = false;
}
