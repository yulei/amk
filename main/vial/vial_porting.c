/**
 * @file vial_porting.c
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vial_porting.h"
#include "progmem.h"
#include "vial_generated_keyboard_definition.h"
#include "tusb.h"
#include "amk_printf.h"

#ifndef VIAL_DEBUG_PRINT
#define VIAL_DEBUG_PRINT 1
#endif

#if VIAL_DEBUG_PRINT
#define vial_debug  amk_printf
#else
#define vial_debug(...)
#endif

#define VIAL_UNLOCK_COUNTER_MAX 50

#ifdef VIAL_INSECURE
#pragma message "Building Vial-enabled firmware in insecure mode."
int vial_unlocked = 1;
#else
int vial_unlocked = 0;
#endif
int vial_unlock_in_progress = 0;
static int vial_unlock_counter = 0;
static uint16_t vial_unlock_timer;

#ifndef VIAL_INSECURE
static uint8_t vial_unlock_combo_rows[] = VIAL_UNLOCK_COMBO_ROWS;
static uint8_t vial_unlock_combo_cols[] = VIAL_UNLOCK_COMBO_COLS;
#define VIAL_UNLOCK_NUM_KEYS (sizeof(vial_unlock_combo_rows)/sizeof(vial_unlock_combo_rows[0]))
//_Static_assert(VIAL_UNLOCK_NUM_KEYS < 15, "Max 15 unlock keys");
//_Static_assert(sizeof(vial_unlock_combo_rows) == sizeof(vial_unlock_combo_cols), "The number of unlock cols and rows should be the same");
#endif

#define VIAL_RAW_EPSIZE 32

#ifndef VIAL_ENCODER_KEYCODE_DELAY
#define VIAL_ENCODER_KEYCODE_DELAY 10
#endif


__attribute__((weak)) void vial_receive_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &(data[0]);
    *command_id         = id_unhandled;
}

void vial_handle_cmd(uint8_t *msg, uint8_t length) {
    /* All packets must be fixed 32 bytes */
    if (length != VIAL_RAW_EPSIZE)
        return;

    /* msg[0] is 0xFE -- prefix vial magic */
    switch (msg[1]) {
        /* Get keyboard ID and Vial protocol version */
        case vial_get_keyboard_id: {
            uint8_t keyboard_uid[] = VIAL_KEYBOARD_UID;

            msg[0] = VIAL_PROTOCOL_VERSION & 0xFF;
            msg[1] = (VIAL_PROTOCOL_VERSION >> 8) & 0xFF;
            msg[2] = (VIAL_PROTOCOL_VERSION >> 16) & 0xFF;
            msg[3] = (VIAL_PROTOCOL_VERSION >> 24) & 0xFF;
            memcpy(&msg[4], keyboard_uid, 8);
            vial_debug("vial_get_keyboard_id\n");
            break;
        }
        /* Retrieve keyboard definition size */
        case vial_get_size: {
            uint32_t sz = sizeof(keyboard_definition);
            msg[0] = sz & 0xFF;
            msg[1] = (sz >> 8) & 0xFF;
            msg[2] = (sz >> 16) & 0xFF;
            msg[3] = (sz >> 24) & 0xFF;
            vial_debug("vial_get_size, %d\n", sz);
            break;
        }
        /* Retrieve 32-bytes block of the definition, page ID encoded within 2 bytes */
        case vial_get_def: {
            uint32_t page = msg[2] + (msg[3] << 8);
            uint32_t start = page * VIAL_RAW_EPSIZE;
            uint32_t end = start + VIAL_RAW_EPSIZE;
            if (end < start || start >= sizeof(keyboard_definition))
                return;
            if (end > sizeof(keyboard_definition))
                end = sizeof(keyboard_definition);
            memcpy(msg, &keyboard_definition[start], end - start);
            vial_debug("vial_get_def, %d--%d\n", start, end);
            break;
        }

        case vial_get_unlock_status: {
            /* Reset message to all FF's */
            memset(msg, 0xFF, length);
            /* First byte of message contains the status: whether board is unlocked */
            msg[0] = vial_unlocked;
            /* Second byte is whether unlock is in progress */
            msg[1] = vial_unlock_in_progress;
#ifndef VIAL_INSECURE
            /* Rest of the message are keys in the matrix that should be held to unlock the board */
            for (size_t i = 0; i < VIAL_UNLOCK_NUM_KEYS; ++i) {
                msg[2 + i * 2] = vial_unlock_combo_rows[i];
                msg[2 + i * 2 + 1] = vial_unlock_combo_cols[i];
            }
#endif
            vial_debug("vial_get_unlock_status, %d--%d\n", vial_unlocked, vial_unlock_in_progress);
            break;
        }
        case vial_unlock_start: {
            vial_unlock_in_progress = 1;
            vial_unlock_counter = VIAL_UNLOCK_COUNTER_MAX;
            vial_unlock_timer = timer_read();
            vial_debug("vial_unlock_start\n");
            break;
        }
        case vial_unlock_poll: {
#ifndef VIAL_INSECURE
            if (vial_unlock_in_progress) {
                int holding = 1;
                for (size_t i = 0; i < VIAL_UNLOCK_NUM_KEYS; ++i)
                    holding &= matrix_is_on(vial_unlock_combo_rows[i], vial_unlock_combo_cols[i]);

                if (timer_elapsed(vial_unlock_timer) > 100 && holding) {
                    vial_unlock_timer = timer_read();

                    vial_unlock_counter--;
                    if (vial_unlock_counter == 0) {
                        /* ok unlock succeeded */
                        vial_unlock_in_progress = 0;
                        vial_unlocked = 1;
                    }
                } else {
                    vial_unlock_counter = VIAL_UNLOCK_COUNTER_MAX;
                }
            }
#endif
            msg[0] = vial_unlocked;
            msg[1] = vial_unlock_in_progress;
            msg[2] = vial_unlock_counter;
            vial_debug("vial_unlock_poll\n");
            break;
        }
        case vial_lock: {
#ifndef VIAL_INSECURE
            vial_unlocked = 0;
#endif
            vial_debug("vial_lock\n");
            break;
        }
    }
}

