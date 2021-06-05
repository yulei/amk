/**
 * @file aw9106b.c
 * @brief driver for aw9106b
 */

#include <stdbool.h>

#include "aw9106b.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#define AW9106B_NUM         2
#define TIMEOUT             100

#define PWM2BUF(x) ((x) - AW9106B_PWM_BASE)
static uint8_t aw9106b_pwm_buf[AW9106B_NUM][AW9106B_PWM_SIZE];
static bool    aw9106b_pwm_dirty[AW9106B_NUM] = {false, false};
static bool    aw9106b_ready[AW9106B_NUM] = {false, false};

void aw9106b_init(uint8_t addr, uint8_t index)
{
    i2c_init();
    uint8_t data = 0;
    amk_error_t status = AMK_SUCCESS;
    status = i2c_write_reg(addr, AW9106B_RESET, &data, 1, TIMEOUT);
    if (status) amk_printf("failed to reset aw9106b, status:%d\n", status);

    wait_ms(1);
    // set max led current
    data = 0x03; // 37mA/4
    i2c_write_reg(addr, AW9106B_CTL, &data, 1, TIMEOUT);
    if (status) amk_printf("failed to set aw9106b current, status:%d\n", status);
    // set port to led mode
    data = 0;
    i2c_write_reg(addr, AW9106B_CPMD_A, &data, 1, TIMEOUT);
    if (status) amk_printf("failed to set aw9106b pin A mode, status:%d\n", status);
    i2c_write_reg(addr, AW9106B_CPMD_B, &data, 1, TIMEOUT);
    if (status) amk_printf("failed to set aw9106b pin B mode, status:%d\n", status);
    // clear pwm buff
    for (uint8_t i = 0; i < AW9106B_PWM_SIZE; i++) {
        aw9106b_pwm_buf[index][i] = 0;
    }

    aw9106b_pwm_dirty[index] = false;
    aw9106b_ready[index] = true;
    amk_printf("AW9106B inited: addr:%d, index:%d\n", addr, index);
}

void aw9106b_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index >= RGB_LED_NUM) return;

    rgb_led_t led = g_aw9106b_leds[index];
    if (aw9106b_pwm_buf[led.driver][PWM2BUF(led.r)] != red) {
        aw9106b_pwm_buf[led.driver][PWM2BUF(led.r)] = red;
        aw9106b_pwm_dirty[led.driver] = true;
    }
     if (aw9106b_pwm_buf[led.driver][PWM2BUF(led.g)] != green) {
        aw9106b_pwm_buf[led.driver][PWM2BUF(led.g)] = green;
        aw9106b_pwm_dirty[led.driver] = true;
    }
     if (aw9106b_pwm_buf[led.driver][PWM2BUF(led.b)] != blue) {
        aw9106b_pwm_buf[led.driver][PWM2BUF(led.b)] = blue;
        aw9106b_pwm_dirty[led.driver] = true;
    }
}

void aw9106b_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < RGB_LED_NUM; i++) {
        aw9106b_set_color(i, red, green, blue);
    }
}

void aw9106b_update_buffers(uint8_t addr, uint8_t index)
{
    amk_error_t status = 0;

    if (aw9106b_ready[index] && aw9106b_pwm_dirty[index]) {
        status = i2c_write_reg(addr, AW9106B_DIM0, &aw9106b_pwm_buf[index][PWM2BUF(AW9106B_DIM0)], 1, TIMEOUT);
        if (status) amk_printf("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(addr, AW9106B_DIM1, &aw9106b_pwm_buf[index][PWM2BUF(AW9106B_DIM1)], 1, TIMEOUT);
        if (status) amk_printf("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(addr, AW9106B_DIM2, &aw9106b_pwm_buf[index][PWM2BUF(AW9106B_DIM2)], 1, TIMEOUT);
        if (status) amk_printf("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(addr, AW9106B_DIM3, &aw9106b_pwm_buf[index][PWM2BUF(AW9106B_DIM3)], 1, TIMEOUT);
        if (status) amk_printf("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(addr, AW9106B_DIM4, &aw9106b_pwm_buf[index][PWM2BUF(AW9106B_DIM4)], 1, TIMEOUT);
        if (status) amk_printf("failed to set DIM0 color status:%d\n", status);
        status = i2c_write_reg(addr, AW9106B_DIM5, &aw9106b_pwm_buf[index][PWM2BUF(AW9106B_DIM5)], 1, TIMEOUT);
        if (status) amk_printf("failed to set DIM0 color status:%d\n", status);

        aw9106b_pwm_dirty[index] = false;
    }
}

void aw9106b_uninit(uint8_t addr, uint8_t index)
{}
