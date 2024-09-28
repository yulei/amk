/**
 * @file amk_protocol.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 *
*/

#include <string.h>

#include "amk_protocol.h"
#include "amk_store.h"
#include "amk_printf.h"
#ifdef AMK_APC_ENABLE
#include "amk_apc.h"
#endif
#ifdef AMK_DKS_ENABLE
#include "amk_dks.h"
#endif
#ifdef AMK_SNAPTAP_ENABLE
#include "amk_snaptap.h"
#endif
#ifdef MSC_ENABLE
#include "anim_file.h"
#endif

#if defined(RGB_LINEAR_ENABLE) || defined(RGB_INDICATOR_ENABLE) || defined(RGB_MATRIX_ENABLE)
#include "rgb_led.h"
#endif

#if defined(RGB_INDICATOR_ENABLE)
#include "rgb_indicator.h"
#endif

#ifdef CUSTOMRGB_ENABLE 
#include "rgb_matrix_stub.h"
#include "customrgb.h"
#endif

#include "timer.h"

#include "keycode_config.h"

#define AMK_PROTOCOL_DEBUG 1

#if AMK_PROTOCOL_DEBUG
#define ap_debug  amk_printf
#else
#define ap_debug(...)
#endif

__attribute__((weak)) void amk_set_datetime_kb(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday, uint8_t hour, uint8_t minute, uint8_t second)
{
}

void amk_protocol_process(uint8_t *msg, uint8_t length)
{
     /* msg[0] is 0xFD -- prefix of amk protocol */
    switch (msg[1]) {
    case amk_protocol_get_version:
        msg[2] = amk_protocol_ok;
        msg[3] = amk_protocol_version;
        ap_debug("AMK Protocol: get version:%d\n", msg[3]);
        break;
#ifdef AMK_APC_ENABLE
    case amk_protocol_get_apc:
        // msg[2]: row, msg[3]: column, msg[4]: profile
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            uint8_t profile = msg[4];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS) && (profile < AMK_STORE_APCRT_PROFILE_COUNT)) {
                msg[2] = amk_protocol_ok;
                uint16_t apc = amk_store_get_apc(profile, row, col);
                msg[3]  = apc >> 8;
                msg[4]  = apc & 0xFF;
                ap_debug("AMK Protocol: get apc:%d\n", apc);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: get apc failed, row=%d, col=%d, profile=%d\n", row, col, profile);
            }
        }
        break;
    case amk_protocol_set_apc:
        // msg[2]: row, msg[3]: column, msg[4]: apc high, msg[5]: apc low, msg[6]: profile
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            uint8_t profile = msg[6];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                uint16_t apc = (msg[4] << 8) | msg[5];
                amk_store_set_apc(profile, row, col, apc);
                apc_matrix_update_interval_at(profile, row, col);
                ap_debug("AMK Protocol: set apc: row=%d, col=%d, apc=%d, profile=%d\n", row, col, apc, profile);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: set apc failed: row=%d, col=%d, profile=%d\n", row, col, profile);
            }
        }
        break;
    case amk_protocol_get_rt:
        // msg[2]: row, msg[3]: column, msg[4]: profile
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            uint8_t profile = msg[4];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS) && (profile < AMK_STORE_APCRT_PROFILE_COUNT)) {
                msg[2] = amk_protocol_ok;
                uint16_t rt = amk_store_get_rt(profile, row, col);
                msg[3]  = rt >> 8;
                msg[4]  = rt & 0xFF;
                ap_debug("AMK Protocol: get rt at [%d,%d]:%d, profile=%d\n", row, col, rt, profile);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: get rt failed, row=%d, col=%d, profile=%d\n", row, col, profile);
            }
        }
        break;
    case amk_protocol_set_rt:
        // msg[2]: row, msg[3]: column, msg[4]: rt high, msg[5]: rt low, msg[6]: profile
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            uint8_t profile = msg[6];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS) && (profile < AMK_STORE_APCRT_PROFILE_COUNT)) {
                msg[2] = amk_protocol_ok;
                uint16_t rt = (msg[4] << 8) | msg[5];
                amk_store_set_rt(profile, row, col, rt);
                apc_matrix_update_interval_at(profile, row, col);
                ap_debug("AMK Protocol: set rt: row=%d, col=%d, apc=%d, profile=%d\n", row, col, rt, profile);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: set rt failed: row=%d, col=%d, profile=%d\n", row, col, profile);
            }
        }
        break;
