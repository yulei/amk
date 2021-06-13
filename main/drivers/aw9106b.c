/**
 * @file aw9106b.c
 * @brief driver for aw9106b
 */

#include <stdbool.h>

#include "aw9106b.h"
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

#ifndef AW9106B_NUM
    #define AW9106B_NUM         2
#endif
#define TIMEOUT             100

typedef struct {
    awinic_t awinic;
    uint8_t pwm_buffer[AW9106B_PWM_SIZE];
    bool pwm_dirty;
    bool ready;
} aw9106b_t;

#define PWM2BUF(x) ((x) - AW9106B_PWM_BASE)

static aw9106b_t aw9106b_drivers[AW9106B_NUM];

//static uint8_t aw9106b_pwm_buf[AW9106B_NUM][AW9106B_PWM_SIZE];
//static bool    aw9106b_pwm_dirty[AW9106B_NUM] = {false, false};
//static bool    aw9106b_ready[AW9106B_NUM] = {false, false};

#ifndef AW9106B_I2C_ID
#define AW9106B_I2C_ID     I2C_INSTANCE_1
#endif
static i2c_handle_t i2c_inst;

awinic_t *aw9106b_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(AW9106B_I2C_ID);
    }

    aw9106b_t *driver = &aw9106b_drivers[index];
    driver->awinic.addr = addr;
    driver->awinic.index = index;
    driver->awinic.led_start = led_start;
    driver->awinic.led_num = led_num;
    driver->awinic.data = driver;

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
    return &driver->awinic;
}

void aw9106b_set_color(awinic_t *driver, int index, uint8_t red, uint8_t green, uint8_t blue)
{
    aw9106b_t * aw9106b = (aw9106b_t*)driver->data;

    if (index >= aw9106b->awinic.led_num) 
    {
        aw9106b_debug("AW9106B: set_color out of range: index=%d, max=%d\n", index, aw9106b->awinic.led_num);
        return;
    }

    rgb_led_t led = g_rgb_leds[index];
    if (aw9106b->pwm_buffer[PWM2BUF(led.r)] != red) {
        aw9106b->pwm_buffer[PWM2BUF(led.r)] = red;
        aw9106b->pwm_dirty = true;
    }
    if (aw9106b->pwm_buffer[PWM2BUF(led.g)] != green) {
        aw9106b->pwm_buffer[PWM2BUF(led.g)] = green;
        aw9106b->pwm_dirty = true;
    }
    if (aw9106b->pwm_buffer[PWM2BUF(led.b)] != blue) {
        aw9106b->pwm_buffer[PWM2BUF(led.b)] = blue;
        aw9106b->pwm_dirty = true;
    }
}

void aw9106b_set_color_all(awinic_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < driver->led_num; i++) {
        aw9106b_set_color(driver, i, red, green, blue);
    }
}

void aw9106b_update_buffers(awinic_t *driver)
{
    amk_error_t status = 0;

    aw9106b_t * aw9106b = (aw9106b_t*)driver->data;
    if (aw9106b->ready && aw9106b->pwm_dirty) {
        status = i2c_write_reg(i2c_inst, aw9106b->awinic.addr, AW9106B_DIM0, &aw9106b->pwm_buffer[PWM2BUF(AW9106B_DIM0)], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->awinic.addr, AW9106B_DIM1, &aw9106b->pwm_buffer[PWM2BUF(AW9106B_DIM1)], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM1 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->awinic.addr, AW9106B_DIM2, &aw9106b->pwm_buffer[PWM2BUF(AW9106B_DIM2)], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM2 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->awinic.addr, AW9106B_DIM3, &aw9106b->pwm_buffer[PWM2BUF(AW9106B_DIM3)], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM3 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->awinic.addr, AW9106B_DIM4, &aw9106b->pwm_buffer[PWM2BUF(AW9106B_DIM4)], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM4 color status:%d\n", status);
        status = i2c_write_reg(i2c_inst, aw9106b->awinic.addr, AW9106B_DIM5, &aw9106b->pwm_buffer[PWM2BUF(AW9106B_DIM5)], 1, TIMEOUT);
        if (status) aw9106b_debug("failed to set DIM5 color status:%d\n", status);

        aw9106b->pwm_dirty = false;
    }
}

void aw9106b_uninit(awinic_t *driver)
{
    aw9106b_t * aw9106b = (aw9106b_t*)driver->data;
    if (!aw9106b->ready) return;

    aw9106b->ready = false;
}
