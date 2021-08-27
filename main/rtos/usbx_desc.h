/**
 * @file usbx_desc.h
 */

#pragma once

#include "tx_api.h"
#include "usb_descriptors.h"

UCHAR* usbx_desc_device_fullspeed(void);
ULONG usbx_desc_device_fullspeed_size(void);

UCHAR* usbx_desc_string(void);
ULONG usbx_desc_string_size(void);

UCHAR* usbx_desc_language_id(void);
ULONG usbx_desc_language_id_size(void);

UCHAR* usbx_desc_hid_keyboard(void);
ULONG usbx_desc_hid_keyboard_size(void);

UCHAR* usbx_desc_hid_other(void);
ULONG usbx_desc_hid_other_size(void);