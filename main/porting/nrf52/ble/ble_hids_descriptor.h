/**
 * @file ble_hids_descriptor.h
 * @brief definition of hid descriptor
 */

#pragma once

#include "ble_config.h"
#include "tusb.h"

const static uint8_t hid_report_descriptor [] = {
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(NRF_REPORT_ID_KEYBOARD) ),
#ifdef MOUSE_ENABLE
    TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(NRF_REPORT_ID_MOUSE) ),
#endif
#ifdef EXTRAKEY_ENABLE
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL( HID_REPORT_ID(NRF_REPORT_ID_SYSTEM) ),
    TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(NRF_REPORT_ID_CONSUMER) ),
#endif
};

#if 0
#define WITH_LUFA 1
#if WITH_LUFA
#define ATTR_PACKED __attribute__((packed))
#define CONCAT(x, y) x##y
#define CONCAT_EXPANDED(x, y) CONCAT(x, y)
#define CPU_TO_LE16(x) (x)

#define __LUFA_COMMON_H__
#define __USBMODE_H__
#define __USBEVENTS_H__
#define __HIDPARSER_H__
#define __USBCONTROLLER_AVR8_H__

#define __INCLUDE_FROM_USB_DRIVER
#define __INCLUDE_FROM_HID_DRIVER
#include "lib/lufa/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h"
#include "lib/lufa/LUFA/Drivers/USB/Class/Common/HIDReportData.h"
const static uint8_t hid_report_descriptor [] = {
    HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
    HID_RI_USAGE(8, 0x06), /* Keyboard */
    HID_RI_COLLECTION(8, 0x01), /* Application */
        HID_RI_REPORT_ID(8, NRF_REPORT_ID_KEYBOARD),
        HID_RI_USAGE_PAGE(8, 0x07), /* Key Codes */
        HID_RI_USAGE_MINIMUM(8, 0xE0), /* Keyboard Left Control */
        HID_RI_USAGE_MAXIMUM(8, 0xE7), /* Keyboard Right GUI */
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_CONSTANT),  /* reserved */

        HID_RI_USAGE_PAGE(8, 0x08), /* LEDs */
        HID_RI_USAGE_MINIMUM(8, 0x01), /* Num Lock */
        HID_RI_USAGE_MAXIMUM(8, 0x05), /* Kana */
        HID_RI_REPORT_COUNT(8, 0x05),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x03),
        HID_RI_OUTPUT(8, HID_IOF_CONSTANT),

        HID_RI_USAGE_PAGE(8, 0x07), /* Keyboard */
        HID_RI_USAGE_MINIMUM(8, 0x00), /* Reserved (no event indicated) */
        HID_RI_USAGE_MAXIMUM(8, 0xFF), /* Keyboard Application */
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0xFF),
        HID_RI_REPORT_COUNT(8, 0x06),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),

        HID_RI_USAGE(8, 0x05),              // Usage (Vendor Defined)
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_COUNT(8, 0x02),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),

