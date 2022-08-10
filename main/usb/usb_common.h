/**
 * @file usb_common.h
 * 
 */

#pragma once

#define USB_MSC_BIT         (1u << 1)
//#define USB_WEBUSB_BIT      (1u << 2)
#define USB_OUTPUT_RF       (1u << 3)
#define USB_AUDIO_BIT       (1u << 4)
#define USB_VIAL_BIT        (1u << 5)
#define USB_SWITCH_BIT      (1u << 6)

extern uint32_t usb_setting;

// HID ep size was 32
// id for 1
// mods for 1
// keys for 30
#define AMK_NKRO_TOTAL_SIZE  32

#ifdef DYNAMIC_CONFIGURATION
// Interface number
enum {
    ITF_NUM_HID_KBD,
#ifdef HID_OTHER_ENABLE
    ITF_NUM_HID_OTHER,
#endif
#if defined(VIAL_ENABLE) || defined(MSC_ENABLE) || defined(AUDIO_ENABLE)
    ITF_NUM_DUMMY,
    #define ITF_NUM_VIAL            ITF_NUM_DUMMY
    #define ITF_NUM_MSC             ITF_NUM_DUMMY
    #define ITF_NUM_AUDIO           ITF_NUM_DUMMY  
#endif
#ifdef AUDIO_ENABLE
    ITF_NUM_AUDIO_STREAMING,
#endif
    ITF_NUM_TOTAL
};

// Endpoint number
enum {
    EPNUM_HID_KBD       = 0x01,
#ifdef HID_OTHER_ENABLE
    EPNUM_HID_OTHER,
#endif
#if defined(VIAL_ENABLE) || defined(MSC_ENABLE) || defined(AUDIO_ENABLE)
    EPNUM_DUMMY,
    #define EPNUM_VIAL_OUT      EPNUM_DUMMY
    #define EPNUM_VIAL_IN       EPNUM_DUMMY
    #define EPNUM_MSC_OUT       EPNUM_DUMMY
    #define EPNUM_MSC_IN        EPNUM_DUMMY 
    #define EPNUM_AUDIO_OUT     EPNUM_DUMMY
    #define EPNUM_AUDIO_IN      EPNUM_DUMMY
#endif
    EPNUM_MAX
};
#else
// Interface number
enum {
    ITF_NUM_HID_KBD,
#ifdef HID_OTHER_ENABLE
    ITF_NUM_HID_OTHER,
#endif
#ifdef CDC_ENABLE
    ITF_NUM_CDC,
    ITF_NUM_CDC_DATA,
#endif
#ifdef VIAL_ENABLE
    ITF_NUM_VIAL,
#endif
#ifdef MSC_ENABLE
    ITF_NUM_MSC,
#endif
#ifdef AUDIO_ENABLE
    ITF_NUM_AUDIO,
    ITF_NUM_AUDIO_STREAMING,
#endif
    ITF_NUM_TOTAL
};

enum {
    EPNUM_HID_KBD       = 0x01,
#ifdef HID_OTHER_ENABLE
    EPNUM_HID_OTHER     = 0x02,
#endif
#ifdef CDC_ENABLE
    EPNUM_CDC_NOTIF     = 0x02,
    EPNUM_CDC_DATAOUT   = 0x03,
    #define EPNUM_CDC_DATAIN    EPNUM_CDC_DATAOUT
#endif
#ifdef VIAL_ENABLE
    EPNUM_VIAL_OUT      = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA) || defined(STM32L072xx)
    EPNUM_VIAL_IN       = 0x04,
    #else
        #define EPNUM_VIAL_IN EPNUM_VIAL_OUT
    #endif
#endif
#ifdef MSC_ENABLE
    EPNUM_MSC_OUT       = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA) || defined(STM32L072xx)
    EPNUM_MSC_IN        = 0x04,
    #else
        #define EPNUM_MSC_IN    EPNUM_MSC_OUT
    #endif
#endif
#ifdef AUDIO_ENABLE
    EPNUM_AUDIO_OUT     = 0x03,
    #define EPNUM_AUDIO_IN  EPNUM_AUDIO_IN
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
    HID_REPORT_ID_VIAL,
    HID_REPORT_ID_MACRO,
    HID_REPORT_ID_DELAY,
    HID_REPORT_ID_CDC,
    HID_REPORT_ID_UNKNOWN,
};

// String index
enum {
    DESC_STR_LANGID,
    DESC_STR_MANUFACTURE,
    DESC_STR_PRODUCT,
    DESC_STR_SERIAL,
    DESC_STR_CONFIGURATION,
    DESC_STR_INTERFACE_HID_KBD,
    DESC_STR_INTERFACE_HID_OTHER,
};