#endif
#ifdef AMK_DKS_ENABLE
    case amk_protocol_get_dks:
        // msg[2]: row, msg[3]: column, msg[4...15]: dks
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                struct amk_dks dks = {0};
                amk_store_get_dks(row, col, &dks);
                uint8_t j = 3;
                for (int i = 0; i < AMK_DKS_EVENT_MAX; i++) {
                    msg[j++] = dks.events[i];
                }
                for (int i = 0; i < AMK_DKS_KEY_MAX; i++) {
                    msg[j++] = (uint8_t)(dks.keys[i] >> 8);
                    msg[j++] = (uint8_t)(dks.keys[i] & 0xFF);
                }
                ap_debug("AMK Protocol: get dks at [%d,%d]\n", row, col);
                ap_debug("Event:{0x%x-0x%x-0x%x-0x%x}\n", dks.events[0], dks.events[1], dks.events[2], dks.events[3]);
                ap_debug("Keys:{0x%x-0x%x-0x%x-0x%x}\n", dks.keys[0], dks.keys[1], dks.keys[2], dks.keys[3]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: get dks failed\n");
            }
        }
        break;
    case amk_protocol_set_dks:
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                struct amk_dks dks;
                uint8_t j = 4;
                for (int i = 0; i < AMK_DKS_EVENT_MAX; i++) {
                    dks.events[i] = msg[j++];
                }
                for (int i = 0; i < AMK_DKS_KEY_MAX; i++) {
                    uint16_t high = msg[j++];
                    uint16_t low = msg[j++];
                    dks.keys[i] = (high<<8) | low;
                }
                amk_store_set_dks(row, col, &dks);
                dks_matrix_update_action_at(row, col);
                ap_debug("AMK Protocol: set dks\n");
                ap_debug("Event:{0x%x-0x%x-0x%x-0x%x}\n", dks.events[0], dks.events[1], dks.events[2], dks.events[3]);
                ap_debug("Keys:{0x%x-0x%x-0x%x-0x%x}\n", dks.keys[0], dks.keys[1], dks.keys[2], dks.keys[3]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: set dks failed\n");
            }
        }
        break;
