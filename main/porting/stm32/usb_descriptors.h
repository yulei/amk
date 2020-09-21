/**
 *  usb_descriptors.h
 */ 

#pragma once

#include "tusb_def.h"

enum {
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE,
    REPORT_ID_SYSTEM,
    REPORT_ID_CONSUMER,
};

enum {
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

extern uint8_t const desc_ms_os_20[];

uint8_t* tud_descriptor_device_cb(void);
uint32_t tud_descriptor_device_size(void);

uint8_t* tud_descriptor_hid_report_cb(void);
uint32_t tud_descriptor_hid_report_size(void);

uint8_t* tud_descriptor_configuration_cb(void);
uint32_t tud_descriptor_configuration_size(void);

uint8_t* tud_descriptor_hid_cb(void);
uint32_t tud_descriptor_hid_size(void);

uint8_t* tud_descriptor_bos_cb(void);
uint32_t tud_descriptor_bos_size(void);

uint32_t tud_descriptor_bos_size(void);

enum {
    DESC_STR_LANGID,
    DESC_STR_MANUFACTURE,
    DESC_STR_PRODUCT,
    DESC_STR_SERIAL,
    DESC_STR_CONFIG_HID,
    DESC_STR_INTERFACE_HID,
    DESC_STR_INTERFACE_WEBUSB,
};

uint8_t* tud_descriptor_string_cb(uint8_t index, uint16_t* length);