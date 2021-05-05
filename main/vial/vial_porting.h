/**
 * @file vial_porting.h
 *  porting the via&vial to amk
 * 
 *  original from qmk's via&vial
 */

#pragma once

#include <stdint.h>
#include <string.h>
#include "eeprom_manager.h"
#include "timer.h"
#include "amk_keymap.h"
#include "matrix.h"
#include "usb_descriptors.h"

// Keyboard level code can change where VIA stores the magic.
// The magic is the build date YYMMDD encoded as BCD in 3 bytes,
// thus installing firmware built on a different date to the one
// already installed can be detected and the EEPROM data is reset.
// The only reason this is important is in case EEPROM usage changes
// and the EEPROM was not explicitly reset by bootmagic lite.
#ifndef VIA_EEPROM_MAGIC_ADDR
#    define VIA_EEPROM_MAGIC_ADDR (EECONFIG_SIZE)
#endif

#define VIA_EEPROM_LAYOUT_OPTIONS_ADDR (VIA_EEPROM_MAGIC_ADDR + 3)

// Changing the layout options size after release will invalidate EEPROM,
// but this is something that should be set correctly on initial implementation.
// 1 byte is enough for most uses (i.e. 8 binary states, or 6 binary + 1 ternary/quaternary )
#ifndef VIA_EEPROM_LAYOUT_OPTIONS_SIZE
#    define VIA_EEPROM_LAYOUT_OPTIONS_SIZE 1
#endif

// Allow override of the layout options default value.
// This requires advanced knowledge of how VIA stores layout options
// and is only really useful for setting a boolean layout option
// state to true by default.
#ifndef VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT
#    define VIA_EEPROM_LAYOUT_OPTIONS_DEFAULT 0x00000000
#endif

// The end of the EEPROM memory used by VIA
// By default, dynamic keymaps will start at this if there is no
// custom config
#define VIA_EEPROM_CUSTOM_CONFIG_ADDR (VIA_EEPROM_LAYOUT_OPTIONS_ADDR + VIA_EEPROM_LAYOUT_OPTIONS_SIZE)

#ifndef VIA_EEPROM_CUSTOM_CONFIG_SIZE
#    define VIA_EEPROM_CUSTOM_CONFIG_SIZE 0
#endif

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