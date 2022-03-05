/**
 *  @file udd_desc.h
 */ 

#pragma once

#include <stdint.h>
#include <stddef.h>

#define USB_MSC_BIT         (1u << 1)
#define USB_WEBUSB_BIT      (1u << 2)
#define USB_OUTPUT_RF       (1u << 3)
#define USB_AUDIO_BIT       (1u << 4)

extern uint32_t usb_setting;

#define NKRO_KEYCODE_SIZE   32

#ifdef DYNAMIC_CONFIGURATION
// Interface number
enum {
    ITF_NUM_HID_KBD,
    ITF_NUM_HID_OTHER,
    ITF_NUM_DUMMY,
    #define ITF_NUM_VIAL            ITF_NUM_DUMMY
    #define ITF_NUM_VENDOR          ITF_NUM_DUMMY
    #define ITF_NUM_MSC             ITF_NUM_DUMMY
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
#ifdef AUDIO_ENABLE
    ITF_NUM_AUDIO_CONTROL,
    ITF_NUM_AUDIO_STREAMING,
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
#ifdef AUDIO_ENABLE
    EPNUM_AUDIO_OUT       = 0x03,
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

uint8_t const* udd_descriptor_device(void);
uint32_t udd_descriptor_device_size(void);

uint8_t const* udd_descriptor_configuration(uint8_t index);
uint32_t udd_descriptor_configuration_size(uint8_t index);

uint8_t const* udd_hid_descriptor_report(uint8_t itf);
uint32_t udd_hid_descriptor_report_size(uint8_t itf);

uint8_t const* udd_descriptor_hid_report_kbd(void);
uint32_t udd_descriptor_hid_report_kbd_size(void);

uint8_t const* udd_descriptor_hid_report_other(void);
uint32_t udd_descriptor_hid_report_other_size(void);

uint8_t const* udd_hid_descriptor_interface(uint8_t itf);
uint32_t udd_hid_descriptor_interface_size(uint8_t itf);

uint8_t const* udd_descriptor_hid_interface_kbd(void);
uint32_t udd_descriptor_hid_interface_kbd_size(void);

uint8_t const* udd_descriptor_hid_interface_other(void);
uint32_t udd_descriptor_hid_interface_other_size(void);

#ifdef VIAL_ENABLE
uint8_t const* udd_descriptor_hid_report_vial(void);
uint32_t udd_descriptor_hid_report_vial_size(void);

uint8_t const* udd_descriptor_hid_interface_vial(void);
uint32_t udd_descriptor_hid_interface_vial_size(void);
#endif

#ifdef WEBUSB_ENABLE
enum {
    WEBUSB_KEYMAP_SET = 1,
    WEBUSB_KEYMAP_GET,
};
uint8_t const* udd_descriptor_bos(void);
uint32_t udd_descriptor_bos_size(void);

uint8_t const* udd_descriptor_url(void);
uint32_t udd_descriptor_url_size(void);

uint8_t const* udd_descriptor_msos20(void);
uint32_t udd_descriptor_msos20_size(void);
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

uint16_t const* udd_descriptor_string(uint8_t index, uint16_t langid);

uint8_t *get_descriptor_str(uint8_t index, uint16_t *length);
