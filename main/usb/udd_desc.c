/** 
 * @file udd_desc.c 
 */

#include <string.h>

#include "usb_desc_def.h"
#include "udd_desc.h"

#ifdef WEBUSB_ENABLE
#define USB_BCD 0x0210
#else
#define USB_BCD 0x0200
#endif

// Device Descriptors
static uint8_t desc_device_fullspeed [] = {
    // Device descriptor
    UDD_DEVICE_DESCRIPTOR(USB_BCD, UDD_EP0_SIZE, VENDOR_ID, PRODUCT_ID, DEVICE_VER, DESC_STR_MANUFACTURE, DESC_STR_PRODUCT, DESC_STR_SERIAL),
};

static uint8_t desc_device_fullspeed_dyn[] = {
    // Device descriptor
    UDD_DEVICE_DESCRIPTOR(USB_BCD, UDD_EP0_SIZE, VENDOR_ID, (uint16_t)(~PRODUCT_ID), DEVICE_VER, DESC_STR_MANUFACTURE, DESC_STR_PRODUCT, DESC_STR_SERIAL),
};

// DEVICE DESCRIPTOR
uint8_t const* udd_descriptor_device(void)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return desc_device_fullspeed_dyn;
    }
#endif
    return desc_device_fullspeed;
}

uint32_t udd_descriptor_device_size(void)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return sizeof(desc_device_fullspeed_dyn);
    }
#endif
    return sizeof(desc_device_fullspeed);
}

// HID Keyboard Report Descriptor
static uint8_t desc_hid_report_kbd[] =
{
    UDD_HID_REPORT_DESC_KEYBOARD(),
};

// HID other report
static uint8_t desc_hid_report_other[] =
{
    UDD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID_MOUSE),
    UDD_HID_REPORT_DESC_EXTRA(HID_REPORT_ID_SYSTEM, HID_REPORT_ID_CONSUMER),
    //UDD_HID_REPORT_DESC_NKRO(HID_REPORT_ID_NKRO)
};

// HID KEYBOARD REPORT DESCRIPTOR
uint8_t const* udd_descriptor_hid_report_kbd(void)
{
    return desc_hid_report_kbd;
}

uint32_t udd_descriptor_hid_report_kbd_size(void)
{
    return sizeof(desc_hid_report_kbd);
}

// HID OTHER REPORT DESCRIPTOR
uint8_t const* udd_descriptor_hid_report_other(void)
{
    return desc_hid_report_other;
}

uint32_t udd_descriptor_hid_report_other_size(void)
{
    return sizeof(desc_hid_report_other);
}

#ifdef VIAL_ENABLE
static uint8_t desc_hid_report_vial[] =
{
    UDD_HID_REPORT_DESC_VIAL(RAW_USAGE_PAGE, RAW_USAGE_ID, VIAL_EPSIZE),
};

// HID VIAL REPORT DESCRIPTOR
uint8_t const* udd_descriptor_hid_report_vial(void)
{
    return desc_hid_report_vial;
}

uint32_t udd_descriptor_hid_report_vial_size(void)
{
    return sizeof(desc_hid_report_vial);
}
#endif

// Configuration Descriptor
#ifdef WEBUSB_ENABLE
#define WEBUSB_DESC_LEN UDD_VENDOR_DESC_LEN
#else 
#define WEBUSB_DESC_LEN 0 
#endif

#define  CONFIG_TOTAL_LEN  (UDD_CONFIG_DESC_LEN + UDD_HID_DESC_LEN + UDD_HID_DESC_LEN + UDD_VIAL_DESC_LEN + UDD_AUDIO_DESC_LEN)

static uint8_t desc_config_fullspeed [] = {
    // Config descriptor
    UDD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, UDD_ATT_REMOTE_WAKEUP, 500),

    // Interface descriptor
    UDD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, 1, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, UDD_HID_EP_SIZE, UDD_HID_INTERVAL),
    UDD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, 0, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, UDD_HID_EP_SIZE, UDD_HID_INTERVAL),

#ifdef VIAL_ENABLE
    UDD_HID_GENERIC_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, UDD_HID_INTERVAL),
