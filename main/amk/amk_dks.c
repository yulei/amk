/**
 * @file amk_dks.c
 * @brief dynamic keystroke implementation
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#include "amk_dks.h"
#include "amk_store.h"

#include "quantum.h"

#include "amk_printf.h"

#ifndef DKS_DEBUG
#define DKS_DEBUG 1
#endif

#if DKS_DEBUG
#define dks_debug  amk_printf
#else
#define dks_debug(...)
#endif

#define DKS_KEY_COUNT       AMK_DKS_KEY_MAX
#define DKS_EVENT_COUNT     AMK_DKS_EVENT_MAX


enum dks_key_state
{
    DKS_KEY_OFF,
    DKS_KEY_FIRST_PRESSING,
    DKS_KEY_FIRST_RELEASING,
    DKS_KEY_FIRST_ON,
    DKS_KEY_SECOND_PRESSING,
    DKS_KEY_SECOND_RELEASING,
    DKS_KEY_ON,
};

struct dks_key
{
    uint32_t state;
    uint32_t min;
    uint32_t max;
    uint32_t trigger;
    struct amk_dks action;
    bool valid;
};

#ifdef EC_MATRIX_ENABLE

#ifndef DKS_FIRST_INTERVAL
#define DKS_FIRST_INTERVAL  15
#endif

#ifndef DKS_SECOND_INTERVAL
#define DKS_SECOND_INTERVAL 50
#endif

#ifndef DKS_KEY_MIN
#define DKS_KEY_MIN             100
#endif

#ifndef DKS_KEY_MAX
#define DKS_KEY_MAX             4000
#endif

#ifndef DKS_THRESHOLD
#define DKS_THRESHOLD           100
#endif

#ifndef DKS_INTERVAL_INVALID
#define DKS_INTERVAL_INVALID    0
#endif

#ifndef DKS_INTERVAL_MIN
#define DKS_INTERVAL_MIN        200
#endif

#ifndef DKS_INTERVAL_MAX
#define DKS_INTERVAL_MAX        3800
#endif 

#ifndef DKS_INTERVAL_PRESET
#define DKS_INTERVAL_PRESET     2048
#endif

#ifndef DKS_KEY_MIN_PRESET
#define DKS_KEY_MIN_PRESET      500
#endif

#ifndef DKS_KEY_MAX_PRESET
#define DKS_KEY_MAX_PRESET      2800
#endif

#else

#ifndef DKS_FIRST_INTERVAL
#define DKS_FIRST_INTERVAL  15
#endif

#ifndef DKS_SECOND_INTERVAL
#define DKS_SECOND_INTERVAL 50
#endif

#ifndef DKS_KEY_MIN
#define DKS_KEY_MIN             2100
#endif

#ifndef DKS_KEY_MAX
#define DKS_KEY_MAX             4000
#endif

#ifndef DKS_THRESHOLD
#define DKS_THRESHOLD           100
#endif

#ifndef DKS_INTERVAL_INVALID
#define DKS_INTERVAL_INVALID    0
#endif

#ifndef DKS_INTERVAL_MIN
#define DKS_INTERVAL_MIN        100
#endif

#ifndef DKS_INTERVAL_MAX
#define DKS_INTERVAL_MAX        2048
#endif 

#ifndef DKS_INTERVAL_PRESET
#define DKS_INTERVAL_PRESET     1300
#endif

#ifndef DKS_KEY_MIN_PRESET
#define DKS_KEY_MIN_PRESET      2200
#endif

#ifndef DKS_KEY_MAX_PRESET
#define DKS_KEY_MAX_PRESET      3000
#endif

#endif

#define DKS_KEY_STATE_DEFAULT   DKS_KEY_OFF
#define DKS_KEY_MIN_DEFAULT     DKS_KEY_MAX
#define DKS_KEY_MAX_DEFAULT     DKS_KEY_MIN
#define DKS_KEY_TRIGGER_DEFAULT 0

static struct dks_key dks_matrix[MATRIX_ROWS][MATRIX_COLS];
extern void* dynamic_keymap_macro_get_addr(uint8_t id);

static void dks_print_action(struct dks_key *dks)
{
    for( int i = 0; i < DKS_KEY_COUNT; i++) {
        dks_debug("Key: index:%d, value: 0x%x\n", i, dks->action.keys[i]);
    }

    for( int i = 0; i < DKS_EVENT_COUNT; i++) {
        dks_debug("Event: index:%d, value: 0x%x\n", i, dks->action.events[i]);
    }
}

static bool dks_keycode_valid(uint16_t keycode)
{
    return keycode != KC_NO;
}

static bool dks_action_valid(struct amk_dks *dks)
{
    for( int i = 0; i < DKS_KEY_COUNT; i++) {
        if (dks_keycode_valid(dks->keys[i])) return true;
    }

//    for( int i = 0; i < DKS_EVENT_COUNT; i++) {
//        if (dks->events[i] != 0) return true;
//    }

    return false;
}

static bool is_adc_value_valid(uint32_t value)
{
    if (value < DKS_KEY_MIN || value > DKS_KEY_MAX) return false;
    return true;
}

static int dks_update_dir(struct dks_key* key, uint32_t value)
{
    if (value > (key->trigger + DKS_THRESHOLD)) {
        return 1; // down
    } 

    if ((value + DKS_THRESHOLD) < key->trigger) {
        return -1; // up
    }

    return 0;
}

static uint32_t dks_get_interval( struct dks_key * key, uint32_t percent)
{
    uint32_t interval = DKS_INTERVAL_INVALID;
    if ((key->min > DKS_KEY_MIN_PRESET) || (key->max < DKS_KEY_MAX_PRESET) || (key->max < (key->min+DKS_INTERVAL_MIN))) {
        interval = (DKS_INTERVAL_PRESET*percent) / 100;
    } else {
        interval = DKS_INTERVAL_MIN + ((key->max-key->min-DKS_INTERVAL_MIN)*percent) / 100;
    }

    return interval;
}

static void dks_clear_key(struct dks_key *key)
{
    for (int i = 0; i < DKS_KEY_COUNT; i++) {
        if (dks_keycode_valid(key->action.keys[i])) {
            unregister_code(key->action.keys[i]);
        }
    }
}

static void dks_key_action(struct dks_key *key, uint32_t index)
{
    for (int i = 0; i < DKS_KEY_COUNT; i++) {
        if (dks_keycode_valid(key->action.keys[i])){
            bool down = (key->action.events[i] & (1<<index)) > 0;
            bool up = (key->action.events[i] & (1<<(index+4))) > 0;
            if (down&&up) {
                tap_code(key->action.keys[i]);
                dks_debug("DKS: tap code=0x%x\n", key->action.keys[i]);
            } else {
                if (down) {
                    register_code(key->action.keys[i]);
                    dks_debug("DKS: register code=0x%x\n", key->action.keys[i]);
                }
                if (up) {
                    unregister_code(key->action.keys[i]);
                    dks_debug("DKS: unregister code=0x%x\n", key->action.keys[i]);
                }
            }
        }
    }
    /*
    uint8_t event = key->action.events[index];
    for (int j = 0; j < DKS_EVENT_COUNT; j++) {
        if (dks_keycode_valid(key->action.keys[j])) {
            bool down = (event & (1<<j)) > 0;
            bool up = (event & (1<<(j+4))) > 0;
            if (down&&up) {
                tap_code(key->action.keys[j]);
                dks_debug("DKS: tap code=0x%x\n", key->action.keys[j]);
            } else {
                if (down) {
                    register_code(key->action.keys[j]);
                    dks_debug("DKS: register code=0x%x\n", key->action.keys[j]);
                }
                if (up) {
                    unregister_code(key->action.keys[j]);
                    dks_debug("DKS: unregister code=0x%x\n", key->action.keys[j]);
                }
            }
        }
    }
    */
}

