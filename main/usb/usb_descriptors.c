/** 
 * @file usb_descriptors.c 
 */

#include "usb_descriptors.h"
#include "tusb.h"

#define TUD_COMPOSITE_DEVICE_DESCRIPTOR(usb_ver, vender_id, product_id, device_ver, str_manufacture, str_product, str_serial ) \
    18, TUSB_DESC_DEVICE, U16_TO_U8S_LE(usb_ver), \
    0x00, 0x00, 0x00, CFG_TUD_ENDPOINT0_SIZE, \
    U16_TO_U8S_LE(vender_id), U16_TO_U8S_LE(product_id), U16_TO_U8S_LE(device_ver), \
    str_manufacture, str_product, str_serial, 1

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

#define TUD_HID_REPORT_DESC_NKRO(...) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )         ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD       )         ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION       )         ,\
    /* Report ID if any */ \
    __VA_ARGS__ \
    /* 8 bits Modifier Keys (Shfit, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                       ,\
        HID_USAGE_MIN    ( 224                                    )  ,\
        HID_USAGE_MAX    ( 231                                    )  ,\
        HID_LOGICAL_MIN  ( 0                                      )  ,\
        HID_LOGICAL_MAX  ( 1                                      )  ,\
        HID_REPORT_COUNT ( 8                                      )  ,\
        HID_REPORT_SIZE  ( 1                                      )  ,\
        HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD ) ,\
        HID_USAGE_MIN    ( 0                                   )  ,\
        HID_USAGE_MAX    ( (NKRO_KEYCODE_SIZE-1)*8             )  ,\
        HID_LOGICAL_MIN  ( 0                                   )  ,\
        HID_LOGICAL_MAX  ( 1                                   )  ,\
        HID_REPORT_COUNT ( (NKRO_KEYCODE_SIZE-1)*8             )  ,\
        HID_REPORT_SIZE  ( 1                                   )  ,\
        HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )  ,\
    /* 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED ) ,\
        HID_USAGE_MIN    ( 1                                       ) ,\
        HID_USAGE_MAX    ( 5                                       ) ,\
        HID_REPORT_COUNT ( 5                                       ) ,\
        HID_REPORT_SIZE  ( 1                                       ) ,\
        HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
        /* led padding */ \
        HID_REPORT_COUNT ( 1                                       ) ,\
        HID_REPORT_SIZE  ( 3                                       ) ,\
        HID_OUTPUT       ( HID_CONSTANT                            ) ,\
    HID_COLLECTION_END \

#define VIAL_USAGE_PAGE     0xFF60
#define VIAL_USAGE_ID       0x61
#define VIAL_EPSIZE         CFG_TUD_HID_EP_BUFSIZE

#define TUD_HID_REPORT_DESC_VIAL(report_size, ...) \
    HID_USAGE_PAGE_N ( VIAL_USAGE_PAGE, 2                                           ),\
    HID_USAGE        ( VIAL_USAGE_ID                                                ),\
    HID_COLLECTION   ( HID_COLLECTION_APPLICATION                                   ),\
        /* Report ID if any */\
        __VA_ARGS__ \
        /* Input */ \
        HID_USAGE       ( 0x62                                                      ),\
        HID_LOGICAL_MIN ( 0x00                                                      ),\
        HID_LOGICAL_MAX ( 0xff                                                      ),\
        HID_REPORT_SIZE ( 8                                                         ),\
        HID_REPORT_COUNT( report_size                                               ),\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE                    ),\
        /* Output */ \
        HID_USAGE       ( 0x63                                                      ),\
        HID_LOGICAL_MIN ( 0x00                                                      ),\
        HID_LOGICAL_MAX ( 0xff                                                      ),\
        HID_REPORT_SIZE ( 8                                                         ),\
        HID_REPORT_COUNT( report_size                                               ),\
        HID_OUTPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_NON_VOLATILE ),\
    HID_COLLECTION_END \

