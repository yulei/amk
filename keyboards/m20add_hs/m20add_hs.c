/**
 * @file m20add_hs.c
 */

#include <stdbool.h>

#include "m20add_hs.h"
#include "led.h"
#include "wait.h"

#include "pca9535.h"
#include "is31fl3731.h"
#include "aw9106b.h"
#include "rgb_driver.h"
#include "rgb_indicator.h"
#include "rgb_linear.h"

#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef HS_DEBUG
#define HS_DEBUG 0
#endif

#if HS_DEBUG
#define hs_debug  amk_printf
#else
#define hs_debug(...)
#endif

rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, C1_1,   C3_2,   C4_2},
    {0, C1_2,   C2_2,   C4_3},
    {0, C1_3,   C2_3,   C3_3},
    {0, C1_4,   C2_4,   C3_4},
    {0, C1_5,   C2_5,   C3_5},
    {0, C1_6,   C2_6,   C3_6},
    {0, C1_7,   C2_7,   C3_7},
    {0, C1_8,   C2_8,   C3_8},

    {0, C9_1,   C8_1,   C7_1},
    {0, C9_2,   C8_2,   C7_2},
    {0, C9_3,   C8_3,   C7_3},
    {0, C9_4,   C8_4,   C7_4},
    {0, C9_5,   C8_5,   C7_5},
    {0, C9_6,   C8_6,   C7_6},
    {0, C9_7,   C8_7,   C6_6},
    {0, C9_8,   C7_7,   C6_7},

    {0, C1_9,   C3_10,  C4_10},
    {0, C1_10,  C2_10,  C4_11},
    {0, C1_11,  C2_11,  C3_11},
    {0, C1_12,  C2_12,  C3_12},

    // indicator leds
    {1, AW_DIM1, AW_DIM2, AW_DIM0},  // CAPS
    {1, AW_DIM4, AW_DIM5, AW_DIM3},  // ESC
    {2, AW_DIM4, AW_DIM5, AW_DIM3},  // SCROLL
    {2, AW_DIM1, AW_DIM2, AW_DIM0},  // NUM
};

rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    { RGB_DRIVER_IS31FL3731, 0xE8, 0, 0, 20},
    { RGB_DRIVER_AW9106B, 0xB6, 0, 20, 2},
    { RGB_DRIVER_AW9106B, 0xB2, 1, 22, 2},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    { 0, 0, 16},
    { 1, 16, 4},
};

uint8_t g_rgb_indicator_index[RGB_INDICATOR_LED_NUM] = { 20,21,22,23};

#define CAPS_LED    0
#define SCROLL_LED  2
#define NUM_LED     3

static const pin_t row_pins[MATRIX_ROWS] = { 11,14,15,16,17,29};
static uint8_t col_pins[MATRIX_COLS] = {
    PCA9535_PIN_DEF(0,0),
    PCA9535_PIN_DEF(0,1),
    PCA9535_PIN_DEF(0,2),
    PCA9535_PIN_DEF(0,3),
    PCA9535_PIN_DEF(0,4),
    PCA9535_PIN_DEF(0,5),
    PCA9535_PIN_DEF(0,6),
    PCA9535_PIN_DEF(0,7),
    PCA9535_PIN_DEF(1,7),
    PCA9535_PIN_DEF(1,6),
    PCA9535_PIN_DEF(1,5),
    PCA9535_PIN_DEF(1,4),
    PCA9535_PIN_DEF(1,3),
    PCA9535_PIN_DEF(1,2),
    PCA9535_PIN_DEF(1,1),
    PCA9535_PIN_DEF(1,0),
};

static void set_pin(uint8_t pin)
{
    uint8_t mask = (1<<PCA9535_PIN(pin));

    pca9535_write_port(PCA9535_PORT(pin), mask);
}

static void set_port(uint8_t port, uint8_t mask)
{
    pca9535_write_port(port, mask);
}

void matrix_i2c_init(void)
{
    pca9535_init();
    // set port0 output
    pca9535_write_config(PCA9535_PORT0, 0);
    // set port1 output
    pca9535_write_config(PCA9535_PORT1, 0);

    // clear output
    pca9535_write_port(PCA9535_PORT0, 0);
    pca9535_write_port(PCA9535_PORT1, 0);

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        gpio_set_input_pulldown(row_pins[row]);
    }

    hs_debug("matrix i2c init finished\n");
}

bool matrix_i2c_scan(matrix_row_t* raw)
{
    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        set_pin(col_pins[col]);

        //wait_us(30);

        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
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
        // Unselect col
        set_port(PCA9535_PORT(col_pins[col]), 0);
    }

    return changed;
}

void matrix_i2c_prepare_sleep(void)
{
    // put col to high
    set_port(PCA9535_PORT0, 0xFF);
    set_port(PCA9535_PORT1, 0xFF);

    // set row pin in sense mode
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        nrf_gpio_cfg_sense_input(row_pins[row], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }

    hs_debug("matrix i2c sleep prepared\n");
}

/*
__attribute__((weak))
bool matrix_i2c_check_boot(void)
{
    return false;
}*/


void led_set(uint8_t led)
{
#ifdef RGB_INDICATOR_ENABLE
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_indicator_set(CAPS_LED, 0xFF, 0xFF, 0xFF);
        hs_debug("set caps led\n");
    } else {
        rgb_indicator_set(CAPS_LED, 0, 0, 0);
        hs_debug("clear caps led\n");
    }

    if (led & (1 << USB_LED_SCROLL_LOCK)) {
        rgb_indicator_set(SCROLL_LED, 0xFF, 0xFF, 0xFF);
        hs_debug("set scroll led\n");
    } else {
        rgb_indicator_set(SCROLL_LED, 0, 0, 0);
        hs_debug("clear scroll led\n");
    }
    
    //if (led & (1 << USB_LED_NUM_LOCK)) {
    //    rgb_indicator_set(NUM_LED, 0xFF, 0xFF, 0xFF);
    //} else {
    //    rgb_indicator_set(NUM_LED, 0, 0, 0);
    //}
#endif
}
