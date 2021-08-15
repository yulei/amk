/**
 * @file usbx_desc.c
 *  descriptors for the usbx
 */

#include "usbx_desc.h"

// HID Keyboard Report Descriptor
static UCHAR desc_hid_report_kbd[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD(),
};

// HID other report
static UCHAR desc_hid_report_other[] =
{
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(HID_REPORT_ID_MOUSE) ),
    TUD_HID_REPORT_DESC_EXTRA( HID_REPORT_ID_SYSTEM, HID_REPORT_ID_CONSUMER ),
};

#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN)

static UCHAR desc_device_fullspeed [] = {
    // Device descriptor
    TUD_COMPOSITE_DEVICE_DESCRIPTOR(0x0200, VENDOR_ID, PRODUCT_ID, DEVICE_VER, DESC_STR_MANUFACTURE, DESC_STR_PRODUCT, DESC_STR_SERIAL),

    // Config descriptor
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),

    // Interface descriptor
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
};

static UCHAR desc_string [] = {
    0x09, 0x04, DESC_STR_MANUFACTURE, 0x0A,
    'M', 'a', 't', 'r', 'i', 'x', ' ', 'L', 'a', 'b',
    0x09, 0x04, DESC_STR_PRODUCT, 0x04,
    'V', 'i', 't', 'a',
    0x09, 0x04, DESC_STR_SERIAL, 0x08,
    'a', 'm', 'k', ':', '4', 'd', '5', '8',
};

static UCHAR desc_language_id[] = {
    /* English. */
    0x09, 0x04
};

UCHAR* usbx_desc_device_fullspeed(void)
{
    return desc_device_fullspeed;
}

ULONG usbx_desc_device_fullspeed_size(void)
{
    return sizeof(desc_device_fullspeed);
}

UCHAR* usbx_desc_string(void)
{
    return desc_string;
}

ULONG usbx_desc_string_size(void)
{
    return sizeof(desc_string);
}

UCHAR* usbx_desc_language_id(void)
{
    return desc_language_id;
}

ULONG usbx_desc_language_id_size(void)
{
    return sizeof(desc_language_id);
}

UCHAR* usbx_desc_hid_keyboard(void)
{
    return desc_hid_report_kbd;
}

ULONG usbx_desc_hid_keyboard_size(void)
{
    return sizeof(desc_hid_report_kbd);
}

UCHAR* usbx_desc_hid_other(void)
{
    return desc_hid_report_other;
}

ULONG usbx_desc_hid_other_size(void)
{
    return sizeof(desc_hid_report_other);
}