#endif
#ifdef AMK_SNAPTAP_ENABLE
    case amk_protocol_get_snaptap:
        // msg[2]: index
        {
            uint8_t index = msg[2];
            if (index < AMK_SNAPTAP_COUNT) {
                msg[2] = amk_protocol_ok;
                struct amk_snaptap snaptap = {0};
                amk_store_get_snaptap(index, &snaptap);
                msg[3] = snaptap.first_row;
                msg[4] = snaptap.first_col;
                msg[5] = snaptap.second_row;
                msg[6] = snaptap.second_col;
                msg[7] = snaptap.mode;
                ap_debug("AMK Protocol: get snaptap at index(%d),first(%d,%d),second(%d,%d),mode(%d)\n", index, 
                        msg[3], msg[4], msg[5], msg[6], msg[7]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: get snaptap at %d failed\n", index);
            }
        }
        break;
    case amk_protocol_set_snaptap:
        {
            // msg[2]: index, msg[3]: first row, msg[4]: first column, msg[5]: second row, msg[6]: second column, msg[7]: mode
            uint8_t index = msg[2];

            if (index < AMK_SNAPTAP_COUNT) {
                msg[2] = amk_protocol_ok;
                struct amk_snaptap snaptap = {0};
                snaptap.first_row = msg[3];
                snaptap.first_col = msg[4];
                snaptap.second_row = msg[5];
                snaptap.second_col = msg[6];
                snaptap.mode = msg[7];
                amk_store_set_snaptap(index, &snaptap);

                snaptap_pair_t pair;
                pair.first.row = snaptap.first_row;
                pair.first.col = snaptap.first_col;
                pair.second.row = snaptap.second_row;
                pair.second.col = snaptap.second_col;
                pair.mode = snaptap.mode;

                snaptap_update(index, &pair);
                ap_debug("AMK Protocol: set snaptap at index(%d),first(%d,%d),second(%d,%d),mode(%d)\n", index, 
                        msg[3], msg[4], msg[5], msg[6], msg[7]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: set snaptap at %d failed\n", index);
            }
        }
        break;
    case amk_protocol_get_snaptap_count:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = AMK_SNAPTAP_COUNT;
            ap_debug("AMK Protocol: get snaptap count (%d) \n", msg[3]);
        }
        break;
    case amk_protocol_get_snaptap_config:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = amk_store_get_snaptap_config();
            ap_debug("AMK Protocol: get snaptap config (%d) \n", msg[3]);
        }
        break;
    case amk_protocol_set_snaptap_config:
        {
            uint8_t config = msg[2];
            msg[2] = amk_protocol_ok;
            amk_store_set_snaptap_config(config);
            ap_debug("AMK Protocol: set snaptap config (%d) \n", msg[3]);
            snaptap_update_config(config);
        }
        break;
