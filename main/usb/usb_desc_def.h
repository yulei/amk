/**
 * @file usb_desc_def.h
 */

#pragma once

#include <stdint.h>

#define UDD_HIGH(x) ((uint8_t)(((x) >> 8) & 0x00ff))
#define UDD_LOW(x)  ((uint8_t)((x) & 0x00ff))
#define UDD_STR(x)  #x
#define UDD_XSTR(x) UDD_STR(x)

enum
{
    UDD_DESC_DEVICE         = 0x01,
    UDD_DESC_CONFIGURATION  = 0x02,
    UDD_DESC_STRING         = 0x03,
    UDD_DESC_INTERFACE      = 0x04,
    UDD_DESC_ENDPOINT       = 0x05,
};

enum
{
    UDD_ATT_REMOTE_WAKEUP = 1<<5,
    UDD_ATT_SELF_POWERED  = 1<<6,
};

enum
{
    UDD_CLASS_AUDIO     = 1,
    UDD_CLASS_HID       = 3,
    UDD_CLASS_MSC       = 8,
    UDD_CLASS_VENDOR    = 0xFF,
};

enum
{
    UDD_AUDIO_SUBCLASS_UNDEFINED    = 0,
    UDD_AUDIO_SUBCLASS_CONTROL      = 1,
    UDD_AUDIO_SUBCLASS_STREAMING    = 2,
    UDD_AUDIO_SUBCLASS_MIDI         = 3,
};

enum
{
    UDD_AUDIO_PROTOCOL_UNDEFINED    = 0,
};

enum
{
    UDD_AUDIO_DESC_UNDEFINED        = 0x20,
    UDD_AUDIO_DESC_DEVICE           = 0x21,
    UDD_AUDIO_DESC_CONFIGURATION    = 0x22,
    UDD_AUDIO_DESC_STRING           = 0x23,
    UDD_AUDIO_DESC_INTERFACE        = 0x24,
    UDD_AUDIO_DESC_ENDPOINT         = 0x25,
};

enum
{
    UDD_AUDIO_CONTROL_DESC_UNDEFINED        = 0x00,
    UDD_AUDIO_CONTROL_DESC_HEADER           = 0x01,
    UDD_AUDIO_CONTROL_DESC_INPUT_TERMINAL   = 0x02,
    UDD_AUDIO_CONTROL_DESC_OUTPUT_TERMINAL  = 0x03,
    UDD_AUDIO_CONTROL_DESC_MIXER_UNIT       = 0x04,
    UDD_AUDIO_CONTROL_DESC_SELECTOR_UNIT    = 0x05,
    UDD_AUDIO_CONTROL_DESC_FEATURE_UNIT     = 0x06,
    UDD_AUDIO_CONTROL_DESC_PROCESSING_UNIT  = 0x07,
    UDD_AUDIO_CONTROL_DESC_EXTENSION_UNIT   = 0x08,
};

enum
{
    UDD_AUDIO_STREAMING_DESC_UNDEFINED      = 0x00,
    UDD_AUDIO_STREAMING_DESC_GENERAL        = 0x01,
    UDD_AUDIO_STREAMING_DESC_FORMAT_TYPE    = 0x02,
    UDD_AUDIO_STREAMING_DESC_FORMAT_SPECIFIC= 0x03,
};

enum 
{
    UDD_AUDIO_TERMINAL_UNDEFINED    = 0x0100,
    UDD_AUDIO_TERMINAL_STREAMING    = 0x0101,
    UDD_AUDIO_TERMINAL_VENDOR       = 0x01FF,
};

enum
{
    UDD_HID_SUBCLASS_NONE = 0,
    UDD_HID_SUBCLASS_BOOT = 1,
};

enum
{
    UDD_HID_DESC_TYPE_HID       = 0x21,
    UDD_HID_DESC_TYPE_REPORT    = 0x22,
    UDD_HID_DESC_TYPE_PHYSICAL  = 0x23,
};


enum
{
    UDD_MSC_SUBCLASS_SCSI = 0x06,
};

enum
{
    UDD_MSC_PROTOCOL_BOT = 0x50,
};

enum
{
    UDD_EP_CONTROL      = 0,
    UDD_EP_ISOCHRONOUS  = 1,
    UDD_EP_BULK         = 2,
    UDD_EP_INTERRUPT    = 3,
};

// descriptor size
#define UDD_CONFIG_DESC_LEN         (9)
#define UDD_HID_DESC_LEN            (9+9+7)
#define UDD_HID_GENERIC_DESC_LEN    (9+9+7+7)

