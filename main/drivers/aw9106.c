/**
 * @file aw9106.c
 * @brief driver for aw9106
 */

#include <stdbool.h>

#include "aw9106.h"
#include "quantum.h"
#include "i2c_master.h"
#include "wait.h"
#include "print.h"

#define i2c_write_reg i2c_writeReg

#define TIMEOUT             100

#define PWM2BUF(x) ((x) - AW9106B_PWM_BASE)
static uint8_t aw9106b_pwm_buf[AW9106B_PWM_SIZE];
static bool    aw9106b_pwm_dirty = false;
static bool    aw9106b_ready     = false;

void aw9106b_init(uint8_t addr)
{
#ifdef AW9106B_SHDN_PIN
    setPinOutput(AW9106B_SHDN_PIN);
    writePin(AW9106B_SHDN_PIN, 1);
    wait_ms(1);
#endif

    //i2c_init();
    // reset chip
    uint8_t data = 0;
    i2c_status_t status = I2C_STATUS_SUCCESS;
    status = i2c_write_reg(addr, AW9106B_RESET, &data, 1, TIMEOUT);
    if (status) xprintf("failed to reset aw9106b, status:%d\n", status);

    wait_ms(1);
    // set max led current
    data = 0x03; // 37mA/4
    i2c_write_reg(addr, AW9106B_CTL, &data, 1, TIMEOUT);
    if (status) xprintf("failed to set aw9106b current, status:%d\n", status);
    // set port to led mode
    data = 0;
    i2c_write_reg(addr, AW9106B_CPMD_A, &data, 1, TIMEOUT);
    if (status) xprintf("failed to set aw9106b pin A mode, status:%d\n", status);
    i2c_write_reg(addr, AW9106B_CPMD_B, &data, 1, TIMEOUT);
    if (status) xprintf("failed to set aw9106b pin B mode, status:%d\n", status);
    // clear pwm buff
    for (uint8_t i = 0; i < AW9106B_PWM_SIZE; i++) {
        aw9106b_pwm_buf[i] = 0;
    }

    aw9106b_pwm_dirty = false;
    aw9106b_ready = true;
}

void aw9106b_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index >= RGBLED_NUM) return;

    aw9106b_led_t led = g_aw9106b_leds[index];
    if (aw9106b_pwm_buf[PWM2BUF(led.r)] != red) {
        aw9106b_pwm_buf[PWM2BUF(led.r)] = red;
        aw9106b_pwm_dirty = true;
    }
     if (aw9106b_pwm_buf[PWM2BUF(led.g)] != green) {
        aw9106b_pwm_buf[PWM2BUF(led.g)] = green;
        aw9106b_pwm_dirty = true;
    }
     if (aw9106b_pwm_buf[PWM2BUF(led.b)] != blue) {
        aw9106b_pwm_buf[PWM2BUF(led.b)] = blue;
        aw9106b_pwm_dirty = true;
    }
}

void aw9106b_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < RGBLED_NUM; i++) {
        aw9106b_set_color(i, red, green, blue);
    }
    aw9106b_pwm_dirty = true;
}

void aw9106b_update_buffers(uint8_t addr)
{
    i2c_status_t status = 0;

    if (aw9106b_ready && aw9106b_pwm_dirty) {
        for (uint8_t i = 0; i < RGBLED_NUM; i++){
            aw9106b_led_t led = g_aw9106b_leds[i];
            status = i2c_write_reg(addr, led.r, &aw9106b_pwm_buf[PWM2BUF(led.r)], 1, TIMEOUT);
            if (status) xprintf("failed to set red color at:%d, status:%d\n", i, status);
            i2c_write_reg(addr, led.g, &aw9106b_pwm_buf[PWM2BUF(led.g)], 1, TIMEOUT);
            if (status) xprintf("failed to set green color at:%d\n, status:%d", i, status);
            i2c_write_reg(addr, led.b, &aw9106b_pwm_buf[PWM2BUF(led.b)], 1, TIMEOUT);
            if (status) xprintf("failed to set blue color at:%d, status:%d\n", i, status);
        }
        aw9106b_pwm_dirty = false;
    }
}

void aw9106b_uninit(uint8_t addr)
{}