#endif
    case amk_protocol_set_datetime:
        {
            uint16_t year = (msg[2] << 8) | msg[3];
            uint8_t month = msg[4];
            uint8_t day = msg[5];
            uint8_t weekday = msg[6];
            uint8_t hour = msg[7];
            uint8_t minute = msg[8];
            uint8_t second = msg[9];
            ap_debug("AMK Protocol: set date time(%d/%d/%d-%d %d:%d:%d)\n", year, month, day, weekday, hour, minute, second);
            msg[2] = amk_protocol_ok;
            amk_set_datetime_kb(year, month, day, weekday, hour, minute, second);
        }
        break;
    case amk_protocol_get_datetime:
        {
            msg[2] = amk_protocol_fail;
        }
        break;
    case amk_protocol_get_poll_rate:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = eeconfig_read_usb();
            ap_debug("AMK Protocol: get poll rate = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_poll_rate:
        {
            uint8_t pr = msg[2];
            uint8_t cur = eeconfig_read_usb();
            if (cur != pr) {
                eeconfig_update_usb(pr);
                extern uint32_t amk_delay_reset;
                amk_delay_reset = timer_read32();
            }
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set poll rate = %d\n", pr);
        }
        break;
    case amk_protocol_get_down_debounce:
        {
            uint8_t debounce = eeconfig_read_debounce();
            msg[2] = amk_protocol_ok;
            msg[3] = (debounce & 0xF0) >> 4;
            ap_debug("AMK Protocol: get down debounce = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_down_debounce:
        {
            uint8_t debounce = eeconfig_read_debounce() & 0x0F;
            debounce |= (msg[2] & 0x0F) << 4;
            eeconfig_update_debounce(debounce);
            extern uint8_t debounce_press;
            debounce_press = msg[2];
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set down debounce = %d\n", (debounce&0xF0) >> 4);
        }
        break;
    case amk_protocol_get_up_debounce:
        {
            uint8_t debounce = eeconfig_read_debounce();
            msg[2] = amk_protocol_ok;
            msg[3] = debounce & 0x0F;
            ap_debug("AMK Protocol: get up debounce = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_up_debounce:
        {
            uint8_t debounce = eeconfig_read_debounce() & 0xF0;
            debounce |= msg[2] & 0x0F;
            extern uint8_t debounce_release;
            debounce_release = msg[2];
            eeconfig_update_debounce(debounce);
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set up debounce = %d\n", debounce & 0x0F);
        }
        break;
    case amk_protocol_get_nkro:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = keymap_config.nkro ? 1 : 0;
            ap_debug("AMK Protocol: get nkro = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_nkro:
        {
            bool old = keymap_config.nkro;
            keymap_config.nkro = msg[2] ? true : false;
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set nkro = %d\n", keymap_config.nkro);

            if (old != keymap_config.nkro) {
                eeconfig_update_keymap(keymap_config.raw);
            }
        }
        break;
#if defined(RGB_LINEAR_ENABLE)
        case amk_protocol_get_rgb_strip_count:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = RGB_SEGMENT_NUM;
            ap_debug("AMK Protocol: get rgb strip count = %d\n", msg[3]);
        }
        break;
        case amk_protocol_get_rgb_strip_param:
        {
            uint8_t index = msg[2];
            if (index < RGB_SEGMENT_NUM) {
                extern rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM];
                msg[2] = amk_protocol_ok;
                msg[3] = index;
                msg[4] = g_rgb_linear_params[index].config;
                msg[5] = g_rgb_linear_params[index].led_start;
                msg[6] = g_rgb_linear_params[index].led_num;
                ap_debug("AMK Protocol: get rgb strip param at %d, config=%d, start=%d, num=%d\n", index, msg[4], msg[5], msg[6]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to get rgb strip param, index = %d\n", index);
            }
        }
        break;
        case amk_protocol_get_rgb_strip_led:
        {
            uint8_t index = msg[2];
            if (index < RGB_LED_NUM) {
                msg[2] = amk_protocol_ok;
                msg[3] = index;
                rgb_led_strip_get_led(index, &msg[4], &msg[5], &msg[6], &msg[7]);
                ap_debug("AMK Protocol: get rgb strip led at %d, hue=%d, sat=%d, val=%d, param=0x%x\n", 
                        index, msg[4], msg[5], msg[6], msg[7]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to get rgb strip led, index = %d, led count = %d\n", index, RGB_LED_NUM);
            }
        }
        break;
        case amk_protocol_set_rgb_strip_led:
        {
            uint8_t index = msg[2];
            if (index < RGB_LED_NUM) {
                msg[2] = amk_protocol_ok;

                uint8_t hue = msg[3];
                uint8_t sat = msg[4];
                uint8_t val = msg[5];
                uint8_t param = msg[6];
                rgb_led_strip_set_led(index, hue, sat, val, param); 
                ap_debug("AMK Protocol: set rgb strip led at %d, hue=%d, sat=%d, val=%d, param=0x%x\n", index, hue, sat, val, param);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to set rgb strip led, index = %d, led count = %d\n", index, RGB_LED_NUM);
            }
        }
        break;
        case amk_protocol_get_rgb_strip_mode:
        {
            uint8_t index = msg[2];
            if (index < RGB_SEGMENT_NUM) {
                msg[2] = amk_protocol_ok;
                msg[3] = index;
                msg[4] = rgb_led_strip_get_mode(index);
                ap_debug("AMK Protocol: get rgb strip mode at %d, mode=%d\n", index, msg[3]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to get rgb strip mode, index = %d\n", index);
            }
        }
        break;
        case amk_protocol_set_rgb_strip_mode:
        {
            uint8_t index = msg[2];
            if (index < RGB_SEGMENT_NUM) {
                msg[2] = amk_protocol_ok;
                uint8_t mode = msg[3];
                rgb_led_strip_set_mode(index, mode);
                ap_debug("AMK Protocol: set rgb strip mode at %d, mode=%d\n", index, mode);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to set rgb strip mode, index = %d, mode = %d\n", index, msg[3]);
            }
        }
        break;
#endif
#if defined(RGB_INDICATOR_ENABLE)
        case amk_protocol_get_rgb_indicator_led:
        {
            uint8_t index = msg[2];
            if (index < RGB_LED_NUM) {
                msg[2] = amk_protocol_ok;
                msg[3] = index;
                rgb_indicator_get_led(index, &msg[4], &msg[5], &msg[6], &msg[7]);
                ap_debug("AMK Protocol: get rgb indicator led at %d, hue=%d, sat=%d, val=%d, param=0x%x\n", 
                        index, msg[4], msg[5], msg[6], msg[7]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to get rgb indicator led, index = %d, led count = %d\n", index, RGB_LED_NUM);
            }
        }
        break;
        case amk_protocol_set_rgb_indicator_led:
        {
            uint8_t index = msg[2];
            if (index < RGB_LED_NUM) {
                msg[2] = amk_protocol_ok;

                uint8_t hue = msg[3];
                uint8_t sat = msg[4];
                uint8_t val = msg[5];
                uint8_t param = msg[6];
                rgb_indicator_set_led(index, hue, sat, val, param); 
                ap_debug("AMK Protocol: set rgb indicator led at %d, hue=%d, sat=%d, val=%d, param=0x%x\n", index, hue, sat, val, param);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to set rgb indicator led, index = %d, led count = %d\n", index, RGB_LED_NUM);
            }
        }
        break;
#endif

#ifdef CUSTOMRGB_ENABLE
        case amk_protocol_get_rgb_matrix_info:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = g_rgb_matrix_params[0].led_start;
            msg[4] = g_rgb_matrix_params[0].led_num;
            ap_debug("AMK Protocol: get rgb matrix info : led start=%d, led total=%d\n", msg[3], msg[4]); 
        }
        break;
        case amk_protocol_get_rgb_matrix_row_info:
        {
            uint8_t index = msg[2];
            uint8_t row = msg[3];
            msg[2] = amk_protocol_ok;
            msg[3] = row;

            for (int i = 0; i < MATRIX_COLS; i++) {
                msg[4+i] = customrgb_get_matrix_led_index(index, row, i);
            }
            ap_debug("AMK Protocol: get rgb matrix info: index=%d, row=%d\n", index, row); 
        }
        break;
        case amk_protocol_get_rgb_matrix_mode:
        {
            uint8_t index = msg[2];

            msg[2] = amk_protocol_ok;
            msg[3] = customrgb_get_matrix_mode(index, &msg[4], &msg[5], &msg[6]);
            ap_debug("AMK Protocol: get rgb matrix mode: current=%d, custom=%d, total=%d, default=%d\n", msg[3], msg[4], msg[5], msg[6]); 
        }
        break;
        case amk_protocol_set_rgb_matrix_mode:
        {
            uint8_t index = msg[2];
            uint8_t mode = msg[3];
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set rgb matrix mode at index=%d, mode=%d\n", index, mode); 
            rgb_matrix_mode(mode);
        }
        break;
        case amk_protocol_get_rgb_matrix_led:
        {
            uint8_t index = msg[2];
            if (index < RGB_LED_NUM) {
                msg[2] = amk_protocol_ok;
                msg[3] = index;
                customrgb_get_led(index, &msg[4], &msg[5], &msg[6], &msg[7]);
                ap_debug("AMK Protocol: get rgb matrix led at %d, hue=%d, sat=%d, val=%d, param=0x%x\n", 
                        index, msg[4], msg[5], msg[6], msg[7]);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to get matrix led, index = %d, led count = %d\n", index, RGB_LED_NUM);
            }
        }
        break;
        case amk_protocol_set_rgb_matrix_led:
        {
            uint8_t index = msg[2];
            if (index < RGB_LED_NUM) {
                msg[2] = amk_protocol_ok;

                uint8_t hue = msg[3];
                uint8_t sat = msg[4];
                uint8_t val = msg[5];
                uint8_t param = msg[6];
                customrgb_set_led(index, hue, sat, val, param); 
                ap_debug("AMK Protocol: set rgb matrix led at %d, hue=%d, sat=%d, val=%d, param=0x%x\n", index, hue, sat, val, param);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: failed to set rgb matrix led, index = %d, led count = %d\n", index, RGB_LED_NUM);
            }
        }
        break;
#endif

#ifdef AMK_APC_ENABLE
    case amk_protocol_get_ms_config:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = amk_get_ms_config();
            ap_debug("AMK Protocol: get ms config = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_ms_config:
        {
            uint8_t old = amk_get_ms_config();
            uint8_t config = msg[2];
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set ms config = %d\n", config);

            if (old != config ) {
                amk_set_ms_config(config);
            }
        }
        break;
    case amk_protocol_get_rt_sens:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = eeconfig_read_rt_sens();
            ap_debug("AMK Protocol: get rt sense = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_rt_sens:
        {
            uint8_t val = msg[2];
            uint8_t cur = eeconfig_read_rt_sens();
            if (cur != val) {
                eeconfig_update_rt_sens(val);
                apc_matrix_init();
            }
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set rt sens = %d\n", val);
        }
        break;
    case amk_protocol_get_top_sens:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = eeconfig_read_top_sens();
            ap_debug("AMK Protocol: get top sense = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_top_sens:
        {
            uint8_t val = msg[2];
            uint8_t cur = eeconfig_read_top_sens();
            if (cur != val) {
                eeconfig_update_top_sens(val);
                apc_matrix_init();
            }
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set top sens = %d\n", val);
        }
        break;
    case amk_protocol_get_btm_sens:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = eeconfig_read_btm_sens();
            ap_debug("AMK Protocol: get btm sense = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_btm_sens:
        {
            uint8_t val = msg[2];
            uint8_t cur = eeconfig_read_btm_sens();
            if (cur != val) {
                eeconfig_update_btm_sens(val);
                apc_matrix_init();
            }
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set btm sens = %d\n", val);
        }
        break;
    case amk_protocol_get_apc_sens:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = eeconfig_read_apc_sens();
            ap_debug("AMK Protocol: get apc sense = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_apc_sens:
        {
            uint8_t val = msg[2];
            uint8_t cur = eeconfig_read_apc_sens();
            if (cur != val) {
                eeconfig_update_apc_sens(val);
                apc_matrix_init();
            }
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set apc sens = %d\n", val);
        }
        break;
    case amk_protocol_get_noise_sens:
        {
            msg[2] = amk_protocol_ok;
            msg[3] = eeconfig_read_noise_sens();
            ap_debug("AMK Protocol: get noise sense = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_noise_sens:
        {
            uint8_t val = msg[2];
            uint8_t cur = eeconfig_read_noise_sens();
            if (cur != val) {
                eeconfig_update_noise_sens(val);
                apc_matrix_init();
            }
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set noise sens = %d\n", val);
        }
        break;
#endif
#ifdef MSC_ENABLE
    case amk_protocol_get_file_system_info:
        {
            msg[2] = amk_protocol_ok;
            struct amk_anim_file_system_info info;
            amk_anim_file_system_info(&info);
            msg[3] = info.total_file;

            msg[4] = info.free_space & 0xFF;
            msg[5] = (info.free_space>>8) & 0xFF;
            msg[6] = (info.free_space>>16) & 0xFF;
            msg[7] = (info.free_space>>24) & 0xFF;

            msg[8] = info.total_space& 0xFF;
            msg[9] = (info.total_space>>8) & 0xFF;
            msg[10] = (info.total_space>>16) & 0xFF;
            msg[11] = (info.total_space>>24) & 0xFF;
            ap_debug("AMK Protocol: get anim file total = %d, free space=%d, total space=%d\n", msg[3], info.free_space, info.total_space);
        }
        break;
    case amk_protocol_get_file_info:
        {
            uint8_t index = msg[2];
            struct amk_anim_file_info info;
            msg[2] = amk_protocol_fail;
            if (amk_anim_file_get_info(index, &info)) {
                memcpy(&msg[3], info.name, ANIM_FILE_NAME_MAX);
                msg[3+ANIM_FILE_NAME_MAX] = info.size & 0xFF;
                msg[4+ANIM_FILE_NAME_MAX] = (info.size>>8) & 0xFF;
                msg[5+ANIM_FILE_NAME_MAX] = (info.size>>16) & 0xFF;
                msg[6+ANIM_FILE_NAME_MAX] = (info.size>>24) & 0xFF;
                ap_debug("AMK Protocol: get anim file info: name=%s\n", &msg[3]);
                msg[2] = amk_protocol_ok;
            }
        }
        break;
    case amk_protocol_open_file:
        {
            uint8_t index = msg[2];
            uint8_t read = msg[3];
            msg[2] = amk_protocol_fail;

            struct amk_anim_file_info info;
            if (read) {
                if (amk_anim_file_get_info(index, &info)) {
                    if (amk_anim_file_open(&info, read)) {
                        msg[2] = amk_protocol_ok;
                        msg[3] = index;
                    }
                }
            } else {
                memcpy(info.name, &msg[4], ANIM_FILE_NAME_MAX);
                if (amk_anim_file_open(&info, read)) {
                    msg[2] = amk_protocol_ok;
                    msg[3] = info.index;
                }
            }
        }
        break;
    case amk_protocol_write_file:
        {
            uint8_t index = msg[2];
            uint8_t size = msg[3];
            uint32_t offset = msg[4] | (msg[5]<<8) | (msg[6]<<16) | (msg[7]<<24);
            msg[2] = amk_protocol_fail;
            struct amk_anim_file_info info;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_write(&info, &msg[8], offset, size)) {
                    msg[2] = amk_protocol_ok;
                }
            }
        }
        break;
    case amk_protocol_read_file:
        {
            uint8_t index = msg[2];
            uint8_t size = msg[3];
            uint32_t offset = msg[4] | (msg[5]<<8) | (msg[6]<<16) | (msg[7]<<24);
            msg[2] = amk_protocol_fail;
            struct amk_anim_file_info info;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_read(&info, &msg[8], offset, size)) {
                    msg[2] = amk_protocol_ok;
                }
            }
        }
        break;
        case amk_protocol_close_file:
        {
            uint8_t index = msg[2];
            struct amk_anim_file_info info;
            msg[2] = amk_protocol_fail;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_close(&info)) {
                    msg[2] = amk_protocol_ok;
                }
            }
        }
        break;
    case amk_protocol_delete_file:
        {
            uint8_t index = msg[2];
            msg[2] = amk_protocol_fail;
            struct amk_anim_file_info info;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_delete(&info)) {
                    msg[2] = amk_protocol_ok;
                }
            }
        }
        break;
    case amk_protocol_display_control:
        {
            extern void amk_animation_display_start(void);
            extern void amk_animation_display_stop(void);
            uint8_t action = msg[2];
            if (action) {
                amk_animation_display_start();
                ap_debug("AMK Protocol: start display\n");
            } else {
                amk_animation_display_stop();
                ap_debug("AMK Protocol: stop display\n");
            }
            msg[2] = amk_protocol_ok;
        }
        break;
#endif
    default:
        {
            msg[2] = amk_protocol_fail;
            ap_debug("AMK Protocol: unknown command=%d\n", msg[2]);
        }
        break;
    }
}

void raw_hid_receive_kb(uint8_t *data, uint8_t length)
{
    if (data[0] == amk_protocol_prefix) {
        amk_protocol_process(data, length);
    } else {
#ifdef SIGNALRGB_ENABLE
    extern void signalrgb_process(uint8_t *data, uint8_t length);
    signalrgb_process(data, length);
#endif
    }
}