#ifdef VIAL_ENABLE
#define UDD_VIAL_DESC_LEN           UDD_HID_GENERIC_DESC_LEN 
#else
#define UDD_VIAL_DESC_LEN 0 
#endif

#ifdef MSC_ENABLE
#define UDD_MSC_DESC_LEN            (9+7+7)
#else
#define UDD_MSC_DESC_LEN            (0)
#endif

#ifdef AUDIO_ENABLE
#define UDD_AUDIO_DESC_LEN          (9+9+9+12+9+9+9+9+7+11+9+7)
#else
#define UDD_AUDIO_DESC_LEN          (0)
#endif

#define UDD_AUDIO_INPUT_TERMINAL_DESC_LEN       (12)
#define UDD_AUDIO_FEATURE_UNIT_DESC_LEN         (9)
#define UDD_AUDIO_OUTPUT_TERMINAL_DESC_LEN      (9)
#define UDD_AUDIO_STREAMING_INTERFACE_DESC_LEN  (7)

#define UDD_AUDIO_CLASS_DESC_LEN                (9+UDD_AUDIO_INPUT_TERMINAL_DESC_LEN+UDD_AUDIO_FEATURE_UNIT_DESC_LEN+UDD_AUDIO_OUTPUT_TERMINAL_DESC_LEN)

// endpoint configuration
#define UDD_EP0_SIZE        64
#define UDD_HID_EP_SIZE     32
#define UDD_HID_INTERVAL    10
#define UDD_MSC_EP_SIZE     64
#define UDD_MSC_BUFFER_SIZE 4096

#define UDD_AUDIO_FREQ_221  22100U
#define UDD_AUDIO_FREQ_441  44100U
#define UDD_AUDIO_FREQ_480  48000U

#ifndef UDD_AUDIO_FREQ
#define UDD_AUDIO_FREQ      UDD_AUDIO_FREQ_480
#endif

#ifndef UDD_AUDIO_CHANNELS
#define UDD_AUDIO_CHANNELS  2
#endif

#ifndef UDD_AUDIO_SAMPLE_FRAME
#define UDD_AUDIO_SAMPLE_FRAME  2
#endif

#ifndef UDD_AUDIO_SAMPLE_BITS
#define UDD_AUDIO_SAMPLE_BITS   16
#endif

#define UDD_AUDIO_PACKET_SIZE ((UDD_AUDIO_FREQ*UDD_AUDIO_CHANNELS*UDD_AUDIO_SAMPLE_SIZE)/1000)

#ifndef UDD_AUDIO_INTERVAL
#define UDD_AUDIO_INTERVAL  1
#endif

#define UDD_DEVICE_DESCRIPTOR(usb_ver, ep0_size, vendor, product, version, str_manufacture, str_product, str_serial ) \
    18, UDD_DESC_DEVICE, UDD_LOW(usb_ver), UDD_HIGH(usb_ver), 0x00, 0x00, 0x00, ep0_size, \
    UDD_LOW(vendor), UDD_HIGH(vendor), UDD_LOW(product), UDD_HIGH(product), UDD_LOW(version), UDD_HIGH(version), \
    str_manufacture, str_product, str_serial, 1

#define UDD_CONFIG_DESCRIPTOR(config, itf_num, str, total, attr, power) \
    9, UDD_DESC_CONFIGURATION, UDD_LOW(total), UDD_HIGH(total), itf_num, config, str, (1<<7) | attr, power/2

#define UDD_HID_DESCRIPTOR(itf, str, boot, report_len, epin, epsize, interval) \
    9, UDD_DESC_INTERFACE, itf, 0, 1, UDD_CLASS_HID, ((boot) ? UDD_HID_SUBCLASS_BOOT : 0), boot, str,\
    9, UDD_HID_DESC_TYPE_HID, 0x11, 0x01, 0, 1, UDD_HID_DESC_TYPE_REPORT, UDD_LOW(report_len), UDD_HIGH(report_len),\
    7, UDD_DESC_ENDPOINT, epin, UDD_EP_INTERRUPT, UDD_LOW(epsize), UDD_HIGH(epsize), interval 

#define UDD_HID_GENERIC_DESCRIPTOR(itf, str, boot, report_len, epout, epin, epsize, interval) \
    9, UDD_DESC_INTERFACE, itf, 0, 2, UDD_CLASS_HID, ((boot) ? UDD_HID_SUBCLASS_BOOT : 0), boot, str,\
    9, UDD_HID_DESC_TYPE_HID, 0x11, 0x01, 0, 1, UDD_HID_DESC_TYPE_REPORT, UDD_LOW(report_len), UDD_HIGH(report_len),\
    7, UDD_DESC_ENDPOINT, epout, UDD_EP_INTERRUPT, UDD_LOW(epsize), UDD_HIGH(epsize), interval, \
    7, UDD_DESC_ENDPOINT, epin, UDD_EP_INTERRUPT, UDD_LOW(epsize), UDD_HIGH(epsize), interval

