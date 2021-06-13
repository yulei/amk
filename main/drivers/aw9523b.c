/**
 * @file aw9523b.c
 * @brief driver implementation of aw9523b
 */

#include <stdbool.h>
#include "aw9523b.h"
#include "rgb_common.h"
#include "wait.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef AW9523B_DEBUG
#define AW9523B_DEBUG 1
#endif

#if AW9523B_DEBUG
#define aw9523b_debug  amk_printf
#else
#define aw9523b_debug(...)
#endif

#define AW9523B_P0_INPUT    0x00
#define AW9523B_P1_INPUT    0x01
#define AW9523B_P0_OUTPUT   0x02
#define AW9523B_P1_OUTPUT   0x03
#define AW9523B_P0_CONF     0x04
#define AW9523B_P1_CONF     0x05
#define AW9523B_P0_INT      0x06
#define AW9523B_P1_INT      0x07

#define AW9523B_ID          0x10
#define AW9523B_CTL         0x11
#define AW9523B_P0_LED      0x12
#define AW9523B_P1_LED      0x13

#define AW9523B_PWM_BASE    0x20

#define AW9523B_P10_PWM     0x20
#define AW9523B_P11_PWM     0x21
#define AW9523B_P12_PWM     0x22
#define AW9523B_P13_PWM     0x23
#define AW9523B_P00_PWM     0x24
#define AW9523B_P01_PWM     0x25
#define AW9523B_P02_PWM     0x26
#define AW9523B_P03_PWM     0x27
#define AW9523B_P04_PWM     0x28
#define AW9523B_P05_PWM     0x29
#define AW9523B_P06_PWM     0x2A
#define AW9523B_P07_PWM     0x2B
#define AW9523B_P14_PWM     0x2C
#define AW9523B_P15_PWM     0x2D
#define AW9523B_P16_PWM     0x2E
#define AW9523B_P17_PWM     0x2F

#define AW9523B_PWM_SIZE    16
#define AW9523B_RESET       0x7F

#ifndef AW9523B_NUM
    #define AW9523B_NUM     1
#endif

#define TIMEOUT             100

typedef struct {
    i2c_led_t   i2c_led;
    uint8_t     pwm_buffer[AW9523B_PWM_SIZE];
    bool        pwm_dirty;
    bool        ready;
} aw9523b_t;

static aw9523b_t aw9523b_drivers[AW9523B_NUM];

#ifndef AW9523B_I2C_ID
#define AW9523B_I2C_ID     I2C_INSTANCE_1
#endif
static i2c_handle_t i2c_inst;

bool aw9523b_available(uint8_t addr)
{
    bool need_release = false;
    if (!i2c_inst) {
        i2c_inst = i2c_init(AW9523B_I2C_ID);
    }

#ifdef RGBLIGHT_EN_PIN
    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#endif
    uint8_t data = 0;
    amk_error_t ec = i2c_write_reg(i2c_inst, addr, AW9523B_RESET, &data, 1, TIMEOUT);

#ifdef RGBLIGHT_EN_PIN
    gpio_set_input_floating(RGBLIGHT_EN_PIN);
#endif
    bool available = (ec == AMK_SUCCESS) ? true : false;
    if (!available) {
        aw9523b_debug("aw9523b not available: %d, release=%d\n", ec, need_release);
        if (need_release) {
            i2c_uninit(i2c_inst);
            i2c_inst = NULL;
        }
    }

    return available;
}

i2c_led_t *aw9523b_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(AW9523B_I2C_ID);
    }

    aw9523b_t *driver = &aw9523b_drivers[index];
    driver->i2c_led.addr = addr;
    driver->i2c_led.index = index;
    driver->i2c_led.led_start = led_start;
    driver->i2c_led.led_num = led_num;
    driver->i2c_led.data = driver;

    // reset chip
    uint8_t data = 0;
    amk_error_t ec = i2c_write_reg(i2c_inst, addr, AW9523B_RESET, &data, 1, TIMEOUT);
    aw9523b_debug("aw9523b write reset result: %d\n", ec);

    wait_ms(1);
    // set max led current
    data = 0x03; // 37mA/4
    i2c_write_reg(i2c_inst, addr, AW9523B_CTL, &data, 1, TIMEOUT);
    // set port to led mode
    data = 0;
    i2c_write_reg(i2c_inst, addr, AW9523B_P0_LED, &data, 1, TIMEOUT);
    i2c_write_reg(i2c_inst, addr, AW9523B_P1_LED, &data, 1, TIMEOUT);
    // clear pwm buff
    for (uint8_t i = 0; i < AW9523B_PWM_SIZE; i++) {
        driver->pwm_buffer[i] = 0;
    }
    driver->pwm_dirty = false;
    driver->ready = true;

    return &driver->i2c_led;
}

void aw9523b_set_color(i2c_led_t *driver, int index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index >= driver->led_num) return;

    aw9523b_t *aw9523b = (aw9523b_t *)driver->data;
    rgb_led_t *led = &g_rgb_leds[index];
    if (aw9523b->pwm_buffer[led->r] != red) {
        aw9523b->pwm_buffer[led->r] = red;
        aw9523b->pwm_dirty = true;
    }
     if (aw9523b->pwm_buffer[led->g] != green) {
        aw9523b->pwm_buffer[led->g] = green;
        aw9523b->pwm_dirty = true;
    }
     if (aw9523b->pwm_buffer[led->b] != blue) {
        aw9523b->pwm_buffer[led->b] = blue;
        aw9523b->pwm_dirty = true;
    }
}

void aw9523b_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < driver->led_num; i++) {
        aw9523b_set_color(driver, i, red, green, blue);
    }
}

void aw9523b_update_buffers(i2c_led_t *driver)
{
    aw9523b_t *aw9523b = (aw9523b_t *)driver->data;
    if (aw9523b->ready && aw9523b->pwm_dirty) {
        for (uint8_t i = 0; i < aw9523b->i2c_led.led_num; i++){
            rgb_led_t *led = &g_rgb_leds[i];
            i2c_write_reg(i2c_inst, aw9523b->i2c_led.addr, led->r+AW9523B_PWM_BASE, &aw9523b->pwm_buffer[led->r], 1, TIMEOUT);
            i2c_write_reg(i2c_inst, aw9523b->i2c_led.addr, led->g+AW9523B_PWM_BASE, &aw9523b->pwm_buffer[led->g], 1, TIMEOUT);
            i2c_write_reg(i2c_inst, aw9523b->i2c_led.addr, led->b+AW9523B_PWM_BASE, &aw9523b->pwm_buffer[led->b], 1, TIMEOUT);
        }
        aw9523b->pwm_dirty = false;
    }
}

void aw9523b_uninit(i2c_led_t *driver)
{
    aw9523b_t *aw9523b = (aw9523b_t *)driver->data;
    if (!aw9523b->ready) return;

    aw9523b->ready = false;
}
