/**
 * @file vial_porting.h
 *  porting the via&vial to amk
 * 
 *  codes from qmk's via&vial
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

#pragma once

#include <stdint.h>
#include <string.h>
#include "amk_eeprom.h"
#include "timer.h"
#include "amk_keymap.h"
#include "matrix.h"
#include "usb_descriptors.h"
#include "keycode_convert.h"

// This is changed only when the command IDs change,
// so VIA Configurator can detect compatible firmware.
#define VIA_PROTOCOL_VERSION 0x0009

enum via_command_id {
    id_get_protocol_version                 = 0x01,  // always 0x01
    id_get_keyboard_value                   = 0x02,
    id_set_keyboard_value                   = 0x03,
    id_dynamic_keymap_get_keycode           = 0x04,
    id_dynamic_keymap_set_keycode           = 0x05,
    id_dynamic_keymap_reset                 = 0x06,
    id_lighting_set_value                   = 0x07,
    id_lighting_get_value                   = 0x08,
    id_lighting_save                        = 0x09,
    id_eeprom_reset                         = 0x0A,
    id_bootloader_jump                      = 0x0B,
    id_dynamic_keymap_macro_get_count       = 0x0C,
    id_dynamic_keymap_macro_get_buffer_size = 0x0D,
    id_dynamic_keymap_macro_get_buffer      = 0x0E,
    id_dynamic_keymap_macro_set_buffer      = 0x0F,
    id_dynamic_keymap_macro_reset           = 0x10,
    id_dynamic_keymap_get_layer_count       = 0x11,
    id_dynamic_keymap_get_buffer            = 0x12,
    id_dynamic_keymap_set_buffer            = 0x13,
    id_vial_prefix                          = 0xFE,
    id_unhandled                            = 0xFF,
};

enum via_keyboard_value_id {
    id_uptime              = 0x01,  //
    id_layout_options      = 0x02,
    id_switch_matrix_state = 0x03
};

#define VIAL_PROTOCOL_VERSION ((uint32_t)0x00000003)
/* Fake encoder position in keyboard matrix, can't use 255 as that is immediately rejected by IS_NOEVENT  */
#define VIAL_ENCODER_MATRIX_MAGIC 254

extern int vial_unlocked;
extern int vial_unlock_in_progress;

enum vial_command_id{
    vial_get_keyboard_id    = 0x00,
    vial_get_size           = 0x01,
    vial_get_def            = 0x02,
    vial_get_encoder        = 0x03,
    vial_set_encoder        = 0x04,
    vial_get_unlock_status  = 0x05,
    vial_unlock_start       = 0x06,
    vial_unlock_poll        = 0x07,
    vial_lock               = 0x08,
};

void vial_process(uint8_t *data, uint8_t length);
void vial_task(void);