#define UDD_MSC_DESCRIPTOR(itf, str, epout, epin, epsize) \
    9, UDD_DESC_INTERFACE, itf, 0, 2, UDD_CLASS_MSC, UDD_MSC_SUBCLASS_SCSI, UDD_MSC_PROTOCOL_BOT, str,\
    7, UDD_DESC_ENDPOINT, epout, UDD_EP_BULK, UDD_LOW(epsize), UDD_HIGH(epsize), 0,\
    7, UDD_DESC_ENDPOINT, epin, UDD_EP_BULK, UDD_LOW(epsize), UDD_HIGH(epsize), 0

#define UDD_AUDIO_CLASS_DESCRIPTOR_HEADER(itf_control, itf_streaming, total) \
    9, UDD_AUDIO_DESC_INTERFACE, UDD_AUDIO_SUBCLASS_CONTROL, 0, 1, total,  itf_control, itf_streaming, itf_streaming

#define UDD_AUDIO_INPUT_TERMINAL_DESCRIPTOR() \
    12, UDD_AUDIO_DESC_INTERFACE, UDD_AUDIO_CONTROL_DESC_INPUT_TERMINAL, 1, UDD_LOW(UDD_AUDIO_TERMINAL_STREAMING) \
    UDD_HIGH(UDD_AUDIO_TERMINAL_STREAMING), 0, 1, 0, 0, 0, 0

#define UDD_AUDIO_FEATURE_UNIT_DESCRIPTOR() \
    9, UDD_AUDIO_DESC_INTERFACE, UDD_AUDIO_CONTROL_DESC_FEATURE_UNIT, 2, 1, 1, 1, 0, 0

#define UDD_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR() \
    9, UDD_AUDIO_DESC_INTERFACE, UDD_AUDIO_CONTROL_DESC_OUTPUT_TERMINAL, 2, 1, 3, 0, 2, 0

#define UDD_AUDIO_STREAMING_DESCRIPTOR() \
    7, UDD_AUDIO_DESC_INTERFACE, UDD_AUDIO_SUBCLASS_STREAMING, 1, 1, 1, 0

#define UDD_AUDIO_FORMAT_TYPE_DESCRIPTOR(type, channels, frame, bits, frequency) \
    11, UDD_AUDIO_DESC_INTERFACE, UDD_AUDIO_STREAMING_DESC_FORMAT_TYPE, type, channels, frame, bits, 1, \
    (uint8_t)(frequency), (uint8_t)((frequency >> 8)), (uint8_t)((frequency>> 16))

#define UDD_AUDIO_ENDPOINT_STANDARD_DESCRIPTOR(epnum, size) \
    9, UDD_DESC_ENDPOINT, epnum, UDD_EP_ISOCHRONOUS, UDD_LOW(size), UDD_HIGH(size), UDD_AUDIO_INTERVAL, 0, 0

#define UDD_AUDIO_ENDPOINT_STREAMING_DESCRIPTOR(attr, unit, delay) \
    7, UDD_AUDIO_DESC_ENDPOINT, 0x01, attr, unit, delay

#define UDD_AUDIO_INTERFACE_DESCRIPTOR(itf, alt, epnum, sub) \
    9, UDD_DESC_INTERFACE, itf, alt, epnum, UDD_CLASS_AUDIO, sub, UDD_AUDIO_PROTOCOL_UNDEFINED, 0

#define UDD_USAGE_PAGE(x)	        0x05, x
#define UDD_USAGE_PAGE_2(x)	        0x06, UDD_LOW(x), UDD_HIGH(x)
#define UDD_USAGE(x)	            0x09, x
#define UDD_USAGE_2(x)	            0x0A, UDD_LOW(x), UDD_HIGH(x)
#define UDD_COLLECTION(x)           0xA1, x
#define UDD_USAGE_MINIMUM(x)        0x19, x
#define UDD_USAGE_MAXIMUM(x)        0x29, x
#define UDD_USAGE_MAXIMUM_2(x)      0x2A, UDD_LOW(x), UDD_HIGH(x)
#define UDD_LOGICAL_MINIMUM(x)	    0x15, x
#define UDD_LOGICAL_MAXIMUM(x)      0x25, x
#define UDD_LOGICAL_MAXIMUM_2(x)    0x26, UDD_LOW(x), UDD_HIGH(x)
#define UDD_REPORT_COUNT(x)         0x95, x
#define UDD_REPORT_SIZE(x)          0x75, x
#define UDD_REPORT_ID(x)            0x85, x
//#define UDD_KBD_REPORT_ID(x)        0x85, x,
#define UDD_INPUT(x)                0x81, (x)
#define UDD_OUTPUT(x)               0x91, (x)
#define UDD_END_COLLECTION(x)       0xC0