#endif

#ifdef AUDIO_ENABLE
    UDD_AUDIO_INTERFACE_DESCRIPTOR(ITF_NUM_AUDIO_CONTROL, 0, 0, UDD_AUDIO_SUBCLASS_CONTROL),
    UDD_AUDIO_CLASS_DESCRIPTOR_HEADER(ITF_NUM_AUDIO_CONTROL, ITF_NUM_AUDIO_STREAMING, UDD_AUDIO_CLASS_DESC_LEN),
    UDD_AUDIO_INPUT_TERMINAL_DESCRIPTOR(),
    UDD_AUDIO_FEATURE_UNIT_DESCRIPTOR(),
    UDD_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR(),
    UDD_AUDIO_INTERFACE_DESCRIPTOR(ITF_NUM_AUDIO_STREAMING, 0, 0, UDD_AUDIO_SUBCLASS_STREAMING),
    UDD_AUDIO_INTERFACE_DESCRIPTOR(ITF_NUM_AUDIO_STREAMING, 1, 1, UDD_AUDIO_SUBCLASS_STREAMING),
    UDD_AUDIO_STREAMING_DESCRIPTOR(),
    UDD_AUDIO_FORMAT_TYPE_DESCRIPTOR(1, UDD_AUDIO_CHANNELS, UDD_AUDIO_SAMPLE_FRAME, UDD_AUDIO_SAMPLE_BITS, UDD_AUDIO_FREQ),
    UDD_AUDIO_ENDPOINT_STANDARD_DESCRIPTOR(EPNUM_AUDIO_OUT, UDD_AUDIO_PACKET_SIZE),
    UDD_AUDIO_ENDPOINT_STREAMING_DESCRIPTOR(0, 0, 0)
#endif
};

#define  DYN_CONFIG_TOTAL_LEN  (UDD_CONFIG_DESC_LEN + UDD_HID_DESC_LEN + UDD_HID_DESC_LEN + UDD_MSC_DESC_LEN)

static uint8_t desc_config_fullspeed_dyn[] = {
    // Config descriptor
    UDD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, DYN_CONFIG_TOTAL_LEN, UDD_ATT_REMOTE_WAKEUP, 500),

    // Interface descriptor
    UDD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, 1, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, UDD_HID_EP_SIZE, UDD_HID_INTERVAL),
    UDD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, 0, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, UDD_HID_EP_SIZE, UDD_HID_INTERVAL),
#ifdef MSC_ENABLE
    UDD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, 0x80|EPNUM_MSC_IN, UDD_MSC_EP_SIZE),
#endif
};

// GET CONFIGURATION DESCRIPTOR
uint8_t const* udd_descriptor_configuration(uint8_t index)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return desc_config_fullspeed_dyn;
    }
#endif

    return desc_config_fullspeed;
}

uint32_t udd_descriptor_configuration_size(uint8_t index)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return sizeof(desc_config_fullspeed_dyn);
    }
#endif
    return sizeof(desc_config_fullspeed);
}

// GET HID REPORT DESCRIPTOR
uint8_t const* udd_hid_descriptor_report(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return udd_descriptor_hid_report_kbd();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return udd_descriptor_hid_report_other();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return udd_descriptor_hid_report_vial();
    }
    #endif
    return NULL;
}

uint32_t udd_hid_descriptor_report_size(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return udd_descriptor_hid_report_kbd_size();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return udd_descriptor_hid_report_other_size();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return udd_descriptor_hid_report_vial_size();
    }
    #endif
    return 0;
}

static uint8_t desc_hid_kbd[] = {
    UDD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, 1, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, UDD_HID_EP_SIZE, UDD_HID_INTERVAL),
};


uint8_t const* ud_hid_descriptor_interface(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return udd_descriptor_hid_interface_kbd();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return udd_descriptor_hid_interface_other();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return udd_descriptor_hid_interface_vial();
    }
    #endif
    return NULL;
}

