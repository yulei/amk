/**
 * @file amk_state.c
 * @brief matrix state scanning
 * 
 * @copyright Copyright (c) astro
 * 
*/

#include "quantum.h"
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
};

extern uint8_t debounce_press;
extern uint8_t debounce_release;

static struct key_state state_matrix[MATRIX_ROWS][MATRIX_COLS];

void state_matrix_init(void)
{
    uint8_t debounce = eeconfig_read_debounce();
    debounce_press = (debounce & 0xF0) >> 4;
    debounce_release = debounce & 0x0F;
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            state_matrix[row][col].on = 0;
            state_matrix[row][col].status = KEY_STATUS_OFF;
            state_matrix[row][col].last = timer_read32();
        }
    }
}

bool state_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct key_state * key = &state_matrix[row][col];
    switch (key->status) {
    case KEY_STATUS_OFF:
        if (value) {
            if (debounce_press == 0) {
                key->status = KEY_STATUS_ON;
                key->on = 1;
            } else {
                key->status = KEY_STATUS_DOWN;
                key->last = timer_read32();
            }
        }
        break;
    case KEY_STATUS_DOWN:
        if (value) {
            if (timer_elapsed32(key->last) > debounce_press) {
                key->status = KEY_STATUS_ON;
                key->on = 1;
            }
        } else {
            key->status = KEY_STATUS_OFF;
        }
        break;
    case KEY_STATUS_ON:
        if (!value) {
            if (debounce_release == 0) {
                key->status = KEY_STATUS_OFF;
                key->on = 0;
            } else {
                key->status = KEY_STATUS_UP;
                key->last = timer_read32();
            }
        }
        break;
    case KEY_STATUS_UP:
        if (!value) {
            if (timer_elapsed32(key->last) > debounce_release) {
                key->status = KEY_STATUS_OFF;
                key->on = 0;
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
    if (press) return debounce_press;

    return debounce_release;
}

void state_matrix_update_debounce(uint8_t down, uint8_t up)
{
    debounce_press= down;
    debounce_release = up;
}
