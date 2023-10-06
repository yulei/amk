/**
 * @file amk_dks.c
 * @brief dynamic keystroke implementation
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#include "amk_dks.h"
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

#define DKS_KEY_COUNT       4
#define DKS_EVENT_COUNT     4
#define DKS_FIRST_INTERVAL  40
#define DKS_SECOND_INTERVAL 80


struct dks_action
{
    uint8_t events[DKS_EVENT_COUNT];
    uint8_t codes[DKS_KEY_COUNT];
    uint32_t count;
};

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
    uint32_t last;
    uint32_t trigger;
    struct dks_action action;
};

#define DKS_KEY_MIN             2048
#define DKS_KEY_MAX             4096
#define DKS_THRESHOLD           100
#define DKS_INTERVAL_INVALID    0
#define DKS_INTERVAL_MIN        50
#define DKS_INTERVAL_MAX        2048

#define DKS_KEY_STATE_DEFAULT   DKS_KEY_OFF
#define DKS_KEY_MIN_DEFAULT     DKS_KEY_MAX
#define DKS_KEY_MAX_DEFAULT     DKS_KEY_MIN
#define DKS_KEY_LAST_DEFAULT    0
#define DKS_KEY_TRIGGER_DEFAULT 0
#define MACRO00                 0xFF01

static struct dks_key dks_matrix[MATRIX_ROWS][MATRIX_COLS];
extern void* dynamic_keymap_macro_get_addr(uint8_t id);

static bool dks_parse_macro(uint16_t keycode, struct dks_key *dks)
{
    uint8_t* p = dynamic_keymap_macro_get_addr(keycode-MACRO00);

    if (!p) return false;

    char data[4] = {0, 0, 0, 0};
    int index = 0;
    for (;;) {
        data[0] = eeprom_read_byte(p++);
        data[1] = 0;
        // Stop at the null terminator of this macro string
        if (data[0] == 0) {
            break;
        }
        if (data[0] == SS_QMK_PREFIX) {
            // If the char is magic, process it as indicated by the next character
            // (tap, down, up, delay)
            data[1] = eeprom_read_byte(p++);
            if (data[1] == 0)
                break;

            if (data[1] == SS_TAP_CODE || data[1] == SS_DOWN_CODE || data[1] == SS_UP_CODE) {
                // For tap, down, up, store the keycode
                data[2] = eeprom_read_byte(p++);

                if (data[2] != 0) {
                    if (!index) {
                        // invalid index
                        break;
                    }

                    if (data[1] == SS_TAP_CODE) {
                        dks->action.events[index-1] |= 1 << dks->action.count; // down event
                        dks->action.events[index-1] |= 1 << (dks->action.count+4); // up event
                        dks->action.codes[dks->action.count] = data[2];
                        dks->action.count++;
                    } else if (data[1] == SS_DOWN_CODE) {
                        dks->action.events[index-1] |= 1 << dks->action.count; // down event
                    } else if (data[1] == SS_UP_CODE) {
                        dks->action.events[index-1] |= 1 << (dks->action.count+4); // up event
                        dks->action.codes[dks->action.count] = data[2];           // record keycode while key up
                        dks->action.count++;
                    }
                    index = 0;
                }
            } else if (data[1] == SS_DELAY_CODE) {
                // Delay was used for DKS index
                uint8_t d0 = eeprom_read_byte(p++);
                uint8_t d1 = eeprom_read_byte(p++);
                int ms = (d0 - 1) + (d1 - 1) * 255;
                if ((ms < 1) || (ms > 4)) {
                    break;
                }
                index = ms;
            }
        } else {
            break;
        }
    }

    return dks->action.count > 0;
}

void dks_matrix_init(void)
{
    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            memset(&dks_matrix[i][j], 0, sizeof(struct dks_key));
        }
    }
}

bool dks_matrix_parse_key(uint32_t row, uint32_t col, uint16_t key)
{
    struct dks_key* dks = &dks_matrix[row][col];
    if (dks_parse_macro(key, dks)) {
        return true;
    }

    return false; 
}

bool dks_matrix_valid(uint32_t row, uint32_t col)
{
    if (dks_matrix[row][col].action.count > 0) {
        return true;
    }

    return false;
}


static bool is_adc_value_valid(uint32_t value)
{
    if (value < DKS_KEY_MIN|| value > DKS_KEY_MAX) return false;
    return true;
}

static int dks_update_dir(struct dks_key* key, uint32_t value)
{
    if (value > (key->last + DKS_THRESHOLD)) {
        key->last = value;
        return 1; // down
    } 

    if ((value + DKS_THRESHOLD) < key->last) {
        key->last = value;
        return -1; // up
    }

    return 0;
}

static uint32_t dks_get_interval( struct dks_key * key, uint32_t percent)
{
    uint32_t interval = DKS_INTERVAL_INVALID;

    if ((key->min == DKS_KEY_MIN_DEFAULT) || (key->max < DKS_KEY_MAX)) {
        interval = DKS_INTERVAL_MIN + ((DKS_INTERVAL_MAX-DKS_INTERVAL_MIN)*percent) / 100;
    } else {
        interval = DKS_INTERVAL_MIN + ((key->max-key->min)*percent) / 100;
    }

    return interval;
}

bool dks_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct dks_key* key = &dks_matrix[row][col];

    if (is_adc_value_valid(value)) {
        if (key->min > value) key->min = value;
        if (key->max < value) key->max = value;
        if (key->last == DKS_KEY_LAST_DEFAULT) key->last = value;

        int dir = dks_update_dir(key, value);

        switch(key->state) {
        case DKS_KEY_OFF:
            if (dir > 0) {
                key->state = DKS_KEY_FIRST_PRESSING;
                key->trigger = value;
            }
            break;
        case DKS_KEY_FIRST_PRESSING:
            if (dir < 0) {
                key->state = DKS_KEY_FIRST_RELEASING;
                key->trigger = value;
            } else {
                uint32_t interval = dks_get_interval(key, DKS_FIRST_INTERVAL);
                if (value > key->min + interval) {
                    key->trigger = value;
                    key->state = DKS_KEY_FIRST_ON;
                    // trigger action
                    dks_debug("DKS from FIRST_PRESSING to FIRST_ON, value=%d, min=%d, max=%d, trigger=%d, interval=%d\n", value, key->min, key->max, key->trigger, interval);
                }
            }
            break;
        case DKS_KEY_FIRST_RELEASING:
            if (dir > 0) {
                key->state = DKS_KEY_FIRST_PRESSING;
                key->trigger = value;
            } else {
                if (value < (key->min+DKS_THRESHOLD)) {
                    key->state = DKS_KEY_OFF;
                    key->trigger = value;
                    // do cleanup
                }
            }
            break;
        case DKS_KEY_FIRST_ON:
            if (dir > 0) {
                key->state = DKS_KEY_SECOND_PRESSING;
                key->trigger = value;
            } else {
                if (dir < 0) {
                    key->state = DKS_KEY_FIRST_RELEASING;
                    key->trigger = value;
                }
            }
            break;
        case DKS_KEY_SECOND_PRESSING:
            if (dir < 0) {
                key->state = DKS_KEY_SECOND_RELEASING;
                key->trigger = value;
            } else {
                uint32_t interval = dks_get_interval(key, DKS_SECOND_INTERVAL);
                if (value > key->min + interval) {
                    key->trigger = value;
                    key->state = DKS_KEY_ON;
                    // trigger action
                    dks_debug("DKS from SECOND_PRESSING to SECOND_ON, value=%d, min=%d, max=%d, trigger=%d, interval=%d\n", value, key->min, key->max, key->trigger, interval);
                }
            }
            break;
        case DKS_KEY_SECOND_RELEASING:
            if (dir > 0) {
                key->state = DKS_KEY_SECOND_PRESSING;
                key->trigger = value;
            } else {
                if (dir < 0) {
                    uint32_t interval = dks_get_interval(key, DKS_FIRST_INTERVAL);
                    if (value < key->min + interval) {
                        key->trigger = value;
                        key->state = DKS_KEY_FIRST_ON;
                        // trigger action
                        dks_debug("DKS from SECOND_RELEASING to FIST_ON, value=%d, min=%d, max=%d, trigger=%d, interval=%d\n", value, key->min, key->max, key->trigger, interval);
                    }
                }
            }
            break;
        case DKS_KEY_ON:
            if (dir < 0) {
                // key up
                uint32_t interval = dks_get_interval(key, DKS_SECOND_INTERVAL);
                if (value < key->min + interval) {
                    key->state = DKS_KEY_SECOND_RELEASING;
                    key->trigger = value;
                    // trigger action
                    dks_debug("DKS from DKS_ON to DKS_SECOND_RELEASING, value=%d, min=%d, max=%d, trigger=%d, interval=%d\n", value, key->min, key->max, key->trigger, interval);
                }
            }
            break;
        default:
            // never here
            break;
        }
    }

    return true;
}
