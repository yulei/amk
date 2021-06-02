/* Copyright 2017 Jason Williams
 * Copyright 2018 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file is31fl3236.c
 * @brief driver implementation for is31fl3236
 */

#include "is31fl3236.h"
#include "i2c_master.h"
#include "wait.h"

// This is a 7-bit address, that gets left-shifted and bit 0
// set to 0 for write, 1 for read (as per I2C protocol)
// The address will vary depending on your wiring:
// 0b0111100 AD <-> GND
// 0b0111111 AD <-> VCC
// 0b0111101 AD <-> SCL
// 0b0111110 AD <-> SDA

// This is the full 8-bit address
#define ISSI_ADDR_DEFAULT 0b01111000

// These are the register addresses
#define ISSI_REG_SHUTDOWN       0x00
#define ISSI_REG_PWM            0x01
#define ISSI_REG_UPDATE         0x25
#define ISSI_REG_CONTROL        0x26
#define ISSI_REG_GLOBAL_CONTROL 0x4A
#define ISSI_REG_FREQUENCY      0x4B
#define ISSI_REG_RESET          0x4F


#ifndef ISSI_TIMEOUT
#    define ISSI_TIMEOUT 100
#endif

// Transfer buffer for TWITransmitData()
uint8_t g_twi_transfer_buffer[40] = {0};

uint8_t g_pwm_buffer[36] = {0};
bool    g_pwm_buffer_update_required = false;

uint8_t g_led_control_registers[36] = {0};
bool    g_led_control_registers_update_required = false;

void IS31FL3236_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer)
{
    i2c_writeReg(addr, ISSI_REG_PWM, pwm_buffer, 36, ISSI_TIMEOUT);

    // update the pwm buffer
    uint8_t data = 0;
    i2c_writeReg(addr, ISSI_REG_UPDATE, &data, 1, ISSI_TIMEOUT);
}

void IS31FL3236_init(uint8_t addr)
{
    i2c_init();
    // Reset 3236 to default state
    uint8_t data = 0;
    i2c_writeReg(addr, ISSI_REG_RESET, &data, 1, ISSI_TIMEOUT);

    wait_ms(10);

    // Turn off software shutdown
    data = 1;
    i2c_writeReg(addr, ISSI_REG_SHUTDOWN, &data, 1, ISSI_TIMEOUT);

    // turn on all LEDs in the LED control register
    for (int i = 0; i < 36; i++) {
        g_led_control_registers[i] = 0x01;
    }
    i2c_writeReg(addr, ISSI_REG_CONTROL, &g_led_control_registers[0], 36, ISSI_TIMEOUT);

    // set PWM on all LEDs
    for (int i = 0; i < 36; i++) {
        g_pwm_buffer[i] = 0;
    }
    i2c_writeReg(addr, ISSI_REG_PWM, &g_pwm_buffer[0], 36, ISSI_TIMEOUT);

    // update PWM and control values
    data = 0;
    i2c_writeReg(addr, ISSI_REG_UPDATE, &data, 1, ISSI_TIMEOUT);
}

void IS31FL3236_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index >= 0 && index < DRIVER_LED_TOTAL) {
        is31_led led = g_is31_leds[index];

        g_pwm_buffer[led.r - ISSI_REG_PWM]   = red;
        g_pwm_buffer[led.g - ISSI_REG_PWM]   = green;
        g_pwm_buffer[led.b - ISSI_REG_PWM]   = blue;
        g_pwm_buffer_update_required = true;
    }
}

void IS31FL3236_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        IS31FL3236_set_color(i, red, green, blue);
    }
}

void IS31FL3236_update_pwm_buffers(uint8_t addr) {
    if (g_pwm_buffer_update_required) {
        IS31FL3236_write_pwm_buffer(addr, &g_pwm_buffer[0]);
    }
    g_pwm_buffer_update_required = false;
}
