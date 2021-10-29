/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file matrix_scan.c
 *  keyboard matrix scanning implementation
 */

#include <string.h>
#include "matrix_scan.h"
#include "keyboard.h"
#include "print.h"
#include "timer.h"
#include "wait.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef MATRIX_SCAN_DEBUG
#define MATRIX_SCAN_DEBUG 0
#endif

#if MATRIX_SCAN_DEBUG
#define matrix_scan_debug  amk_printf
#else
#define matrix_scan_debug(...)
#endif

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

#ifndef MATRIX_I2C_PINS
    #if defined(MATRIX_DIRECT_PINS)
    static pin_t direct_pins[] = MATRIX_DIRECT_PINS;
    #else
    static pin_t col_pins[] = MATRIX_COL_PINS;
    static pin_t row_pins[] = MATRIX_ROW_PINS;
    #endif
#endif

#ifdef MATRIX_I2C_PINS
__attribute__((weak))
void matrix_i2c_init(void)
{
}

__attribute__((weak))
bool matrix_i2c_scan(matrix_row_t* raw)
{
    return false;
}

__attribute__((weak))
void matrix_i2c_prepare_sleep(void)
{}

__attribute__((weak))
bool matrix_i2c_check_boot(void)
{
    return false;
}

#endif


static bool debouncing = false;
static uint16_t debouncing_time = 0;

__attribute__((weak))
void matrix_init_kb(void)
{
}

__attribute__((weak))
void matrix_scan_kb(void)
{
}

__attribute__((weak))
void matrix_init_custom(void)
{
#ifdef MATRIX_I2C_PINS
    matrix_i2c_init();
#else
    #ifndef MATRIX_DIRECT_PINS
        for (int col = 0; col < MATRIX_COLS; col++) {
            gpio_set_output_pushpull(col_pins[col]);
            gpio_write_pin(col_pins[col], 0);
        }
        for (int row = 0; row < MATRIX_ROWS; row++) {
            gpio_set_input_pulldown(row_pins[row]);
        }
    #else
        for (int pin = 0; pin < sizeof(direct_pins)/sizeof(pin_t); pin++) {
            gpio_set_input_pullup(direct_pins[pin]);
        }
    #endif
#endif
}

void matrix_init(void)
{
    // initialize row and col
    matrix_init_custom();

    // initialize matrix state: all keys off
    memset(&matrix[0], 0, sizeof(matrix));
    memset(&matrix_debouncing[0], 0, sizeof(matrix_debouncing));

    matrix_init_kb();
}

__attribute__((weak))
bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
#ifdef MATRIX_I2C_PINS
    changed = matrix_i2c_scan(raw);
#else
    #ifndef MATRIX_DIRECT_PINS
        for (int col = 0; col < MATRIX_COLS; col++) {
            gpio_write_pin(col_pins[col], 1);
            wait_us(30);

            for(uint8_t row = 0; row < MATRIX_ROWS; row++) {
                matrix_row_t last_row_value    = raw[row];
                matrix_row_t current_row_value = last_row_value;

                if (gpio_read_pin(row_pins[row])) {
                    current_row_value |= (1 << col);
                } else {
                    current_row_value &= ~(1 << col);
                }

                if (last_row_value != current_row_value) {
                    raw[row] = current_row_value;
                    changed = true;
                }
            }
            gpio_write_pin(col_pins[col], 0);
        }
    #else
        for (int col = 0; col < MATRIX_COLS; col++) {
            for(uint8_t row = 0; row < MATRIX_ROWS; row++) {
                matrix_row_t last_row_value    = matrix_debouncing[row];
                matrix_row_t current_row_value = last_row_value;

                if ( gpio_read_pin(direct_pins[MATRIX_COLS*row+col])) {
                    current_row_value &= ~(1 << col);
                } else {
                    current_row_value |= (1 << col);
                }

                if (last_row_value != current_row_value) {
                    matrix_debouncing[row] = current_row_value;
                    changed = true;
                }
            }
        }
    #endif
#endif
    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            matrix_scan_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}

uint8_t matrix_scan(void)
{
    matrix_scan_debug("matrix scan start\n");
    bool changed = matrix_scan_custom(&matrix_debouncing[0]);

    if (changed && !debouncing) {
        debouncing = true;
        debouncing_time = timer_read();
    }

    if (debouncing && timer_elapsed(debouncing_time) >= DEBOUNCE) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix[row] = matrix_debouncing[row];
        }
        debouncing = false;
    }

    matrix_scan_kb();
    return 1;
}

bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        amk_printf("%x\n", matrix_get_row(row));
    }
}

bool matrix_is_off(void)
{
    for (int i = 0; i < MATRIX_ROWS; i++) {
        if (matrix[i] != 0) {
            return false;
        }
    }
    return true;
}

#if defined(NRF52) || defined(NRF52840_XXAA)
#include "nrf_gpio.h"
#include "common_config.h"

void matrix_prepare_sleep(void)
{
#ifdef MATRIX_I2C_PINS
    matrix_i2c_prepare_sleep();
#else
    for (uint32_t i = 0; i < NUMBER_OF_PINS; i++) {
        nrf_gpio_cfg_default(i);
    }

    for (uint32_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_output(col_pins[i]);
        nrf_gpio_pin_set(col_pins[i]);
    }

    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_sense_input(row_pins[i], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }
#endif
}

bool matrix_check_boot(void)
{
#ifdef MATRIX_I2C_PINS
    return matrix_i2c_check_boot();
#else
    nrf_gpio_cfg_input(row_pins[0], NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_output(col_pins[0]);
    wait_ms(1);
    nrf_gpio_pin_write(col_pins[0], 1);
    wait_ms(1);
    return nrf_gpio_pin_read(row_pins[0]) ? true : false;
#endif
}

#else

void matrix_prepare_sleep(void) {}

#endif