// Device Descriptors
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
#ifdef WEBUSB_ENABLE
    .bcdUSB             = 0x0210, // at least 2.1 or 3.x for BOS & webUSB
#else
    .bcdUSB             = 0x0200,
#endif
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = VENDOR_ID,
    .idProduct          = PRODUCT_ID,
    .bcdDevice          = DEVICE_VER,

    .iManufacturer      = DESC_STR_MANUFACTURE,
    .iProduct           = DESC_STR_PRODUCT,
    .iSerialNumber      = DESC_STR_SERIAL,

    .bNumConfigurations = 0x01
};

// Device Descriptors
tusb_desc_device_t const desc_device_dyn =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = VENDOR_ID,
    .idProduct          = (uint16_t)(~PRODUCT_ID),
    .bcdDevice          = DEVICE_VER,

    .iManufacturer      = DESC_STR_MANUFACTURE,
    .iProduct           = DESC_STR_PRODUCT,
    .iSerialNumber      = DESC_STR_SERIAL,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
uint8_t const* tud_descriptor_device_cb(void)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return (uint8_t*) &desc_device_dyn;
    }
#endif
    return (uint8_t *) &desc_device;
}

uint32_t tud_descriptor_device_size(void)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return sizeof(desc_device_dyn);
    }
#endif
    return sizeof(desc_device);
}

// HID Keyboard Report Descriptor
static uint8_t desc_hid_report_kbd[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD(),
};

// HID other report
static uint8_t desc_hid_report_other[] =
{
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(HID_REPORT_ID_MOUSE) ),
    TUD_HID_REPORT_DESC_EXTRA( HID_REPORT_ID_SYSTEM, HID_REPORT_ID_CONSUMER ),
    //TUD_HID_REPORT_DESC_NKRO( HID_REPORT_ID(HID_REPORT_ID_NKRO) )          
};

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const* tud_descriptor_hid_report_kbd_cb(void)
{
    return desc_hid_report_kbd;
}

uint32_t tud_descriptor_hid_report_kbd_size(void)
{
    return sizeof(desc_hid_report_kbd);
}

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const* tud_descriptor_hid_report_other_cb(void)
{
    return desc_hid_report_other;
}

uint32_t tud_descriptor_hid_report_other_size(void)
{
    return sizeof(desc_hid_report_other);
}

#ifdef VIAL_ENABLE
static uint8_t desc_hid_report_vial[] =
{
    TUD_HID_REPORT_DESC_VIAL(VIAL_EPSIZE),
};

uint8_t const* tud_descriptor_hid_report_vial_cb(void)
{
    return desc_hid_report_vial;
}

uint32_t tud_descriptor_hid_report_vial_size(void)
{
    return sizeof(desc_hid_report_vial);
}
#endif

// Configuration Descriptor
#ifdef VIAL_ENABLE
#define VIAL_DESC_LEN TUD_HID_INOUT_DESC_LEN 
#else
#define VIAL_DESC_LEN 0 
#endif
#ifdef WEBUSB_ENABLE
#define WEBUSB_DESC_LEN TUD_VENDOR_DESC_LEN
#else 
#define WEBUSB_DESC_LEN 0 
#endif

#ifdef MSC_ENABLE 
#define MSCUSB_DESC_LEN TUD_MSC_DESC_LEN 
#else 
#define MSCUSB_DESC_LEN 0 
#endif

#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + VIAL_DESC_LEN + WEBUSB_DESC_LEN + MSCUSB_DESC_LEN)

#define CONFIG_LEN_MSC (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + MSCUSB_DESC_LEN)

#define CONFIG_LEN_WEBUSB (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + VIAL_DESC_LEN + WEBUSB_DESC_LEN)

static uint8_t desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),

#ifdef VIAL_ENABLE
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, CFG_TUD_HID_POLL_INTERVAL),
#endif

#ifdef WEBUSB_ENABLE
    // Interface number, string index, EP Out & IN address, EP size
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, EPNUM_VENDOR_OUT, 0x80|EPNUM_VENDOR_IN, CFG_TUD_VENDOR_EPSIZE),
#endif

