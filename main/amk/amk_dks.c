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
#include "keymap_introspection.h"

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
#define DKS_FIRST_INTERVAL  30
#define DKS_SECOND_INTERVAL 70


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

static struct dks_key dks_matrix[MATRIX_ROWS][MATRIX_COLS];
extern void* dynamic_keymap_macro_get_addr(uint8_t id);

static void dks_print_action(struct dks_key *dks)
{
    for( int i = 0; i < dks->action.count; i++) {
        dks_debug("Code: index:%d, value: 0x%x\n", i, dks->action.codes[i]);
    }

    if (dks->action.count) {
        for( int i = 0; i < DKS_EVENT_COUNT; i++) {
            dks_debug("Event: index:%d, value: 0x%x\n", i, dks->action.events[i]);
        }
    }
}

static bool dks_parse_macro(uint16_t keycode, struct dks_key *dks)
{
    uint8_t* p = dynamic_keymap_macro_get_addr(keycode);

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
                        if (dks->action.count < 4) {
                            dks->action.events[index-1] |= 1 << dks->action.count; // down event
                            dks->action.events[index-1] |= 1 << (dks->action.count+4); // up event
                            dks->action.codes[dks->action.count] = data[2];
                            dks->action.count++;
                        }
                    } else if (data[1] == SS_DOWN_CODE) {
                        if (dks->action.count < 4) {
                            dks->action.events[index-1] |= 1 << dks->action.count; // down event
                            dks->action.codes[dks->action.count] = data[2];        // record keycode while key down
                            dks->action.count++;
                        }
                    } else if (data[1] == SS_UP_CODE) {
                        for (int i = 0; i < dks->action.count; i++) {
                            if (data[2] == dks->action.codes[i]) {
                                dks->action.events[index-1] |= 1 << (i+4); // up event
                                break;
                            }
                        }
                    }
                    //index = 0;
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

static void dks_matrix_update_key(uint32_t row, uint32_t col)
{
    uint16_t keycode = keycode_at_keymap_location(APC_KEYMAP_DKS_LAYER, row, col);
    if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
        uint8_t id = keycode - QK_MACRO;
        if (dks_parse_macro(id, &dks_matrix[row][col])) {
            dks_print_action(&dks_matrix[row][col]);
        }
    }
}

void dks_matrix_init(void)
{
    for (uint32_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint32_t col = 0; col < MATRIX_COLS; col++) {
            dks_matrix[row][col].state = DKS_KEY_STATE_DEFAULT;
            dks_matrix[row][col].min = DKS_KEY_MIN_DEFAULT;
            dks_matrix[row][col].max = DKS_KEY_MAX_DEFAULT;
            dks_matrix[row][col].last = DKS_KEY_LAST_DEFAULT;
            dks_matrix[row][col].trigger = DKS_KEY_TRIGGER_DEFAULT;
            memset(&dks_matrix[row][col].action, 0, sizeof(struct dks_action));
            dks_matrix_update_key(row, col);
        }
    }
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
        interval = //DKS_INTERVAL_MIN + ((DKS_INTERVAL_MAX-DKS_INTERVAL_MIN)*percent) / 100;
                (1600*percent) / 100;
    } else {
        interval = DKS_INTERVAL_MIN + ((key->max-key->min)*percent) / 100;
    }

    return interval;
}

void dks_clear_key(struct dks_key *key)
{
    for (int i = 0; i < key->action.count; i++) {
        unregister_code(key->action.codes[i]);
    }
}

void dks_key_action(struct dks_key *key, uint32_t index)
{
    uint8_t event = key->action.events[index];
    for (int j = 0; j < key->action.count; j++) {
        // 
        bool down = (event & (1<<j)) > 0;
        bool up = (event & (1<<(j+4))) > 0;
        if (down&&up) {
            tap_code(key->action.codes[j]);
        } else {
            if (down) {
                register_code(key->action.codes[j]);
            }
            if (up) {
                unregister_code(key->action.codes[j]);
            }
        }
    }
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
                    dks_debug("DKS from FIRST_PRESSING to FIRST_ON, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                    dks_key_action(key, 0);
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
                    dks_clear_key(key);
                }
            }
            break;
        case DKS_KEY_FIRST_ON:
            if (dir > 0) {
                key->state = DKS_KEY_SECOND_PRESSING;
                key->trigger = value;
                uint32_t interval = dks_get_interval(key, DKS_SECOND_INTERVAL);
                dks_debug("DKS from FIRST_ON to SECOND_PRESSING, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);

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
                    dks_debug("DKS from SECOND_PRESSING to SECOND_ON, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                    dks_key_action(key, 1);
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
                        dks_debug("DKS from SECOND_RELEASING to FIST_ON, value=%ld, min=%ld, max=%ld, trigger=%ld, interval=%ld\n", value, key->min, key->max, key->trigger, interval);
                        dks_key_action(key, 3);
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

void dks_matrix_update_action(void)
{
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            memset(&dks_matrix[row][col].action, 0, sizeof(struct dks_action));
            dks_matrix_update_key(row, col);
        }
    }
}