static uint32_t via_get_layout_options(void)
{
    return eeconfig_read_layout_options();
}

static void via_set_layout_options(uint32_t value)
{
    eeconfig_write_layout_options((uint8_t) value);
}

__attribute__((__weak__))
bool vial_process_kb(uint8_t *data, uint8_t length)
{
    return false;
}

void vial_process(uint8_t *data, uint8_t length) 
{
    if (vial_process_kb(data, length)) return;

    uint8_t *command_id   = &(data[0]);
    uint8_t *command_data = &(data[1]);

#ifdef VIAL_ENABLE
    /* When unlock is in progress, we can only react to a subset of commands */
    if (vial_unlock_in_progress) {
        if (data[0] != id_vial_prefix)
            goto skip;
        uint8_t cmd = data[1];
        if (cmd != vial_get_keyboard_id && cmd != vial_get_size && cmd != vial_get_def && cmd != vial_get_unlock_status && cmd != vial_unlock_start && cmd != vial_unlock_poll)
            goto skip;
    }
#endif

    switch (*command_id) {
        case id_get_protocol_version: {
            command_data[0] = VIA_PROTOCOL_VERSION >> 8;
            command_data[1] = VIA_PROTOCOL_VERSION & 0xFF;
            vial_debug("via: id_get_protocol_version\n");
            break;
        }
        case id_get_keyboard_value: {
            vial_debug("via: id_get_keyboard_value...");
            switch (command_data[0]) {
                case id_uptime: {
                    uint32_t value  = timer_read32();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    vial_debug("id_uptime\n");
                    break;
                }
                case id_layout_options: {
                    uint32_t value  = via_get_layout_options();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    vial_debug("id_layout_option: %d\n", value);
                    break;
                }
                case id_switch_matrix_state: {
#ifdef VIAL_ENABLE
                    /* Disable wannabe keylogger unless unlocked */
                    if (!vial_unlocked)
                        goto skip;
#endif

#if ((MATRIX_COLS / 8 + 1) * MATRIX_ROWS <= 28)
                    uint8_t i = 1;
                    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                        matrix_row_t value = matrix_get_row(row);
#    if (MATRIX_COLS > 24)
                        command_data[i++] = (value >> 24) & 0xFF;
#    endif
#    if (MATRIX_COLS > 16)
                        command_data[i++] = (value >> 16) & 0xFF;
#    endif
#    if (MATRIX_COLS > 8)
                        command_data[i++] = (value >> 8) & 0xFF;
#    endif
                        command_data[i++] = value & 0xFF;
                    }
#endif
                    vial_debug("id_switch_matrix_state\n");
                    break;
                }
                default: {
                    vial_receive_kb(data, length);
                    vial_debug("unknown command\n");
                    break;
                }
            }
            break;
        }
        case id_set_keyboard_value: {
            vial_debug("via: id_set_keyboard_value...");
            switch (command_data[0]) {
                case id_layout_options: {
                    uint32_t value = ((uint32_t)command_data[1] << 24) | ((uint32_t)command_data[2] << 16) | ((uint32_t)command_data[3] << 8) | (uint32_t)command_data[4];
                    via_set_layout_options(value);
                    vial_debug("id_layout_options\n");
                    break;
                }
                default: {
                    vial_receive_kb(data, length);
                    vial_debug("unknown command\n");
                    break;
                }
            }
            break;
        }
        case id_dynamic_keymap_get_keycode: {
            uint16_t keycode = amk_keymap_get(command_data[0], command_data[1], command_data[2]);//dynamic_keymap_get_keycode(command_data[0], command_data[1], command_data[2]);
            keycode = amk_to_vial(keycode);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            vial_debug("via: id_dynamic_keymap_get_keycode: %d\n", keycode);
            break;
        }
        case id_dynamic_keymap_set_keycode: {
            uint16_t keycode = (command_data[3] << 8) | command_data[4];
            keycode = vial_to_amk(keycode);
            amk_keymap_set(command_data[0], command_data[1], command_data[2], keycode);
            vial_debug("via: id_dynamic_keymap_set_keycode: %d\n",(command_data[3] << 8) | command_data[4]);
            break;
        }
        case id_dynamic_keymap_reset: {
            amk_keymap_reset();
            amk_keymap_macro_reset();
            vial_debug("via: id_dynamic_keymap_reset\n");
            break;
        }
        case id_lighting_set_value: {
            // Return the unhandled state
            *command_id = id_unhandled;
            vial_debug("unhandled command: id_lighting_set_value\n");
            break;
        }
        case id_lighting_get_value: {
            // Return the unhandled state
            *command_id = id_unhandled;
            vial_debug("unhandled command: id_lighting_get_value\n");
            break;
        }
        case id_lighting_save: {
            // Return the unhandled state
            *command_id = id_unhandled;
            vial_debug("unhandled command: id_lighting_save\n");
            break;
        }
        case id_dynamic_keymap_macro_get_count: {
            //command_data[0] = dynamic_keymap_macro_get_count();
            //*command_id = id_unhandled;
            command_data[0] = amk_keymap_macro_get_count();
            vial_debug("vial: id_dynamic_keymap_macro_get_count: %d\n", command_data[0]);
            break;
        }
        case id_dynamic_keymap_macro_get_buffer_size: {
            uint16_t size   = //0;
            amk_keymap_macro_get_buffer_size();
            command_data[0] = size >> 8;
            command_data[1] = size & 0xFF;
            *command_id = id_unhandled;
            vial_debug("vial: id_dynamic_keymap_macro_get_buffer_size: %d\n", size);
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            if (size <= 28) {
                amk_keymap_macro_get_buffer(offset, size, &command_data[3]);
            } else {
                *command_id = id_unhandled;
            }
            vial_debug("vial: id_dynamic_keymap_macro_get_buffer\n");
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: {
#ifdef VIAL_ENABLE
            /* Until keyboard is unlocked, don't allow changing macros */
            if (!vial_unlocked)
                goto skip;
#endif
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            if (size <= 28) {
                amk_keymap_macro_set_buffer(offset, size, &command_data[3]);
            } else {
                *command_id = id_unhandled;
            }
            vial_debug("vial: id_dynamic_keymap_macro_set_buffer\n");
            break;
        }
        case id_dynamic_keymap_macro_reset: {
            amk_keymap_macro_reset();
            //*command_id = id_unhandled;
            vial_debug("vial: id_dynamic_keymap_macro_reset\n");
            break;
        }
        case id_dynamic_keymap_get_layer_count: {
            command_data[0] = amk_keymap_get_layer_count();
            vial_debug("via: id_dynamic_keymap_get_layer_count: %d\n", command_data[0]);
            break;
        }
        case id_dynamic_keymap_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            if (size <= 28) {
                amk_keymap_get_buffer(offset, size, &command_data[3]);
                convert_keycode_buffer(&command_data[3], size, true);
            }
            vial_debug("vial: id_dynamic_keymap_get_buffer: offset:%d, size:%d\n", offset, size);
            break;
        }
        case id_dynamic_keymap_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            if (size <= 28) {
                convert_keycode_buffer(&command_data[3], size, false);
                amk_keymap_set_buffer(offset, size, &command_data[3]);
            }
            vial_debug("via: id_dynamic_keymap_set_buffer: offset:%d, size:%d\n");
            break;
        }
