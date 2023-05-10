/**
 * @file rf_driver.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define CMD_MAX_LEN         64
#define RING_BUF_MAX_LEN    128
#define SYNC_BYTE_1         0xAA
#define SYNC_BYTE_2         0x55
#define SYNC_PING           0xA5
#define SYNC_PONG           0x5A

enum
{
    CMD_KEY_REPORT,
    CMD_MOUSE_REPORT,
    CMD_SYSTEM_REPORT,
    CMD_CONSUMER_REPORT,
    CMD_RESET_TO_BOOTLOADER,
    CMD_SET_LEDS,
    CMD_KEYMAP_SET,
    CMD_KEYMAP_SET_ACK,
    CMD_KEYMAP_GET,
    CMD_KEYMAP_GET_ACK,
    CMD_TOGGLE_SCREEN,
    CMD_TOGGLE_MSC,
    CMD_TOGGLE_DATETIME,
    CMD_ERASE_BOND,
    CMD_SELECT_PEER,
    CMD_VIAL_REQUEST,
    CMD_VIAL_RESPONSE,
    CMD_NKRO_REPORT,
};

void rf_driver_init(bool use_rf);

void rf_driver_task(void);

void rf_driver_toggle(void);
void rf_driver_erase_bond(void);
void rf_driver_select_peer(int peer);

void rf_driver_put_report(uint32_t type, void* data, uint32_t size);