/**
 * @file amk_apc.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include "amk_apc.h"
#include "matrix.h"
#include "wait.h"
#include "led.h"
#include "timer.h"

#include "amk_gpio.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifndef CUSTOM_MATRIX_DEBUG
#define CUSTOM_MATRIX_DEBUG 1
#endif

#if CUSTOM_MATRIX_DEBUG
#define custom_matrix_debug  amk_printf
#else
#define custom_matrix_debug(...)
#endif

#define APC_KEY_STATE_DEFAULT   APC_KEY_OFF
#define APC_KEY_MIN_DEFAULT     APC_KEY_MAX
#define APC_KEY_MAX_DEFAULT     APC_KEY_MIN
#define APC_KEY_LAST_DEFAULT    0
#define APC_KEY_TRIGGER_DEFAULT 0
#define APC_KEY_DOWN_DEFAULT    1024
#define APC_KEY_UP_DEFAULT      0

enum apc_key_state
{
    APC_KEY_OFF,
    APC_KEY_PRESSING,
    APC_KEY_ON,
    APC_KEY_RELEASING,
};

struct apc_key
{
    uint32_t state;
    uint32_t min;
    uint32_t max;
    uint32_t last;
    uint32_t trigger;
    uint32_t down;
    uint32_t up;
    bool on;
};

static struct apc_key apc_matrix[MATRIX_ROWS][MATRIX_COLS];

#ifdef DYNAMIC_KEYMAP_ENABLE
#include "dynamic_keymap.h"
#else
extern const uint16_t keymaps[][][];
#endif

static uint32_t apc_get_key_interval(uint32_t row, uint32_t col, uint32_t layer)
{
#ifdef DYNAMIC_KEYMAP_ENABLE
    uint16_t keycode = dynamic_keymap_get_keycode(layer, row, col);
#else
    uint16_t keycode = keymaps[layer][row][col];
#endif
    uint16_t index = 0;
    if (keycode>=KC_F1 && keycode <= KC_F12) {
        index = keycode - KC_F1 + 1;
    } else if (keycode>=KC_F13 && keycode <= KC_F24) {
        index = keycode - KC_F13 + (KC_F12 - KC_F1) + 1;
    } else {
        if (layer == APC_KEYMAP_UP_LAYER) {
            return APC_INTERVAL_INVALID;
        }
        index = APC_INTERVAL_INDEX;
    }

    uint32_t interval = APC_INTERVAL_MIN + ((APC_INTERVAL_MAX-APC_INTERVAL_MIN)/APC_INTERVAL_COUNT)*(index);

    return interval;
}

static void apc_update_key_interval(uint32_t row, uint32_t col)
{
    apc_matrix[row][col].down = apc_get_key_interval(row, col, APC_KEYMAP_DOWN_LAYER);
    apc_matrix[row][col].up = apc_get_key_interval(row, col, APC_KEYMAP_UP_LAYER);
}

void apc_matrix_init(void)
{
    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            apc_matrix[i][j].state = APC_KEY_STATE_DEFAULT;
            apc_matrix[i][j].min = APC_KEY_MIN_DEFAULT;
            apc_matrix[i][j].max = APC_KEY_MAX_DEFAULT;
            apc_matrix[i][j].last = APC_KEY_LAST_DEFAULT;
            apc_matrix[i][j].trigger = APC_KEY_TRIGGER_DEFAULT;
            apc_matrix[i][j].down = APC_KEY_DOWN_DEFAULT;
            apc_matrix[i][j].up = APC_KEY_UP_DEFAULT;
            apc_matrix[i][j].on = false;
        }
    }
}

static bool is_adc_value_valid(uint32_t value)
{
    if (value < APC_KEY_MIN|| value > APC_KEY_MAX) return false;
    return true;
}

static int apc_update_dir(struct apc_key* key, uint32_t value)
{
    if (value > (key->last + APC_THRESHOLD)) {
        key->last = value;
        return 1; // down
    } 

    if ((value + APC_THRESHOLD) < key->last) {
        key->last = value;
        return -1; // up
    }

    return 0;
}

bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct apc_key* key = &apc_matrix[row][col];
    if (is_adc_value_valid(value)) {
        if (key->min > value) key->min = value;
        if (key->max < value) key->max = value;
        if (key->last == APC_KEY_LAST_DEFAULT) key->last = value;
        apc_update_key_interval(row, col);
        int dir = apc_update_dir(key, value);

        switch(key->state) {
        case APC_KEY_OFF:
            if (dir > 0) {
                // key down
                key->state = APC_KEY_PRESSING;
                key->trigger = value;
            }
            break;
        case APC_KEY_ON:
            if (dir < 0) {
                // key up
                key->state = APC_KEY_RELEASING;
                key->trigger = value;
            }
            break;
        case APC_KEY_PRESSING: 
            {
                if (dir < 0) {
                    // switch to releasing
                    key->state = APC_KEY_RELEASING;
                    key->trigger = value;
                } else {
                    if (!key->on) {
                        if (value > key->min + key->down) {
                            key->trigger = value;
                            key->state = APC_KEY_ON;
                            key->on = true;
                            custom_matrix_debug("APC from PRESSING to ON, value=%d, min=%d, max=%d, trigger=%d, down=%d\n", value, key->min, key->max, key->trigger, key->down);
                        }
                    }
                }
            }
            break;
        case APC_KEY_RELEASING:
            {
                if (dir > 0) {
                    // switch to pressing
                    key->state = APC_KEY_PRESSING;
                    key->trigger = value;
                } else {
                    if (key->on) {
                        if (key->up != APC_INTERVAL_INVALID) {
                            if (((value + key->up) < key->trigger)) {
                                // rapid trigger
                                key->state = APC_KEY_OFF;
                                key->on = false;
                                key->trigger = value;
                                custom_matrix_debug("APC RT from RELEASING to OFF, value=%d, min=%d, max=%d, trigger=%d, up=%d\n", value, key->min, key->max, key->trigger, key->up);
                            }
                        } else {
                            if ((value + APC_THRESHOLD) < key->min + key->down) {
                                key->state = APC_KEY_OFF;
                                key->on = false;
                                key->trigger = value;
                                custom_matrix_debug("APC Normal from RELEASING to OFF, value=%d, min=%d, max=%d, trigger=%d\n", value, key->min, key->max, key->trigger);
                            }
                        }
                    } else {
                        if (value < (key->min + APC_THRESHOLD)) {
                            key->state = APC_KEY_OFF;
                            key->trigger = value;
                        }
                    }
                }
            }
            break;
        default:
            // never here
            break;
        }
    }

    return key->on;
}
