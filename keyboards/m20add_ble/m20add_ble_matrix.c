/**
 * @file m20add_ble_matrix.c
 */

#include <stdint.h>
#include "matrix.h"
#include "wait.h"
#include "led.h"
#include "tca6424.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef I2C_MATRIX_DEBUG
#define I2C_MATRIX_DEBUG 1
#endif

#if I2C_MATRIX_DEBUG
#define i2c_matrix_debug  amk_printf
#else
#define i2c_matrix_debug(...)
#endif

static const uint16_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
static uint8_t row_mask[] = {ROW1_MASK,ROW2_MASK,ROW3_MASK,ROW4_MASK,ROW5_MASK,ROW6_MASK};
static uint8_t col_mask[] = {COL1_MASK, COL2_MASK, COL3_MASK, COL4_MASK, COL5_MASK, COL6_MASK, COL7_MASK, COL8_MASK, COL9_MASK, COL10_MASK, COL11_MASK, COL12_MASK, COL13_MASK, COL14_MASK, COL15_MASK, COL16_MASK};

static void set_pin(uint16_t pin)
{
    uint8_t data = tca6424_read_port(GET_PORT(pin));
    data |= ( 1 << GET_PIN(pin));
    tca6424_write_port(GET_PORT(pin), data);
}

static void clear_pin(uint16_t pin)
{
    uint8_t data = tca6424_read_port(GET_PORT(pin));
    data &= ~( 1 << GET_PIN(pin));
    tca6424_write_port(GET_PORT(pin), data);
}

/*
static uint8_t read_pin(uint16_t pin)
{
    uint8_t data = tca6424_read_port(GET_PORT(pin));
    return (data & (1<<GET_PIN(pin))) ? 1 : 0;
}*/

void matrix_i2c_init(void)
{
    tca6424_init();
    // set port0
    tca6424_write_config(TCA6424_PORT0, 0);
    // set port1
    tca6424_write_config(TCA6424_PORT1, 0);
    // set port2
    tca6424_write_config(TCA6424_PORT2, 0xF5);

    // clear output
    tca6424_write_port(TCA6424_PORT0, 0);
    tca6424_write_port(TCA6424_PORT1, 0);
    tca6424_write_port(TCA6424_PORT2, 0);

    i2c_matrix_debug("matrix i2c init finished\n");
}

bool matrix_i2c_scan(matrix_row_t* current_matrix)
{
    bool changed = false;
    uint8_t p0_data = tca6424_read_port(TCA6424_PORT0);

    for (int col = 0; col < MATRIX_COLS; col++) {
        // Select col and wait for col selecton to stabilize
        switch(col) {
        case 0:
            set_pin(col_pins[col]);
            break;
        case 1 ... 8:
            tca6424_write_port(TCA6424_PORT1, col_mask[col]);
            break;
        default:
            tca6424_write_port(TCA6424_PORT0, col_mask[col]|(p0_data&0x01));
            break;
        }

        //wait_ms(1);

        // read row port for all rows
        uint8_t row_value = tca6424_read_port(ROW_PORT);
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            uint8_t tmp = row;
            // Store last value of row prior to reading
            matrix_row_t last_row_value = current_matrix[tmp];

            // Check row pin state
            if (row_value & row_mask[row]) {
                // Pin HI, set col bit
                i2c_matrix_debug("i2c matrix scan pin HI, col:%d \n", col);
                current_matrix[tmp] |= (1 << col);
            } else {
                // Pin LOW, clear col bit
                //i2c_matrix_debug("i2c matrix scan pin LOW, col:%d \n", col);
                current_matrix[tmp] &= ~(1 << col);
            }

            // Determine if the matrix changed state
            if ((last_row_value != current_matrix[tmp]) && !(changed)) {
                i2c_matrix_debug("i2c matrix changed: last: 0x%x, current: 0x%x\n", last_row_value, current_matrix[tmp]);
                changed = true;
            }
        }
        // Unselect col
        switch(col) {
        case 0:
            clear_pin(col_pins[col]);
            break;
        case 8:
            tca6424_write_port(TCA6424_PORT1, 0);
            break;
        case 15:
            tca6424_write_port(TCA6424_PORT0, p0_data&0x01);
            break;
        default:
            break;
        }
    }

    return changed;
}

#define P0_SLEEP_STATE  0xFE
#define P1_SLEEP_STATE  0xFF
#define P2_SLEEP_STATE  0x02

void matrix_i2c_prepare_sleep(void)
{
    // put row to input

    // put col to 1
    tca6424_write_port(TCA6424_PORT0, P0_SLEEP_STATE);
    tca6424_write_port(TCA6424_PORT1, P1_SLEEP_STATE);
    tca6424_write_port(TCA6424_PORT2, P2_SLEEP_STATE);
    // clear INT 
    tca6424_read_port(TCA6424_PORT2);
    // set INT detect port event
    nrf_gpio_cfg_sense_input(MATRIX_INT_PIN, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    i2c_matrix_debug("matrix i2c sleep prepared\n");
}

/*
__attribute__((weak))
bool matrix_i2c_check_boot(void)
{
    return false;
}*/


static uint16_t caps_lock_pin = DEF_PIN(TCA6424_PORT2, 3);
static uint16_t scroll_lock_pin = DEF_PIN(TCA6424_PORT0, 1);

void led_set(uint8_t led)
{
    if (led & (1 << USB_LED_CAPS_LOCK)) {
        set_pin(caps_lock_pin);
    } else {
        clear_pin(caps_lock_pin);
    }

    if (led & (1 << USB_LED_SCROLL_LOCK)) {
        set_pin(scroll_lock_pin);
    } else {
        clear_pin(scroll_lock_pin);
    }
}