#define UDD_HID_DATA            (0<<0)
#define UDD_HID_CONSTANT        (1<<0)
#define UDD_HID_ARRAY           (0<<1)
#define UDD_HID_VARIABLE        (1<<1)
#define UDD_HID_ABSOLUTE        (0<<2)
#define UDD_HID_RELATIVE        (1<<2)

#define UDD_HID_REPORT_DESC_KEYBOARD(...) \
    UDD_USAGE_PAGE(0x01), \
    UDD_USAGE(0x06), \
    UDD_COLLECTION(0x01), \
        __VA_ARGS__ \
\
        UDD_USAGE_PAGE(0x07), \
        UDD_USAGE_MINIMUM(0xE0), \
        UDD_USAGE_MAXIMUM(0xE7), \
        UDD_LOGICAL_MINIMUM(0x00), \
        UDD_LOGICAL_MAXIMUM(0x01), \
        UDD_REPORT_COUNT(0x08), \
        UDD_REPORT_SIZE(0x01), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        UDD_REPORT_COUNT(0x01), \
        UDD_REPORT_SIZE(0x08), \
        UDD_INPUT(UDD_HID_CONSTANT), \
\
        UDD_USAGE_PAGE(0x07), \
        UDD_USAGE_MINIMUM(0x00), \
        UDD_USAGE_MAXIMUM_2(0x00FF), \
        UDD_LOGICAL_MINIMUM(0x00), \
        UDD_LOGICAL_MAXIMUM_2(0x00FF), \
        UDD_REPORT_COUNT(0x06), \
        UDD_REPORT_SIZE(0x08), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_ARRAY | UDD_HID_ABSOLUTE), \
\
        UDD_USAGE_PAGE(0x08), \
        UDD_USAGE_MINIMUM(0x01), \
        UDD_USAGE_MAXIMUM(0x05), \
        UDD_REPORT_COUNT(0x05), \
        UDD_REPORT_SIZE(0x01), \
        UDD_OUTPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
        UDD_REPORT_COUNT(0x01), \
        UDD_REPORT_SIZE(0x03), \
        UDD_OUTPUT(UDD_HID_CONSTANT), \
    UDD_END_COLLECTION(0)

#define UDD_HID_REPORT_DESC_MOUSE(mouse) \
    UDD_USAGE_PAGE(0x01), \
    UDD_USAGE(0x02), \
    UDD_COLLECTION(0x01), \
        UDD_REPORT_ID(mouse), \
        UDD_USAGE(0x01), \
        UDD_COLLECTION(0x00), \
\
            UDD_USAGE_PAGE(0x09), \
            UDD_USAGE_MINIMUM(0x01), \
            UDD_USAGE_MAXIMUM(0x08), \
            UDD_LOGICAL_MINIMUM(0x00), \
            UDD_LOGICAL_MAXIMUM(0x01), \
            UDD_REPORT_COUNT(0x08), \
            UDD_REPORT_SIZE(0x01), \
            UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
            UDD_USAGE_PAGE(0x01), \
            UDD_USAGE(0x30), \
            UDD_USAGE(0x31), \
            UDD_LOGICAL_MINIMUM(-127), \
            UDD_LOGICAL_MAXIMUM(127), \
            UDD_REPORT_COUNT(0x02), \
            UDD_REPORT_SIZE(0x08), \
            UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_RELATIVE), \
\
            UDD_USAGE(0x38), \
            UDD_LOGICAL_MINIMUM(-127), \
            UDD_LOGICAL_MAXIMUM(127), \
            UDD_REPORT_COUNT(0x01), \
            UDD_REPORT_SIZE(0x08), \
            UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_RELATIVE), \
\
            UDD_USAGE_PAGE(0x0C), \
            UDD_USAGE_2(0x0238), \
            UDD_LOGICAL_MINIMUM(-127), \
            UDD_LOGICAL_MAXIMUM(127), \
            UDD_REPORT_COUNT(0x01), \
            UDD_REPORT_SIZE(0x08), \
            UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_RELATIVE), \
        UDD_END_COLLECTION(0), \
    UDD_END_COLLECTION(0) \