#if defined(VIAL_ENABLE) && !defined(VIAL_INSECURE)
        /* As VIA removed bootloader jump entirely, we shall only keep it for secure builds */
        case id_bootloader_jump: {
            /* Until keyboard is unlocked, don't allow jumping to bootloader */
            if (!vial_unlocked)
                goto skip;
            // Need to send data back before the jump
            // Informs host that the command is handled
            //raw_hid_send(data, length);
            // Give host time to read it
            //wait_ms(100);
            //bootloader_jump();
            *command_id = id_unhandled;
            vial_debug("unhandled command: id_bootloader_jump\n");
            break;
        }
#endif
#ifdef VIAL_ENABLE
        case id_vial_prefix: {
            vial_handle_cmd(data, length);
            break;
        }
#endif
        default: {
            // The command ID is not known
            // Return the unhandled state
            *command_id = id_unhandled;
            vial_debug("unknown command\n");
            break;
        }
    }
#ifdef VIAL_ENABLE
skip:
#endif
    // Return the same buffer, optionally with values changed
    // (i.e. returning state to the host, or the unhandled state).
    vial_send(data, length);
}

#ifndef TINYUSB_ENABLE
#include "generic_hal.h"
#include "usb_device.h"
#include "usbd_composite.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

void vial_send(uint8_t *data, uint8_t length)
{
    if (usbd_comp_itf_ready(&hUsbDeviceFS, ITF_NUM_VIAL)) {
        usbd_comp_send(&hUsbDeviceFS, HID_REPORT_ID_VIAL, data, length);
    } else {
        // save to queue
        vial_debug("vial send busy\n");
    }
}
#else
void vial_send(uint8_t *data, uint8_t length)
{
    if (tud_hid_n_ready(ITF_NUM_VIAL)) {
        tud_hid_n_report(ITF_NUM_VIAL, 0, data, length);
    } else {
        // save to queue
        vial_debug("vial send busy\n");
    }
}
#endif

__attribute__((__weak__))
void vial_task_kb(void) {}

void vial_task(void)
{
    vial_task_kb();
}