#ifdef MSC_ENABLE
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, 0x80|EPNUM_MSC_IN, CFG_TUD_MSC_EPSIZE),
#endif
};

#ifdef DYNAMIC_CONFIGURATION
static uint8_t desc_with_msc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_LEN_MSC, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    // Interface number, string index, EP Out & EP In address, EP size
#ifdef MSC_ENABLE
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, 0x80|EPNUM_MSC_IN, CFG_TUD_MSC_EPSIZE),
#endif
};

static uint8_t desc_with_webusb[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
#ifdef VIAL_ENABLE
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_LEN_WEBUSB, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
#else
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL-1, 0, CONFIG_LEN_WEBUSB, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
#endif
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
#ifdef VIAL_ENABLE
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, CFG_TUD_HID_POLL_INTERVAL),
#endif
    // Interface number, string index, EP Out & IN address, EP size
#ifdef WEBUSB_ENABLE
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, EPNUM_VENDOR_OUT, 0x80|EPNUM_VENDOR_IN, CFG_TUD_VENDOR_EPSIZE),
#endif
};
#endif

// Invoked when received GET CONFIGURATION DESCRIPTOR
uint8_t const* tud_descriptor_configuration_cb(uint8_t index)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return desc_with_msc;
    }
    return desc_with_webusb;
#endif

    return desc_configuration;
}

uint32_t tud_descriptor_configuration_size(uint8_t index)
{
#ifdef DYNAMIC_CONFIGURATION
    if (usb_setting & USB_MSC_BIT) {
        return sizeof(desc_with_msc);
    }
    return sizeof(desc_with_webusb);
#endif
    return sizeof(desc_configuration);
}

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_report_kbd_cb();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_report_other_cb();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_report_vial_cb();
    }
    #endif
    return NULL;
}

uint32_t tud_hid_descriptor_report_size(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_report_kbd_size();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_report_other_size();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_report_vial_size();
    }
    #endif
    return 0;
}

static uint8_t desc_hid_kbd[] = {
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
};


uint8_t const* tud_hid_descriptor_interface_cb(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_interface_kbd_cb();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_interface_other_cb();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_interface_vial_cb();
    }
    #endif
    return NULL;
}

uint32_t tud_hid_descriptor_interface_size(uint8_t itf)
{
    if (itf == ITF_NUM_HID_KBD) {
        return tud_descriptor_hid_interface_kbd_size();
    } else if (itf == ITF_NUM_HID_OTHER) {
        return tud_descriptor_hid_interface_other_size();
    }
    #ifdef VIAL_ENABLE
    else if (itf == ITF_NUM_VIAL) {
        return tud_descriptor_hid_interface_vial_size();
    }
    #endif
    return 0;
}

// Invoded when received GET HID DESCRIPTOR
uint8_t const* tud_descriptor_hid_interface_kbd_cb(void)
{
    return desc_hid_kbd;
}

uint32_t tud_descriptor_hid_interface_kbd_size(void)
{
    return sizeof(desc_hid_kbd);
}

static uint8_t desc_hid_other[] = {
    TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_BUFSIZE, CFG_TUD_HID_POLL_INTERVAL),
};

// Invoded when received GET HID DESCRIPTOR
uint8_t const* tud_descriptor_hid_interface_other_cb(void)
{
    return desc_hid_other;
}

uint32_t tud_descriptor_hid_interface_other_size(void)
{
    return sizeof(desc_hid_other);
}

#ifdef VIAL_ENABLE
static uint8_t desc_hid_vial[] = {
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VIAL, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_vial), EPNUM_VIAL_OUT, 0x80 | EPNUM_VIAL_IN, VIAL_EPSIZE, CFG_TUD_HID_POLL_INTERVAL),
};

uint8_t const* tud_descriptor_hid_interface_vial_cb(void)
{
    return desc_hid_vial;
}

uint32_t tud_descriptor_hid_interface_vial_size(void)
{
    return sizeof(desc_hid_vial);
}

