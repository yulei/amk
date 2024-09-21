/**
 * @file amk_protocol.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/


#pragma once

#include <stdint.h>

#define amk_protocol_version    0x01
#define amk_protocol_prefix     0xFD
#define amk_protocol_ok         0xAA
#define amk_protocol_fail       0x55

enum {
    amk_protocol_get_version            = 0,
    amk_protocol_get_apc                = 1,
    amk_protocol_set_apc                = 2,
    amk_protocol_get_rt                 = 3,
    amk_protocol_set_rt                 = 4,
    amk_protocol_get_dks                = 5,
    amk_protocol_set_dks                = 6,
    amk_protocol_get_poll_rate          = 7,
    amk_protocol_set_poll_rate          = 8,
    amk_protocol_get_down_debounce      = 9,
    amk_protocol_set_down_debounce      = 10,
    amk_protocol_get_up_debounce        = 11,
    amk_protocol_set_up_debounce        = 12,
    amk_protocol_get_nkro               = 13,
    amk_protocol_set_nkro               = 14,
    amk_protocol_get_ms_config          = 15,
    amk_protocol_set_ms_config          = 16,
    amk_protocol_get_rt_sens            = 17,
    amk_protocol_set_rt_sens            = 18,
    amk_protocol_get_top_sens           = 19,
    amk_protocol_set_top_sens           = 20,
    amk_protocol_get_btm_sens           = 21,
    amk_protocol_set_btm_sens           = 22,
    amk_protocol_get_apc_sens           = 23,
    amk_protocol_set_apc_sens           = 24,
    amk_protocol_get_noise_sens         = 25,
    amk_protocol_set_noise_sens         = 26,
    amk_protocol_get_rgb_strip_count    = 27,
    amk_protocol_get_rgb_strip_param    = 28,
    amk_protocol_get_rgb_strip_led      = 29,
    amk_protocol_set_rgb_strip_led      = 30,
    amk_protocol_get_rgb_strip_mode     = 31,
    amk_protocol_set_rgb_strip_mode     = 32,
    amk_protocol_get_rgb_indicator_led  = 33,
    amk_protocol_set_rgb_indicator_led  = 34,
    amk_protocol_get_file_system_info   = 35,
    amk_protocol_get_file_info          = 36,
    amk_protocol_open_file              = 37,
    amk_protocol_write_file             = 38,
    amk_protocol_read_file              = 39,
    amk_protocol_close_file             = 40,
    amk_protocol_delete_file            = 41,
    amk_protocol_display_control        = 42,
    amk_protocol_get_rgb_matrix_info    = 43,
    amk_protocol_get_rgb_matrix_row_info= 44,
    amk_protocol_get_rgb_matrix_mode    = 45,
    amk_protocol_set_rgb_matrix_mode    = 46,
    amk_protocol_get_rgb_matrix_led     = 47,
    amk_protocol_set_rgb_matrix_led     = 48,
    amk_protocol_get_snaptap            = 49,
    amk_protocol_set_snaptap            = 50,
    amk_protocol_get_snaptap_count      = 51,
    amk_protocol_get_snaptap_config     = 52,
    amk_protocol_set_snaptap_config     = 53,
    amk_protocol_set_datetime           = 54,
    amk_protocol_get_datetime           = 55,
};

void amk_protocol_process(uint8_t *msg, uint8_t length);
