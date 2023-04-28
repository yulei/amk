/**
 * @file amk_usb.h
 *  usb driver interface definitions
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum 
{
    USB_STATE_IDLE,
    USB_STATE_MOUNTED,
    USB_STATE_UNMOUNTED,
    USB_STATE_SUSPENDED,
    USB_STATE_RESUMED,
} usb_state_t;

bool amk_usb_itf_ready(uint32_t type);
bool amk_usb_itf_send_report(uint32_t type, const void* data, uint32_t size);
void amk_usb_init(void);
void amk_usb_task(void);
bool amk_usb_ready(void);
bool amk_usb_suspended(void);
void amk_usb_remote_wakeup(void);
void amk_usb_send_report(uint8_t report_type, const void* data, size_t size);
void amk_usb_connect(bool on);