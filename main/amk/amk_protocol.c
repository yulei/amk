/**
 * @file amk_protocol.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 *
*/

#include "amk_protocol.h"
#include "amk_store.h"
#include "amk_printf.h"
#ifdef AMK_APC_ENABLE
#include "amk_apc.h"
#endif
#ifdef AMK_DKS_ENABLE
#include "amk_dks.h"
#endif
#include "timer.h"

#include "keycode_config.h"

#define AMK_PROTOCOL_DEBUG 1

#if AMK_PROTOCOL_DEBUG
#define ap_debug  amk_printf
#else
#define ap_debug(...)
#endif

#if defined(RGB_LINEAR_ENABLE) || defined(RGB_INDICATOR_ENABLE)
#include "rgb_led.h"
#endif

#if defined(RGB_INDICATOR_ENABLE)
#include "rgb_indicator.h"
#endif

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
        // msg[2]: row, msg[4]: column
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                uint16_t apc = amk_store_get_apc(row, col);
                msg[3]  = apc >> 8;
                msg[4]  = apc & 0xFF;
                ap_debug("AMK Protocol: get apc:%d\n", apc);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: get apc failed, row=%d, col=%d\n", row, col);
            }
        }
        break;
    case amk_protocol_set_apc:
        // msg[2]: row, msg[3]: column, msg[4]: keycode high, msg[5]: keycode low
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                uint16_t apc = (msg[4] << 8) | msg[5];
                amk_store_set_apc(row, col, apc);
                apc_matrix_update_interval_at(row, col);
                ap_debug("AMK Protocol: set apc: row=%d, col=%d, apc=%d\n", row, col, apc);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: set apc failed\n");
            }
        }
        break;
    case amk_protocol_get_rt:
        // msg[2]: row, msg[3]: column
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                uint16_t rt = amk_store_get_rt(row, col);
                msg[3]  = rt >> 8;
                msg[4]  = rt & 0xFF;
                ap_debug("AMK Protocol: get rt at [%d,%d]:%d\n", row, col, rt);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: get rt failed\n");
            }
        }
        break;
    case amk_protocol_set_rt:
        // msg[2]: row, msg[3]: column, msg[4]: keycode high, msg[5]: keycode low
        {
            uint8_t row = msg[2];
            uint8_t col = msg[3];
            if ((row < MATRIX_ROWS) && (col < MATRIX_COLS)) {
                msg[2] = amk_protocol_ok;
                uint16_t rt = (msg[4] << 8) | msg[5];
                amk_store_set_rt(row, col, rt);
                apc_matrix_update_interval_at(row, col);
                ap_debug("AMK Protocol: set rt: row=%d, col=%d, apc=%d\n", row, col, rt);
            } else {
                msg[2] = amk_protocol_fail;
                ap_debug("AMK Protocol: set rt failed\n");
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
#ifdef AMK_APC_ENABLE
    case amk_protocol_get_pole:
        {
            extern uint8_t amk_magnetive_pole;
            msg[2] = amk_protocol_ok;
            msg[3] = amk_magnetive_pole;
            ap_debug("AMK Protocol: get magnetive pole = %d\n", msg[3]);
        }
        break;
    case amk_protocol_set_pole:
        {
            extern uint8_t amk_magnetive_pole;
            uint8_t old = amk_magnetive_pole;
            amk_magnetive_pole = msg[2];
            msg[2] = amk_protocol_ok;
            ap_debug("AMK Protocol: set magnetive pole = %d\n", amk_magnetive_pole);

            if (old != amk_magnetive_pole) {
                eeconfig_update_pole(amk_magnetive_pole);
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