#ifdef MOUSE_ENABLE
    HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
    HID_RI_USAGE(8, 0x02), /* Mouse */
    HID_RI_COLLECTION(8, 0x01), /* Application */
        HID_RI_REPORT_ID(8, NRF_REPORT_ID_MOUSE),
        HID_RI_USAGE(8, 0x01), /* Pointer */
        HID_RI_COLLECTION(8, 0x00), /* Physical */

            HID_RI_USAGE_PAGE(8, 0x09), /* Button */
            HID_RI_USAGE_MINIMUM(8, 0x01),  /* Button 1 */
            HID_RI_USAGE_MAXIMUM(8, 0x05),  /* Button 5 */
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 0x05),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x03),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),

            HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
            HID_RI_USAGE(8, 0x30), /* Usage X */
            HID_RI_USAGE(8, 0x31), /* Usage Y */
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_REPORT_SIZE(8, 0x08),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

            HID_RI_USAGE(8, 0x38), /* Wheel */
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x08),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

            HID_RI_USAGE_PAGE(8, 0x0C), /* Consumer */
            HID_RI_USAGE(16, 0x0238), /* AC Pan (Horizontal wheel) */
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x08),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#endif
#ifdef EXTRAKEY_ENABLE
    HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
    HID_RI_USAGE(8, 0x80), /* System Control */
    HID_RI_COLLECTION(8, 0x01), /* Application */
        HID_RI_REPORT_ID(8, NRF_REPORT_ID_SYSTEM),
        HID_RI_LOGICAL_MINIMUM(16, 0x0001),
        HID_RI_LOGICAL_MAXIMUM(16, 0x0003),
        HID_RI_USAGE_MINIMUM(16, 0x0081), /* System Power Down */
        HID_RI_USAGE_MAXIMUM(16, 0x0083), /* System Wake Up */
        HID_RI_REPORT_SIZE(8, 16),
        HID_RI_REPORT_COUNT(8, 1),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),

    HID_RI_USAGE_PAGE(8, 0x0C), /* Consumer */
    HID_RI_USAGE(8, 0x01), /* Consumer Control */
    HID_RI_COLLECTION(8, 0x01), /* Application */
        HID_RI_REPORT_ID(8, NRF_REPORT_ID_CONSUMER),
        HID_RI_LOGICAL_MINIMUM(16, 0x0001),
        HID_RI_LOGICAL_MAXIMUM(16, 0x029C),
        HID_RI_USAGE_MINIMUM(16, 0x0001), /* +10 */
        HID_RI_USAGE_MAXIMUM(16, 0x029C), /* AC Distribute Vertically */
        HID_RI_REPORT_SIZE(8, 16),
        HID_RI_REPORT_COUNT(8, 1),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),
#endif
#else
const static uint8_t hid_report_descriptor [] = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x06,       // Usage (Keyboard)
    0xA1, 0x01,       // Collection (Application)
    0x05, 0x07,       // Usage Page (Key Codes)
    0x19, 0xe0,       // Usage Minimum (224)
    0x29, 0xe7,       // Usage Maximum (231)
    0x15, 0x00,       // Logical Minimum (0)
    0x25, 0x01,       // Logical Maximum (1)
    0x75, 0x01,       // Report Size (1)
    0x95, 0x08,       // Report Count (8)
    0x81, 0x02,       // Input (Data, Variable, Absolute)

    0x95, 0x01,       // Report Count (1)
    0x75, 0x08,       // Report Size (8)
    0x81, 0x01,       // Input (Constant) reserved byte(1)

    0x95, 0x05,       // Report Count (5)
    0x75, 0x01,       // Report Size (1)
    0x05, 0x08,       // Usage Page (Page# for LEDs)
    0x19, 0x01,       // Usage Minimum (1)
    0x29, 0x05,       // Usage Maximum (5)
    0x91, 0x02,       // Output (Data, Variable, Absolute), Led report
    0x95, 0x01,       // Report Count (1)
    0x75, 0x03,       // Report Size (3)
    0x91, 0x01,       // Output (Data, Variable, Absolute), Led report padding

    0x95, 0x06,       // Report Count (6)
    0x75, 0x08,       // Report Size (8)
    0x15, 0x00,       // Logical Minimum (0)
    0x25, 0x65,       // Logical Maximum (101)
    0x05, 0x07,       // Usage Page (Key codes)
    0x19, 0x00,       // Usage Minimum (0)
    0x29, 0x65,       // Usage Maximum (101)
    0x81, 0x00,       // Input (Data, Array) Key array(6 bytes)

    0x09, 0x05,       // Usage (Vendor Defined)
    0x15, 0x00,       // Logical Minimum (0)
    0x26, 0xFF, 0x00, // Logical Maximum (255)
    0x75, 0x08,       // Report Size (8 bit)
    0x95, 0x02,       // Report Count (2)
    0xB1, 0x02,       // Feature (Data, Variable, Absolute)

    0xC0              // End Collection (Application)
#endif

};

#endif