#define UDD_HID_REPORT_DESC_EXTRA(system, comsumer) \
    UDD_USAGE_PAGE(0x01), \
    UDD_USAGE(0x80), \
    UDD_COLLECTION(0x01), \
        UDD_REPORT_ID(system), \
        UDD_USAGE_MINIMUM(0x01), \
        UDD_USAGE_MAXIMUM_2(0x00B7), \
        UDD_LOGICAL_MINIMUM(0x01), \
        UDD_LOGICAL_MAXIMUM_2(0x00B7), \
        UDD_REPORT_COUNT(1), \
        UDD_REPORT_SIZE(16), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_ARRAY | UDD_HID_ABSOLUTE), \
    UDD_END_COLLECTION(0), \
\
    UDD_USAGE_PAGE(0x0C), \
    UDD_USAGE(0x01), \
    UDD_COLLECTION(0x01), \
        UDD_REPORT_ID(comsumer), \
        UDD_USAGE_MINIMUM(0x01), \
        UDD_USAGE_MAXIMUM_2(0x02A0), \
        UDD_LOGICAL_MINIMUM(0x01), \
        UDD_LOGICAL_MAXIMUM_2(0x02A0), \
        UDD_REPORT_COUNT(1), \
        UDD_REPORT_SIZE(16), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_ARRAY | UDD_HID_ABSOLUTE), \
    UDD_END_COLLECTION(0)

#ifndef REPORT_BITS
#define REPORT_BITS 30
#endif

#define UDD_HID_REPORT_DESC_NKRO(nkro) \
    UDD_USAGE_PAGE(0x01), \
    UDD_USAGE(0x06), \
    UDD_COLLECTION(0x01), \
\
        UDD_REPORT_ID(nkro), \
        UDD_USAGE_PAGE(0x07), \
        UDD_USAGE_MINIMUM(0xE0), \
        UDD_USAGE_MAXIMUM(0xE7), \
        UDD_LOGICAL_MINIMUM(0x00), \
        UDD_LOGICAL_MAXIMUM(0x01), \
        UDD_REPORT_COUNT(0x08), \
        UDD_REPORT_SIZE(0x01), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        UDD_USAGE_PAGE(0x07), \
        UDD_USAGE_MINIMUM(0x00), \
        UDD_USAGE_MAXIMUM(REPORT_BITS * 8 - 1), \
        UDD_LOGICAL_MINIMUM(0x00), \
        UDD_LOGICAL_MAXIMUM(0x01), \
        UDD_REPORT_COUNT(REPORT_BITS * 8), \
        UDD_REPORT_SIZE(0x01), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        UDD_USAGE_PAGE(0x08), \
        UDD_USAGE_MINIMUM(0x01), \
        UDD_USAGE_MAXIMUM(0x05), \
        UDD_REPORT_COUNT(0x05), \
        UDD_REPORT_SIZE(0x01), \
        UDD_OUTPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE | UDD_HID_NON_VOLATILE), \
\
        UDD_REPORT_COUNT(0x01), \
        UDD_REPORT_SIZE(0x03), \
        UDD_OUTPUT(UDD_HID_CONSTANT), \
    UDD_END_COLLECTION(0)

#ifndef RAW_USAGE_PAGE
#    define RAW_USAGE_PAGE 0xFF60
#endif

#ifndef RAW_USAGE_ID
#    define RAW_USAGE_ID 0x61
#endif

#define UDD_HID_REPORT_DESC_VIAL(page, id, size) \
    UDD_USAGE_PAGE_2(page), \
    UDD_USAGE(id), \
    UDD_COLLECTION(0x01), \
        UDD_USAGE(0x62), \
        UDD_LOGICAL_MINIMUM(0x00), \
        UDD_LOGICAL_MAXIMUM_2(0x00FF), \
        UDD_REPORT_COUNT(size), \
        UDD_REPORT_SIZE(0x08), \
        UDD_INPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE), \
\
        UDD_USAGE(0x63), \
        UDD_LOGICAL_MINIMUM(0x00), \
        UDD_LOGICAL_MAXIMUM_2(0x00FF), \
        UDD_REPORT_COUNT(RAW_EPSIZE), \
        UDD_REPORT_SIZE(0x08), \
        UDD_OUTPUT(UDD_HID_DATA | UDD_HID_VARIABLE | UDD_HID_ABSOLUTE | UDD_HID_NON_VOLATILE), \
    UDD_END_COLLECTION(0)