#endif

#ifdef WEBUSB_ENABLE
// BOS Descriptor
#define BOS_TOTAL_LEN      (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

#define MS_OS_20_DESC_LEN  0xB2

// BOS Descriptor is required for webUSB
static uint8_t desc_bos[] = {
    // total length, number of device caps
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

    // Vendor Code, iLandingPage
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

    // Microsoft OS 2.0 descriptor
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)
};

uint8_t const* tud_descriptor_bos_cb(void)
{
    return desc_bos;
}

uint32_t tud_descriptor_bos_size(void)
{
    return BOS_TOTAL_LEN;
}

#define URL  "iatkb.com"

const tusb_desc_webusb_url_t desc_url = {
    .bLength         = 3 + sizeof(URL) - 1,
    .bDescriptorType = 3, // WEBUSB URL type
    .bScheme         = 0, // 0: http, 1: https
    .url             = URL
};

uint8_t const* tud_descriptor_url_cb(void)
{
    return (uint8_t*)&desc_url;
}

uint32_t tud_descriptor_url_size(void)
{
    return desc_url.bLength;
}

static uint8_t desc_ms_os_20[] = {
    // Set header: length, type, windows version, total length
    U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR), U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

    // Configuration subset header: length, type, configuration index, reserved, configuration total length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION), 0, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN-0x0A),

    // Function Subset header: length, type, first interface, reserved, subset length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION), ITF_NUM_VENDOR, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN-0x0A-0x08),

    // MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub compatible ID
    U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sub-compatible

    // MS OS 2.0 Registry property descriptor: length, type
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN-0x0A-0x08-0x08-0x14), U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    U16_TO_U8S_LE(0x0007), U16_TO_U8S_LE(0x002A), // wPropertyDataType, wPropertyNameLength and PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
    'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
    U16_TO_U8S_LE(0x0050), // wPropertyDataLength
    //bPropertyData: “{975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}”.
    '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, 'F', 0x00, '4', 0x00, '4', 0x00, 'D', 0x00, '9', 0x00, '-', 0x00,
    '0', 0x00, 'D', 0x00, '0', 0x00, '8', 0x00, '-', 0x00, '4', 0x00, '3', 0x00, 'F', 0x00, 'D', 0x00, '-', 0x00,
    '8', 0x00, 'B', 0x00, '3', 0x00, 'E', 0x00, '-', 0x00, '1', 0x00, '2', 0x00, '7', 0x00, 'C', 0x00, 'A', 0x00,
    '8', 0x00, 'A', 0x00, 'F', 0x00, 'F', 0x00, 'F', 0x00, '9', 0x00, 'D', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00
};

TU_VERIFY_STATIC(sizeof(desc_ms_os_20) == MS_OS_20_DESC_LEN, "Incorrect size");

uint8_t const* tud_descriptor_msos20_cb(void)
{
    return desc_ms_os_20; 
}

uint32_t tud_descriptor_msos20_size(void)
{
    return MS_OS_20_DESC_LEN;
}

#endif

// String Descriptors

// array of pointer to string descriptors
char const* string_desc_arr [] =
{
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    TU_XSTRING(MANUFACTURER),       // 1: Manufacturer
    TU_XSTRING(PRODUCT),            // 2: Product
#ifdef VIAL_ENABLE
    "vial:f64c2b3c",                // 3: Serials, should use chip ID
#else
    "amk:4d58",                     // 3: Serials, should use chip ID
#endif
    "Configuration",                // 4: Device configuration 
    "HID Keyboard",                 // 5: Hid keyboard
    "HID Extra",                    // 6: Hid extra key
    "WebUSB",                       // 7: Webusb interface
};

static uint16_t _desc_str[32];

//static uint16_t *get_device_serial(uint16_t* length);
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint16_t length = 0;
    return (uint16_t *)get_descriptor_str(index, &length);
}

// Invoked when received GET STRING DESCRIPTOR request
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
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
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