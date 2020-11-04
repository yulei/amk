/**
 *  usb_descriptors.h
 */ 

#pragma once

#include "tusb.h"

// Extra key report
#define TUD_HID_REPORT_DESC_EXTRA(system, consumer) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )         ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_SYSTEM_CONTROL )         ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION       )         ,\
    HID_REPORT_ITEM( system, 8, RI_TYPE_GLOBAL, 1     )         ,\
    HID_LOGICAL_MIN_N( 0x0001, 2                           )    ,\
    HID_LOGICAL_MAX_N( 0x0037, 2                           )    ,\
    /* System Power Down */ \
    HID_USAGE_MIN_N  ( 0x0081, 2                           )    ,\
    /* System Display LCD Autoscale */ \
    HID_USAGE_MAX_N  ( 0x00B7, 2                           )    ,\
    HID_REPORT_COUNT ( 1                                   )    ,\
    HID_REPORT_SIZE  ( 16                                  )    ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )    ,\
    HID_COLLECTION_END, \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER    )               ,\
    HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL )               ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION )               ,\
    HID_REPORT_ITEM( consumer, 8, RI_TYPE_GLOBAL, 1        )    ,\
    HID_LOGICAL_MIN_N( 0x0001, 2                           )    ,\
    HID_LOGICAL_MAX_N( 0x029C, 2                           )    ,\
    HID_USAGE_MIN_N  ( 0x0001, 2                           )    ,\
    HID_USAGE_MAX_N  ( 0x029C, 2                           )    ,\
    HID_REPORT_COUNT ( 1                                   )    ,\
    HID_REPORT_SIZE  ( 16                                  )    ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )    ,\
    HID_COLLECTION_END \

// Interface number
enum {
    ITF_NUM_HID_KBD,
    ITF_NUM_HID_OTHER,
    ITF_NUM_VENDOR,
    ITF_NUM_TOTAL
};

// Endpoint number
#define EPNUM_HID_KBD       0x01
#define EPNUM_HID_OTHER     0x02
#define EPNUM_VENDOR_OUT    0x03
#if defined(STM32F103xB)
#define EPNUM_VENDOR_IN     0x04
#else
#define EPNUM_VENDOR_IN     0x03
#endif

// Report id
enum {
    HID_REPORT_ID_KEYBOARD,
    HID_REPORT_ID_MOUSE,
    HID_REPORT_ID_SYSTEM,
    HID_REPORT_ID_CONSUMER,
    HID_REPORT_ID_WEBUSB,
};

// Vendor request id
enum {
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

uint8_t const* tud_descriptor_device_cb(void);
uint32_t tud_descriptor_device_size(void);

uint8_t const* tud_descriptor_configuration_cb(uint8_t index);
uint32_t tud_descriptor_configuration_size(uint8_t index);

uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf);

uint8_t const* tud_descriptor_hid_report_kbd_cb(void);
uint32_t tud_descriptor_hid_report_kbd_size(void);

uint8_t const* tud_descriptor_hid_report_other_cb(void);
uint32_t tud_descriptor_hid_report_other_size(void);

uint8_t const* tud_descriptor_hid_interface_kbd_cb(void);
uint32_t tud_descriptor_hid_interface_kbd_size(void);

uint8_t const* tud_descriptor_hid_interface_other_cb(void);
uint32_t tud_descriptor_hid_interface_other_size(void);

uint8_t const* tud_descriptor_bos_cb(void);
uint32_t tud_descriptor_bos_size(void);

uint8_t const* tud_descriptor_url_cb(void);
uint32_t tud_descriptor_url_size(void);

uint8_t const* tud_descriptor_msos20_cb(void);
uint32_t tud_descriptor_msos20_size(void);

// String index
enum {
    DESC_STR_LANGID,
    DESC_STR_MANUFACTURE,
    DESC_STR_PRODUCT,
    DESC_STR_SERIAL,
    DESC_STR_CONFIGURATION,
    DESC_STR_INTERFACE_HID_KBD,
    DESC_STR_INTERFACE_HID_OTHER,
    DESC_STR_INTERFACE_WEBUSB,
};

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid);

uint8_t *get_descriptor_str(uint8_t index, uint16_t *length);