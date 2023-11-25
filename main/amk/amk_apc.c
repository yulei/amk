/**
 * @file amk_apc.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include "amk_apc.h"
#include "amk_dks.h"

#include "quantum.h"
#include "keymap_introspection.h"

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

#define MIN_ADJUST_VALUE        100
#define MAX_ADJUST_VALUE        100

#ifndef KEY_ADJUST_MIN
#define KEY_ADJUST_MIN          2150
#endif

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
    uint32_t apc;
    uint32_t rt;
    bool on;
};


static struct apc_key apc_matrix[MATRIX_ROWS][MATRIX_COLS];

//
// TODO:
// actually the relationship of the interval and the magnetic value
// was non-linear. Currently implementation was not accurate.
//
// first part use 1/3, last part use 2/3

#define FIST_PART   1
#define LAST_PART   2
#define TOTAL_PART  (FIST_PART + LAST_PART)

static uint32_t apc_compute_interval(uint32_t row, uint32_t col, uint32_t index)
{
    if (index == 0) return APC_INTERVAL_INVALID;

    struct apc_key *key = &apc_matrix[row][col];
    uint32_t interval = APC_INTERVAL_INVALID;

    if ((key->min > APC_KEY_MIN_PRESET) || (key->max < APC_KEY_MAX_PRESET) || (key->max < (key->min+APC_INTERVAL_MIN))) {
        interval = APC_INTERVAL_MIN + ((APC_INTERVAL_MAX - APC_INTERVAL_MIN)/(APC_INTERVAL_COUNT*TOTAL_PART))*(index);
    } else {
        interval = APC_INTERVAL_MIN + ((key->max - key->min - APC_INTERVAL_MIN)/(APC_INTERVAL_COUNT*TOTAL_PART))*(index);
    }

    return interval;
}

static uint32_t apc_get_key_interval(uint32_t row, uint32_t col, uint32_t layer)
{
    uint16_t keycode = keycode_at_keymap_location(layer, row, col);

    uint16_t index = 0;
    switch(keycode) {
    case KC_F1 ... KC_F12:
        index = keycode - KC_F1 + 1;
        break;
    case KC_F13 ... KC_F24:
        index = (keycode - KC_F13 + 1 + (KC_F12 - KC_F1) + 1)*2;
        break;
    default:
        if (layer == APC_KEYMAP_UP_LAYER) {
            index = APC_INTERVAL_INVALID;
        } else {
            index = APC_INTERVAL_INDEX;
        }
        break;
    }

    return index;
}

static void apc_adjust_min(struct apc_key* key, uint32_t value)
{
    if ((value > KEY_ADJUST_MIN) || (key->min > KEY_ADJUST_MIN)) return;

    key->min = (key->min * 80 + value * 20) / 100;
}

void apc_matrix_init(void)
{
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            apc_matrix[row][col].state = APC_KEY_STATE_DEFAULT;
            apc_matrix[row][col].min = APC_KEY_MIN_DEFAULT;
            apc_matrix[row][col].max = APC_KEY_MAX_DEFAULT;
            apc_matrix[row][col].last = APC_KEY_LAST_DEFAULT;
            apc_matrix[row][col].trigger = APC_KEY_TRIGGER_DEFAULT;
            apc_matrix[row][col].apc = apc_get_key_interval(row, col, APC_KEYMAP_DOWN_LAYER);
            apc_matrix[row][col].rt = apc_get_key_interval(row, col, APC_KEYMAP_UP_LAYER);
            apc_matrix[row][col].on = false;
        }
    }

#ifdef APC_KEYMAP_DKS_LAYER
    dks_matrix_init();
#endif
}

static bool is_adc_value_valid(uint32_t value)
{
    if (value < APC_KEY_MIN || value > APC_KEY_MAX) return false;

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

static bool update_if_off(struct apc_key* key, uint32_t value)
{
    if (key->on && (value < (key->min + APC_THRESHOLD))) {
        key->state = APC_KEY_OFF;
        key->on = false;
        key->trigger = value;
        key->last = value;
        return true;
    }

    return false;
}

bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
#ifdef APC_KEYMAP_DKS_LAYER
    if (dks_matrix_valid(row, col)) {
        return dks_matrix_update(row, col, value);
    }
#endif

    struct apc_key* key = &apc_matrix[row][col];

    if (is_adc_value_valid(value)) {
        if (key->min > value) key->min = value;
        if (key->max < value) key->max = value;
        if (key->last == APC_KEY_LAST_DEFAULT) key->last = value;
        apc_adjust_min(key, value);

        uint32_t down = apc_compute_interval(row, col, key->apc);
        uint32_t up = apc_compute_interval(row, col, key->rt);
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

            if (update_if_off(key, value)) {
                custom_matrix_debug("APC from ON to FORCE OFF, value=%ld, min=%ld, max=%ld, trigger=%ld, up=%ld\n", value, key->min, key->max, key->trigger, up);
            }
            break;
        case APC_KEY_PRESSING: 
            if (dir < 0) {
                // switch to releasing
                key->state = APC_KEY_RELEASING;
                key->trigger = value;
            } else {
                if (!key->on) {
                    if (value > key->min + down) {
                        key->trigger = value;
                        key->state = APC_KEY_ON;
                        key->on = true;
                        custom_matrix_debug("APC from PRESSING to ON, value=%ld, min=%ld, max=%ld, trigger=%ld, down=%ld\n", value, key->min, key->max, key->trigger, down);
                    }
                }
            }
            if (update_if_off(key, value)) {
                custom_matrix_debug("APC from PRESSING to FORCE OFF, value=%ld, min=%ld, max=%ld, trigger=%ld, down=%ld\n", value, key->min, key->max, key->trigger, down);
            }
            break;
        case APC_KEY_RELEASING:
            if (dir > 0) {
                // switch to pressing
                key->state = APC_KEY_PRESSING;
                key->trigger = value;
            } else {
                if (key->on) {
                    if (up != APC_INTERVAL_INVALID) {
                        if ((value + up) < key->trigger)  {
                            // rapid trigger
                            key->state = APC_KEY_OFF;
                            key->on = false;
                            key->trigger = value;
                            custom_matrix_debug("APC RT from RELEASING to OFF, value=%ld, min=%ld, max=%ld, trigger=%ld, up=%ld\n", value, key->min, key->max, key->trigger, up);
                        }
                    } else {
                        if ((value + APC_THRESHOLD) < (key->min + down) ) {
                            key->state = APC_KEY_OFF;
                            key->on = false;
                            key->trigger = value;
                            custom_matrix_debug("APC Normal from RELEASING to OFF, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
                        }
                    }
                }
            }
            if (update_if_off(key, value)) {
                custom_matrix_debug("APC Abnormal from RELEASING to FORCE OFF, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            }
            break;
        default:
            // never here
            break;
        }
    }

    return key->on;
}

void apc_matrix_update_interval(void)
{
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            apc_matrix[row][col].apc = apc_get_key_interval(row, col, APC_KEYMAP_DOWN_LAYER);
            apc_matrix[row][col].rt = apc_get_key_interval(row, col, APC_KEYMAP_UP_LAYER);
        }
    }
}

void raw_hid_send_kb(uint8_t *data, uint8_t length)
{
    if (data[0] == id_dynamic_keymap_set_keycode) {
        // update apc/rt/dks
        #ifdef APC_KEYMAP_DKS_LAYER
        if (data[1] == APC_KEYMAP_DKS_LAYER) {
            dks_matrix_update_action();
        }
        else
        #endif
        {
            if ((data[1] == APC_KEYMAP_DOWN_LAYER) || (data[1] == APC_KEYMAP_UP_LAYER)) {
                apc_matrix_update_interval();
            }
        }
    }
}
