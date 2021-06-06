/**
 * @file abelx_hs.c
 */

#include <stdbool.h>

#include "abelx_hs.h"
#include "gpio_pin.h"
#include "pca9535.h"
#include "rgb_effects.h"
#include "amk_printf.h"
#include "led.h"
#include "aw9106b.h"
#include "rgb_indicator.h"
#include "rgb_ring.h"
#include "wait.h"

#ifndef HS_DEBUG
#define HS_DEBUG 0
#endif

#if HS_DEBUG
#define hs_debug  amk_printf
#else
#define hs_debug(...)
#endif

rgb_led_t g_aw9106b_leds[] = {
    {0, AW9106B_DIM1, AW9106B_DIM2, AW9106B_DIM0},  // CAPS
    {0, AW9106B_DIM4, AW9106B_DIM5, AW9106B_DIM3},  // ESC
    {1, AW9106B_DIM4, AW9106B_DIM5, AW9106B_DIM3},  // SCROLL
    {1, AW9106B_DIM1, AW9106B_DIM2, AW9106B_DIM0},  // NUM
};

#define CAPS_LED    0
#define SCROLL_LED  2
#define NUM_LED     3

static const pin_t row_pins[MATRIX_ROWS] = { 8, 11,12,13,14,29};
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

    gpio_set_output_pushpull(RGBLIGHT_EN_PIN);
    gpio_write_pin(RGBLIGHT_EN_PIN, 1);
    wait_ms(1);
#ifdef INDICATOR_ENABLE
    rgb_indicator_init();
#endif

#ifdef RGB_RING_ENABLE
    rgb_ring_init();
#endif

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

#ifdef INDICATOR_ENABLE
    rgb_indicator_task();
#endif

#ifdef RGB_RING_ENABLE
    rgb_ring_task();
#endif
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

#ifdef INDICATOR_ENABLE
    rgb_indicator_uninit();
#endif
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
#ifdef INDICATOR_ENABLE
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        rgb_indicator_set(CAPS_LED, 0xFF, 0xFF, 0xFF);
    } else {
        rgb_indicator_set(CAPS_LED, 0, 0, 0);
    }

    if (led & (1 << USB_LED_SCROLL_LOCK)) {
        rgb_indicator_set(SCROLL_LED, 0xFF, 0xFF, 0xFF);
    } else {
        rgb_indicator_set(SCROLL_LED, 0, 0, 0);
    }
    
    //if (led & (1 << USB_LED_NUM_LOCK)) {
    //    rgb_indicator_set(NUM_LED, 0xFF, 0xFF, 0xFF);
    //} else {
    //    rgb_indicator_set(NUM_LED, 0, 0, 0);
    //}
#endif
}
