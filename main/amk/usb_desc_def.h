/**
 * @file usb_desc_def.h
 */

#pragma once

#include <stdint.h>

#define UDD_HIGH(x) ((uint8_t)(((x) >> 8) & 0x00ff))
#define UDD_LOW(x)  ((uint8_t)((x) & 0x00ff))

enum
{
    UDD_DESC_DEVICE         = 0x01,
    UDD_DESC_CONFIGURATION  = 0x02,
    UDD_DESC_STRING         = 0x03,
    UDD_DESC_INTERFACE      = 0x04,
    UDD_DESC_ENDPOINT       = 0x05
};

enum {
    UDD_ATT_REMOTE_WAKEUP = 1<<5,
    UDD_ATT_SELF_POWERED  = 1<<6,
};

enum
{
    UDD_CLASS_HID       = 3,
    UDD_CLASS_MSC       = 8,
    UDD_CLASS_VENDOR    = 0xFF,
};

enum
{
    HID_SUBCLASS_NONE = 0,
    HID_SUBCLASS_BOOT = 1,
};

enum
{
    HID_DESC_TYPE_HID       = 0x21,
    HID_DESC_TYPE_REPORT    = 0x22,
    HID_DESC_TYPE_PHYSICAL  = 0x23,
};

enum
{
    MSC_SUBCLASS_SCSI   = 0x06,
};

enum
{
    MSC_PROTOCOL_BOT    = 0x50,
};

enum
{
  UDD_XFER_CONTROL      = 0,
  UDD_XFER_ISOCHRONOUS  = 1,
  UDD_XFER_BULK         = 2,
  UDD_XFER_INTERRUPT    = 3,
};


// endpoint configuration
#define UDD_EP0_SIZE        64
#define UDD_HID_EP_SIZE     32
#define UDD_HID_INTERVAL    10
#define UDD_MSC_EP_SIZE     64
#define UDD_MSC_BUFFER_SIZE 4096

#define UDD_DEVICE_DESCRIPTOR(usb_ver, ep0_size, vendor, product, version, str_manufacture, str_product, str_serial ) \
    18, UDD_DESC_DEVICE, UDD_LOW(usb_ver), UDD_HIGH(usb_ver), 0x00, 0x00, 0x00, ep0_size, \
    UDD_LOW(vendor), UDD_HIGH(vendor), UDD_LOW(product), UDD_HIGH(product), UDD_LOW(version), UDD_HIGH(version), \
    str_manufacture, str_product, str_serial, 1

#define UDD_CONFIG_DESCRIPTOR(config, itf_num, str, total, attr, power) \
    9, UDD_DESC_CONFIGURATION, UDD_LOW(total), UDD_HIGH(total), itf_num, config, str, (1<<7) | attr, power/2

#define UDD_HID_DESCRIPTOR(itf, str, boot, report_len, epin, epsize, interval) \
    9, UDD_DESC_INTERFACE, itf, 0, 1, UDD_CLASS_HID, ((boot) ? HID_SUBCLASS_BOOT : 0), boot, str,\
    9, HID_DESC_TYPE_HID, 0x11, 0x01, 0, 1, HID_DESC_TYPE_REPORT, UDD_LOW(report_len), UDD_HIGH(report_len),\
    7, UDD_DESC_ENDPOINT, epin, UDD_XFER_INTERRUPT, UDD_LOW(epsize), UDD_HIGH(epsize), interval 

#define UDD_HID_GENERIC_DESCRIPTOR(itf, str, boot, report_len, epout, epin, epsize, interval) \
    9, UDD_DESC_INTERFACE, itf, 0, 2, UDD_CLASS_HID, ((boot) ? HID_SUBCLASS_BOOT : 0), boot, str,\
    9, HID_DESC_TYPE_HID, 0x11, 0x01, 0, 1, HID_DESC_TYPE_REPORT, UDD_LOW(report_len), UDD_HIGH(report_len),\
    7, UDD_DESC_ENDPOINT, epout, UDD_XFER_INTERRUPT, UDD_LOW(epsize), UDD_HIGH(epsize), interval, \
    7, UDD_DESC_ENDPOINT, epin, UDD_XFER_INTERRUPT, UDD_LOW(epsize), UDD_HIGH(epsize), interval

#define UDD_MSC_DESCRIPTOR(itf, str, epout, epin, epsize) \
    9, UDD_DESC_INTERFACE, itf, 0, 2, UDD_CLASS_MSC, MSC_SUBCLASS_SCSI, MSC_PROTOCOL_BOT, str,\
    7, UDD_DESC_ENDPOINT, epout, UDD_XFER_BULK, UDD_LOW(epsize), UDD_HIGH(epsize), 0,\
    7, UDD_DESC_ENDPOINT, epin, UDD_XFER_BULK, UDD_LOW(epsize), UDD_HIGH(epsize), 0

