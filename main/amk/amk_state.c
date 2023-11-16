/**
 * @file amk_state.c
 * @brief matrix state scanning
 * 
 * @copyright Copyright (c) astro
 * 
*/

#include "amk_state.h"
#include "timer.h"

enum key_status {
    KEY_STATUS_OFF,
    KEY_STATUS_DOWN,
    KEY_STATUS_ON,
    KEY_STATUS_UP,
};

struct key_state {
    uint32_t on;
    uint32_t status;
    uint32_t last;
};

static struct key_state state_matrix[MATRIX_ROWS][MATRIX_COLS];

void state_matrix_init(void)
{
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
            key->status = KEY_STATUS_DOWN;
            key->last = timer_read32();
        } else {
            // do nothing
        }
        break;
    case KEY_STATUS_DOWN:
        if (value) {
            if (timer_elapsed32(key->last) > DOWN_DEBOUNCE) {
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
            if (timer_elapsed32(key->last) > UP_DEBOUNCE) {
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
