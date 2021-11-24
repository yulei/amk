/**
 *  usb_descriptors.h
 */ 

#pragma once

//#include "tusb.h"
#include <stdint.h>
#include <stddef.h>

#define USB_MSC_BIT         (1u << 1)
#define USB_WEBUSB_BIT      (1u << 2)
#define OUTPUT_RF          (1u << 3)

extern uint32_t usb_setting;

#define NKRO_KEYCODE_SIZE   32

#ifdef DYNAMIC_CONFIGURATION
// Interface number
enum {
    ITF_NUM_HID_KBD,
    ITF_NUM_HID_OTHER,
    ITF_NUM_DUMMY,
    #define ITF_NUM_VIAL ITF_NUM_DUMMY
    #define ITF_NUM_VENDOR ITF_NUM_DUMMY
    #define ITF_NUM_MSC ITF_NUM_DUMMY
    ITF_NUM_TOTAL
};

// Endpoint number
enum {
    EPNUM_HID_KBD       = 0x01,
    EPNUM_HID_OTHER,
    EPNUM_DUMMY,
    #define EPNUM_VIAL_OUT      EPNUM_DUMMY
    #define EPNUM_VIAL_IN       EPNUM_DUMMY
    #define EPNUM_VENDOR_OUT    EPNUM_DUMMY
    #define EPNUM_VENDOR_IN     EPNUM_DUMMY
    #define EPNUM_MSC_OUT       EPNUM_DUMMY
    #define EPNUM_MSC_IN        EPNUM_DUMMY
    //EPNUM_MSC_IN,
    EPNUM_MAX
};
#else
// Interface number
enum {
    ITF_NUM_HID_KBD,
    ITF_NUM_HID_OTHER,
#ifdef VIAL_ENABLE
    ITF_NUM_VIAL,
#endif
#ifdef WEBUSB_ENABLE
    ITF_NUM_VENDOR,
#endif
#ifdef MSC_ENABLE
    ITF_NUM_MSC,
#endif
    ITF_NUM_TOTAL
};

enum {
    EPNUM_HID_KBD       = 0x01,
    EPNUM_HID_OTHER     = 0x02,
#ifdef VIAL_ENABLE
    EPNUM_VIAL_OUT      = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_VIAL_IN       = 0x04,
    #else
        #define EPNUM_VIAL_IN EPNUM_VIAL_OUT
    #endif
#endif
#ifdef WEBUSB_ENABLE
    EPNUM_VENDOR_OUT    = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_VENDOR_IN     = 0x04,
    #else
        #define EPNUM_VENDOR_IN EPNUM_VENDOR_OUT
    #endif
#endif
#ifdef MSC_ENABLE
    EPNUM_MSC_OUT       = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_MSC_IN        = 0x04,
    #else
        #define EPNUM_MSC_IN    EPNUM_MSC_OUT
    #endif
#endif
    EPNUM_MAX
};

#endif// DYNAMIC_CONFIGURATION
// Report id
enum {
    HID_REPORT_ID_KEYBOARD = 0,
    HID_REPORT_ID_MOUSE,
    HID_REPORT_ID_SYSTEM,
    HID_REPORT_ID_CONSUMER,
    HID_REPORT_ID_NKRO,
    HID_REPORT_ID_WEBUSB,
    HID_REPORT_ID_VIAL,
    HID_REPORT_ID_MACRO,
    HID_REPORT_ID_UNKNOWN,
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
uint32_t tud_hid_descriptor_report_size(uint8_t itf);

uint8_t const* tud_descriptor_hid_report_kbd_cb(void);
uint32_t tud_descriptor_hid_report_kbd_size(void);

uint8_t const* tud_descriptor_hid_report_other_cb(void);
uint32_t tud_descriptor_hid_report_other_size(void);

uint8_t const* tud_hid_descriptor_interface_cb(uint8_t itf);
uint32_t tud_hid_descriptor_interface_size(uint8_t itf);

uint8_t const* tud_descriptor_hid_interface_kbd_cb(void);
uint32_t tud_descriptor_hid_interface_kbd_size(void);

uint8_t const* tud_descriptor_hid_interface_other_cb(void);
uint32_t tud_descriptor_hid_interface_other_size(void);

#ifdef VIAL_ENABLE
uint8_t const* tud_descriptor_hid_report_vial_cb(void);
uint32_t tud_descriptor_hid_report_vial_size(void);

uint8_t const* tud_descriptor_hid_interface_vial_cb(void);
uint32_t tud_descriptor_hid_interface_vial_size(void);
#endif

#ifdef WEBUSB_ENABLE
enum {
    WEBUSB_KEYMAP_SET = 1,
    WEBUSB_KEYMAP_GET,
};
uint8_t const* tud_descriptor_bos_cb(void);
uint32_t tud_descriptor_bos_size(void);

uint8_t const* tud_descriptor_url_cb(void);
uint32_t tud_descriptor_url_size(void);

uint8_t const* tud_descriptor_msos20_cb(void);
uint32_t tud_descriptor_msos20_size(void);
#endif

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