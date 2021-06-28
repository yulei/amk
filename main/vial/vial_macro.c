/**
 * @file vial_macro.c
 */

#include <stddef.h>
#include "vial_macro.h"
#include "send_string_keycodes.h"
#include "send_string.h"
#include "action_layer.h"
#include "debug.h"
#include "amk_keymap.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

static void layer_state_set(uint32_t state)
{
    dprint("layer_state: ");
    layer_debug();
    dprint(" to ");
    layer_state = state;
    layer_debug();
    dprintln();

    clear_keyboard_but_mods();
}

static uint32_t update_tri_layer_state(uint32_t state, uint8_t layer1, uint8_t layer2, uint8_t layer3)
{
    uint32_t mask12 = (1UL << layer1) | (1UL << layer2);
    uint32_t mask3  = 1UL << layer3;
    return (state & mask12) == mask12 ? (state | mask3) : (state & ~mask3);
}

void vial_update_tri_layer(uint8_t layer1, uint8_t layer2, uint8_t layer3)
{
    layer_state_set(update_tri_layer_state(layer_state, layer1, layer2, layer3));
}

static uint8_t *get_macro_start(uint8_t id)
{
    // Check the last byte of the buffer.
    // If it's not zero, then we are in the middle
    // of buffer writing, possibly an aborted buffer
    // write. So do nothing.
    uint8_t *p = (uint8_t *)(EEKEYMAP_MACRO_START_ADDR + EEKEYMAP_MACRO_SIZE - 1);
#if 1
    uint8_t data = eeprom_read_byte(p);
    if (data != 0) {
        amk_printf("macro buffer invalid: end=%d\n", data);
        return NULL;
    }
#endif

    // Skip N null characters
    // p will then point to the Nth macro
    p         = (uint8_t *)(EEKEYMAP_MACRO_START_ADDR);
    uint8_t *end = (uint8_t *)(EEKEYMAP_MACRO_START_ADDR + EEKEYMAP_MACRO_SIZE);
    while (id > 0) {
        // If we are past the end of the buffer, then the buffer
        // contents are garbage, i.e. there were not DYNAMIC_KEYMAP_MACRO_COUNT
        // nulls in the buffer.
        if (p == end) {
            return NULL;
        }
        if (eeprom_read_byte(p) == 0) {
            --id;
        }
        ++p;
    }

    return p;
}

bool vial_macro_play(uint8_t id, uint32_t *offset, uint32_t *delay)
{
    if (id >= amk_keymap_macro_get_count()) {
        return false;
    }

    uint8_t *p = NULL;
    if (*offset == 0) {
        p = get_macro_start(id);
    } else {
        p = (uint8_t*)(*offset);
    }

    if (!p) {
        return false;
    }

    // Send the macro string one or three chars at a time
    // by making temporary 1 or 3 char strings
    char data[4] = {0, 0, 0, 0};
    // We already checked there was a null at the end of
    // the buffer, so this cannot go past the end
    data[0] = eeprom_read_byte(p++);
    data[1] = 0;
    // Stop at the null terminator of this macro string
    if (data[0] == 0) {
        return false;
    }

    if (data[0] == SS_QMK_PREFIX) {
        // If the char is magic, process it as indicated by the next character
        // (tap, down, up, delay)
        data[1] = eeprom_read_byte(p++);
        if (data[1] == 0) {
            return false;
        }

        if (data[1] == SS_TAP_CODE || data[1] == SS_DOWN_CODE || data[1] == SS_UP_CODE) {
            // For tap, down, up, just stuff it into the array and send_string it
            data[2] = eeprom_read_byte(p++);
            if (data[2] != 0) {
                send_string(data);
            }
            *offset = (uint32_t)p;
            return true;
        }

        if (data[1] == SS_DELAY_CODE) {
            // For delay, decode the delay and wait_ms for that amount
            uint8_t d0 = eeprom_read_byte(p++);
            uint8_t d1 = eeprom_read_byte(p++);
            if (d0 == 0 || d1 == 0)
                *delay = 0;
            // we cannot use 0 for these, need to subtract 1 and use 255 instead of 256 for delay calculation
            int ms = (d0 - 1) + (d1 - 1) * 255;
            *delay = ms;
            *offset = (uint32_t)p;
            return true;
        }
    } else {
        // If the char wasn't magic, just send it
        send_string(data);
        *offset = (uint32_t)p;
        return true;
    }

    return false;
}