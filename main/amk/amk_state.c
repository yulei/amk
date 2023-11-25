/**
 * @file amk_state.c
 * @brief matrix state scanning
 * 
 * @copyright Copyright (c) astro
 * 
*/

#include "quantum.h"
#include "keymap_introspection.h"
#include "amk_state.h"
#include "timer.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

enum key_status {
    KEY_STATUS_OFF,
    KEY_STATUS_DOWN,
    KEY_STATUS_ON,
    KEY_STATUS_UP,
};

struct key_state {
    uint32_t status;
    uint32_t last;
    uint16_t on;
    uint16_t alt;
};

uint8_t debounce_press;
uint8_t debounce_release;

static struct key_state state_matrix[MATRIX_ROWS][MATRIX_COLS];
static uint8_t press_debounce;
static uint8_t release_debounce;

static uint16_t get_keycode(uint32_t row, uint32_t col, uint32_t layer)
{
    uint16_t keycode = keycode_at_keymap_location(layer, row, col);

    uint16_t alt = KC_NO;
    if ((keycode != KC_NO) && (keycode != KC_TRNS)) {
        alt = keycode;
    }

    return alt;
}

static void update_state_matrix(void)
{
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            state_matrix[row][col].alt = get_keycode(row, col, STATE_ALTERNATE_KEYCODE_LAYER); 
            amk_printf("State Matrix Update: row=%d, col=%d, keycode=%d\n", row, col, state_matrix[row][col].alt);
        }
    }

}

void state_matrix_init(void)
{
    uint8_t debounce = eeconfig_read_debounce();
    press_debounce = (debounce & 0xF0) >> 4;
    release_debounce = debounce & 0x0F;
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            state_matrix[row][col].on = 0;
            state_matrix[row][col].status = KEY_STATUS_OFF;
            state_matrix[row][col].last = timer_read32();
            state_matrix[row][col].alt = get_keycode(row, col, STATE_ALTERNATE_KEYCODE_LAYER); 
            amk_printf("DEBOUNCE: row=%d, col=%d, keycode=%d\n", row, col, state_matrix[row][col].alt);
        }
    }
}

bool state_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct key_state * key = &state_matrix[row][col];
    switch (key->status) {
    case KEY_STATUS_OFF:
        if (value) {
            key->status = KEY_STATUS_DOWN;
            key->last = timer_read32();
        } else {
            // do nothing
        }
        break;
    case KEY_STATUS_DOWN:
        if (value) {
            if (timer_elapsed32(key->last) > press_debounce) {
                key->status = KEY_STATUS_ON;
                key->on = 1 << col;
            }
        } else {
            key->status = KEY_STATUS_OFF;
        }
        break;
    case KEY_STATUS_ON:
        if (!value) {
            key->status = KEY_STATUS_UP;
            key->last = timer_read32();
        } else {
            // do nothing
        }
        break;
    case KEY_STATUS_UP:
        if (!value) {
            if (timer_elapsed32(key->last) > release_debounce) {
                key->status = KEY_STATUS_OFF;
                key->on = 0;
                if (key->alt != KC_NO) {
                    tap_code(key->alt);
                }
            }
        } else {
            key->status = KEY_STATUS_ON;
        }
        break;
    default:
        // can't be here
        break;
    }

    return key->on != 0;
}

uint8_t state_matrix_get_debounce(bool press)
{
    if (press) return press_debounce;

    return release_debounce;
}

void state_matrix_update_debounce(uint8_t down, uint8_t up)
{
    press_debounce = down;
    release_debounce = up;
}

void raw_hid_send_kb(uint8_t *data, uint8_t length)
{
    if (data[0] == id_dynamic_keymap_set_keycode) {
        // update press/release/lt
        if ( data[1] == STATE_ALTERNATE_KEYCODE_LAYER) {
            update_state_matrix();
        }
    }
}