void dks_matrix_init(void)
{
    for (uint32_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint32_t col = 0; col < MATRIX_COLS; col++) {
            dks_matrix[row][col].state = DKS_KEY_STATE_DEFAULT;
            dks_matrix[row][col].min = DKS_KEY_MIN_DEFAULT;
            dks_matrix[row][col].max = DKS_KEY_MAX_DEFAULT;
            dks_matrix[row][col].trigger = DKS_KEY_TRIGGER_DEFAULT;
            amk_store_get_dks(row, col, &dks_matrix[row][col].action);
            dks_matrix[row][col].valid = dks_action_valid(&dks_matrix[row][col].action);
        }
    }
}

bool dks_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct dks_key* key = &dks_matrix[row][col];

    if (is_adc_value_valid(value)) {
        if (key->min > value) key->min = value;
        if (key->max < value) key->max = value;
        if (key->trigger == DKS_KEY_TRIGGER_DEFAULT) key->trigger = value;

        int dir = dks_update_dir(key, value);

        switch(key->state) {
        case DKS_KEY_OFF:
            if (dir > 0) {
                key->state = DKS_KEY_FIRST_PRESSING;
                key->trigger = value;
                dks_debug("DKS from KEY_OFF to FIRST_PRESSING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            }
            break;
        case DKS_KEY_FIRST_PRESSING:
            if (dir < 0) {
                key->state = DKS_KEY_FIRST_RELEASING;
                key->trigger = value;
                dks_debug("DKS from FIRST_PRESSING to FIRST_RELEASING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            } else {
                uint32_t interval = dks_get_interval(key, DKS_FIRST_INTERVAL);
                if (value > key->min + interval) {
                    key->state = DKS_KEY_FIRST_ON;
                    key->trigger = value;
                    // trigger action
                    dks_debug("DKS from FIRST_PRESSING to FIRST_ON, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                    dks_key_action(key, 0);
                }
            }
            break;
        case DKS_KEY_FIRST_RELEASING:
            if (dir > 0) {
                key->state = DKS_KEY_FIRST_PRESSING;
                key->trigger = value;
                dks_debug("DKS from FIRST_RELEASING to FIRST_PRESSING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            } else {
                if (value < (key->min+DKS_THRESHOLD)) {
                    key->state = DKS_KEY_OFF;
                    key->trigger = value;
                    // do cleanup
                    dks_clear_key(key);
                    dks_debug("DKS from FIRST_RELEASING to KEY_OFF, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
                }
            }
            break;
        case DKS_KEY_FIRST_ON:
            if (dir > 0) {
                key->state = DKS_KEY_SECOND_PRESSING;
                key->trigger = value;
                dks_debug("DKS from FIRST_ON to SECOND_PRESSING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            } else {
                if (dir < 0) {
                    key->state = DKS_KEY_FIRST_RELEASING;
                    key->trigger = value;
                    dks_debug("DKS from FIRST_ON to FIRST_RELEASING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
                }
            }
            break;
        case DKS_KEY_SECOND_PRESSING:
            if (dir < 0) {
                key->state = DKS_KEY_SECOND_RELEASING;
                key->trigger = value;
                dks_debug("DKS from SECOND_PRESSING to SECOND_RELEASING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            } else {
                uint32_t interval = dks_get_interval(key, DKS_SECOND_INTERVAL);
                if (value > key->min + interval) {
                    key->state = DKS_KEY_ON;
                    key->trigger = value;
                    // trigger action
                    dks_debug("DKS from SECOND_PRESSING to SECOND_ON, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                    dks_key_action(key, 1);
                }
            }
            break;
        case DKS_KEY_SECOND_RELEASING:
            if (dir > 0) {
                key->state = DKS_KEY_SECOND_PRESSING;
                key->trigger = value;
                dks_debug("DKS from SECOND_RELEASING to SECOND_PRESSING, value=%ld, min=%ld, max=%ld, trigger=%ld\n", value, key->min, key->max, key->trigger);
            } else {
                if (dir < 0) {
                    uint32_t interval = dks_get_interval(key, DKS_FIRST_INTERVAL);
                    if (value < key->min + interval) {
                        key->state = DKS_KEY_FIRST_ON;
                        key->trigger = value;
                        // trigger action
                        dks_debug("DKS from SECOND_RELEASING to FIST_ON, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                        dks_key_action(key, 3);
                    }
                }
            }
            break;
        case DKS_KEY_ON:
            if ( dir < 0) {
                // only process while key up to second interval
                uint32_t interval = dks_get_interval(key, DKS_SECOND_INTERVAL);
                if (value < key->min + interval) {
                    key->state = DKS_KEY_SECOND_RELEASING;
                    key->trigger = value;
                    // trigger action
                    dks_debug("DKS from DKS_ON to DKS_SECOND_RELEASING, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                    dks_key_action(key, 2);
                }
            }
            break;
        default:
            // never here
            break;
        }
    }

    return false;
}

bool dks_matrix_valid(uint32_t row, uint32_t col)
{
    return dks_matrix[row][col].valid;
}

void dks_matrix_update_action(void)
{
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            dks_matrix_update_action_at(row, col);
        }
    }
}

void dks_matrix_update_action_at(uint32_t row, uint32_t col)
{
    amk_store_get_dks(row, col, &dks_matrix[row][col].action);
    dks_matrix[row][col].valid = dks_action_valid(&dks_matrix[row][col].action);
    dks_print_action(&dks_matrix[row][col]);
}