uint32_t udd_hid_descriptor_interface_size(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return udd_descriptor_hid_interface_kbd_size();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return udd_descriptor_hid_interface_other_size();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return udd_descriptor_hid_interface_vial_size();
    }
    #endif
    return 0;
}

// GET HID DESCRIPTOR
uint8_t const* udd_descriptor_hid_interface_kbd(void)
{
    return desc_hid_kbd;
}

uint32_t udd_descriptor_hid_interface_kbd_size(void)
{
    return sizeof(desc_hid_kbd);
}

static uint8_t desc_hid_other[] = {
    UDD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, 0, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, UDD_HID_EP_SIZE, UDD_HID_INTERVAL),
};

// GET HID DESCRIPTOR
uint8_t const* udd_descriptor_hid_interface_other(void)
{
    return desc_hid_other;
}

uint32_t udd_descriptor_hid_interface_other_size(void)
{
    return sizeof(desc_hid_other);
}

#ifdef VIAL_ENABLE
static uint8_t desc_hid_vial[] = {
    UDD_HID_GENERIC_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, UDD_HID_INTERVAL),
};

uint8_t const* udd_descriptor_hid_interface_vial(void)
{
    return desc_hid_vial;
}

uint32_t udd_descriptor_hid_interface_vial_size(void)
{
    return sizeof(desc_hid_vial);
}

#endif

// String Descriptors
char const* string_desc_arr [] =
{
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    UDD_XSTR(MANUFACTURER),       // 1: Manufacturer
    UDD_XSTR(PRODUCT),            // 2: Product
#ifdef VIAL_ENABLE
    "vial:f64c2b3c",                // 3: Serials, should use chip ID
#else
    "amk:4d58",                     // 3: Serials, should use chip ID
#endif
    "Configuration",                // 4: Device configuration 
    "HID Keyboard",                 // 5: Hid keyboard
    "HID Extra",                    // 6: Hid extra key
};

static uint16_t _desc_str[32];

uint16_t const* udd_descriptor_string(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint16_t length = 0;
    return (uint16_t *)get_descriptor_str(index, &length);
}

// STRING DESCRIPTOR
uint8_t *get_descriptor_str(uint8_t index, uint16_t *length)
{
    uint8_t chr_count;

    if ( index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } /*else if (index == DESC_STR_SERIAL) {
        return (uint8_t*)get_device_serial(length);
    }*/ else {
        // Convert ASCII string into UTF-16

        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if ( chr_count > 31 ) chr_count = 31;

        for(uint8_t i=0; i<chr_count; i++) {
          _desc_str[1+i] = str[i];
        }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (UDD_DESC_STRING << 8 ) | (2*chr_count + 2);
  *length = _desc_str[0] & 0xFF;

  return (uint8_t*)_desc_str;
}

#if 0
static uint8_t _desc_serial[64];

static void to_unicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
    uint8_t idx = 0;

    for (idx = 0; idx < len; idx++) {
        if (((value >> 28)) < 0xA) {
            pbuf[2 * idx] = (value >> 28) + '0';
        } else {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }
        value = value << 4;
        pbuf[2 * idx + 1] = 0;
    }
}

#if defined(NRF52840_XXAA) || defined(NRF52)
#include "nrf.h"
#else
#include "generic_hal.h"
#endif
uint16_t* get_device_serial(uint16_t* length)
{
  uint32_t d0, d1;

#if defined(NRF52840_XXAA) || defined(NRF52)
    d0 = NRF_FICR->DEVICEADDR[0];
    d1 = NRF_FICR->DEVICEADDR[1] | 0xC000;
#else
    d0  = *(uint32_t *)(UID_BASE) + *(uint32_t *)(UID_BASE + 0x8);
    d1  = *(uint32_t *)(UID_BASE + 0x4);
#endif

    memset(&_desc_serial[0], 0, sizeof(_desc_serial));
    to_unicode(d0, &_desc_serial[2], 8);
    to_unicode(d1, &_desc_serial[18], 4);
    _desc_serial[0] = TUSB_DESC_STRING;
    _desc_serial[1] = 2 * 16 + 2;
    *length = _desc_serial[1];
    return (uint16_t *)_desc_serial;
}
#endif