#define UDD_CONFIG_DESC_LEN         (9)
#define UDD_HID_DESC_LEN            (9+9+7)
#define UDD_HID_GENERIC_DESC_LEN    (9+9+7+7)
#define UDD_MSC_DESC_LEN            (9+7+7)

#define USAGE_PAGE(x)	        0x05, x
#define USAGE_PAGE_2(x)	        0x06, UDD_LOW(x), UDD_HIGH(x)
#define USAGE(x)	            0x09, x
#define USAGE_2(x)	            0x0A, UDD_LOW(x), UDD_HIGH(x)
#define COLLECTION(x)           0xA1, x
#define USAGE_MINIMUM(x)        0x19, x
#define USAGE_MAXIMUM(x)        0x29, x
#define USAGE_MAXIMUM_2(x)      0x2A, UDD_LOW(x), UDD_HIGH(x)
#define LOGICAL_MINIMUM(x)	    0x15, x
#define LOGICAL_MAXIMUM(x)      0x25, x
#define LOGICAL_MAXIMUM_2(x)    0x26, UDD_LOW(x), UDD_HIGH(x)
#define REPORT_COUNT(x)         0x95, x
#define REPORT_SIZE(x)          0x75, x
#define REPORT_ID(x)            0x85, x
#define KBD_REPORT_ID(x)        0x85, x,
#define INPUT(x)                0x81, (x)
#define OUTPUT(x)               0x91, (x)
#define END_COLLECTION(x)       0xC0

#define UDD_HID_DATA            (0<<0)
#define UDD_HID_CONSTANT        (1<<0)
#define UDD_HID_ARRAY           (0<<1)
#define UDD_HID_VARIABLE        (1<<1)
#define UDD_HID_ABSOLUTE        (0<<2)
#define UDD_HID_RELATIVE        (1<<2)

#define UDD_HID_REPORT_DESC_KEYBOARD(...) \
    USAGE_PAGE(0x01), \
    USAGE(0x06), \
    COLLECTION(0x01), \
        __VA_ARGS__ \
\
        USAGE_PAGE(0x07), \
        USAGE_MINIMUM(0xE0), \
        USAGE_MAXIMUM(0xE7), \
        LOGICAL_MINIMUM(0x00), \
        LOGICAL_MAXIMUM(0x01), \
        REPORT_COUNT(0x08), \
        REPORT_SIZE(0x01), \
        INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        REPORT_COUNT(0x01), \
        REPORT_SIZE(0x08), \
        INPUT(UDD_HID_CONSTANT), \
\
        USAGE_PAGE(0x07), \
        USAGE_MINIMUM(0x00), \
        USAGE_MAXIMUM(0xFF), \
        LOGICAL_MINIMUM(0x00), \
        LOGICAL_MAXIMUM_2(0x00FF), \
        REPORT_COUNT(0x06), \
        REPORT_SIZE(0x08), \
        INPUT(UDD_HID_DATA | UDD_HID_ARRAY | UDD_HID_ABSOLUTE), \
\
        USAGE_PAGE(0x08), \
        USAGE_MINIMUM(0x01), \
        USAGE_MAXIMUM(0x05), \
        REPORT_COUNT(0x05), \
        REPORT_SIZE(0x01), \
        OUTPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
        REPORT_COUNT(0x01), \
        REPORT_SIZE(0x03), \
        OUTPUT(UDD_HID_CONSTANT), \
    END_COLLECTION(0)

#define UDD_HID_REPORT_DESC_MOUSE(mouse) \
    USAGE_PAGE(0x01), \
    USAGE(0x02), \
    COLLECTION(0x01), \
        REPORT_ID(mouse), \
        USAGE(0x01), \
        COLLECTION(0x00), \
\
            USAGE_PAGE(0x09), \
            USAGE_MINIMUM(0x01), \
            USAGE_MAXIMUM(0x08), \
            LOGICAL_MINIMUM(0x00), \
            LOGICAL_MAXIMUM(0x01), \
            REPORT_COUNT(0x08), \
            REPORT_SIZE(0x01), \
            INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
            USAGE_PAGE(0x01), \
            USAGE(0x30), \
            USAGE(0x31), \
            LOGICAL_MINIMUM(-127), \
            LOGICAL_MAXIMUM(127), \
            REPORT_COUNT(0x02), \
            REPORT_SIZE(0x08), \
            INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_RELATIVE), \
\
            USAGE(0x38), \
            LOGICAL_MINIMUM(-127), \
            LOGICAL_MAXIMUM(127), \
            REPORT_COUNT(0x01), \
            REPORT_SIZE(0x08), \
            INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_RELATIVE), \
\
            USAGE_PAGE(0x0C), \
            USAGE_2(0x0238), \
            LOGICAL_MINIMUM(-127), \
            LOGICAL_MAXIMUM(127), \
            REPORT_COUNT(0x01), \
            REPORT_SIZE(0x08), \
            INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_RELATIVE), \
        END_COLLECTION(0), \
    END_COLLECTION(0) \

#define UDD_HID_REPORT_DESC_EXTRA(system, comsumer) \
    USAGE_PAGE(0x01), \
    USAGE(0x80), \
    COLLECTION(0x01), \
        REPORT_ID(system), \
        USAGE_MINIMUM(0x01), \
        USAGE_MAXIMUM_2(0x00B7), \
        LOGICAL_MINIMUM(0x01), \
        LOGICAL_MAXIMUM_2(0x00B7), \
        REPORT_COUNT(1), \
        REPORT_SIZE(16), \
        INPUT(UDD_HID_DATA | UDD_HID_ARRAY | UDD_HID_ABSOLUTE), \
    END_COLLECTION(0), \
\
    USAGE_PAGE(0x0C), \
    USAGE(0x01), \
    COLLECTION(0x01), \
        REPORT_ID(comsumer), \
        USAGE_MINIMUM(0x01), \
        USAGE_MAXIMUM_2(0x02A0), \
        LOGICAL_MINIMUM(0x01), \
        LOGICAL_MAXIMUM_2(0x02A0), \
        REPORT_COUNT(1), \
        REPORT_SIZE(16), \
        INPUT(UDD_HID_DATA | UDD_HID_ARRAY | UDD_HID_ABSOLUTE), \
    END_COLLECTION(0)

#ifndef  REPORT_BITS
#define REPORT_BITS 30
#endif

#define UDD_HID_REPORT_DESC_NKRO(nkro) \
    USAGE_PAGE(0x01), \
    USAGE(0x06), \
    COLLECTION(0x01), \
\
        REPORT_ID(nkro), \
        USAGE_PAGE(0x07), \
        USAGE_MINIMUM(0xE0), \
        USAGE_MAXIMUM(0xE7), \
        LOGICAL_MINIMUM(0x00), \
        LOGICAL_MAXIMUM(0x01), \
        REPORT_COUNT(0x08), \
        REPORT_SIZE(0x01), \
        INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        USAGE_PAGE(0x07), \
        USAGE_MINIMUM(0x00), \
        USAGE_MAXIMUM(REPORT_BITS * 8 - 1), \
        LOGICAL_MINIMUM(0x00), \
        LOGICAL_MAXIMUM(0x01), \
        REPORT_COUNT(REPORT_BITS * 8), \
        REPORT_SIZE(0x01), \
        INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        USAGE_PAGE(0x08), \
        USAGE_MINIMUM(0x01), \
        USAGE_MAXIMUM(0x05), \
        REPORT_COUNT(0x05), \
        REPORT_SIZE(0x01), \
        OUTPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE | UDD_HID_NON_VOLATILE), \
\
        REPORT_COUNT(0x01), \
        REPORT_SIZE(0x03), \
        OUTPUT(UDD_HID_CONSTANT), \
    END_COLLECTION(0)

#ifndef RAW_USAGE_PAGE
#    define RAW_USAGE_PAGE 0xFF60
#endif

#ifndef RAW_USAGE_ID
#    define RAW_USAGE_ID 0x61
#endif

#define UDD_HID_REPORT_DESC_VIAL(page, id, size) \
    USAGE_PAGE_2(page), \
    USAGE(id), \
    COLLECTION(0x01), \
        USAGE(0x62), \
        LOGICAL_MINIMUM(0x00), \
        LOGICAL_MAXIMUM_2(0x00FF), \
        REPORT_COUNT(size), \
        REPORT_SIZE(0x08), \
        INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        USAGE(0x63), \
        LOGICAL_MINIMUM(0x00), \
        LOGICAL_MAXIMUM_2(0x00FF), \
        REPORT_COUNT(RAW_EPSIZE), \
        REPORT_SIZE(0x08), \
        OUTPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE | UDD_HID_NON_VOLATILE), \
    END_COLLECTION(0)
