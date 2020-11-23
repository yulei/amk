/**
 * @file usbh_hid_def.h
 */

#pragma once

#define HID_MIN_POLL                                10U
//#define HID_REPORT_SIZE                             16U
#define HID_MAX_USAGE                               10U
#define HID_MAX_NBR_REPORT_FMT                      10U
#define HID_PACKET_SIZE                             16U
#define HID_QUEUE_SIZE                              10U

#define  HID_ITEM_LONG                              0xFEU

#define  HID_ITEM_TYPE_MAIN                         0x00U
#define  HID_ITEM_TYPE_GLOBAL                       0x01U
#define  HID_ITEM_TYPE_LOCAL                        0x02U
#define  HID_ITEM_TYPE_RESERVED                     0x03U


#define  HID_MAIN_ITEM_TAG_INPUT                    0x08U
#define  HID_MAIN_ITEM_TAG_OUTPUT                   0x09U
#define  HID_MAIN_ITEM_TAG_COLLECTION               0x0AU
#define  HID_MAIN_ITEM_TAG_FEATURE                  0x0BU
#define  HID_MAIN_ITEM_TAG_ENDCOLLECTION            0x0CU


#define  HID_GLOBAL_ITEM_TAG_USAGE_PAGE             0x00U
#define  HID_GLOBAL_ITEM_TAG_LOG_MIN                0x01U
#define  HID_GLOBAL_ITEM_TAG_LOG_MAX                0x02U
#define  HID_GLOBAL_ITEM_TAG_PHY_MIN                0x03U
#define  HID_GLOBAL_ITEM_TAG_PHY_MAX                0x04U
#define  HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT          0x05U
#define  HID_GLOBAL_ITEM_TAG_UNIT                   0x06U
#define  HID_GLOBAL_ITEM_TAG_REPORT_SIZE            0x07U
#define  HID_GLOBAL_ITEM_TAG_REPORT_ID              0x08U
#define  HID_GLOBAL_ITEM_TAG_REPORT_COUNT           0x09U
#define  HID_GLOBAL_ITEM_TAG_PUSH                   0x0AU
#define  HID_GLOBAL_ITEM_TAG_POP                    0x0BU


#define  HID_LOCAL_ITEM_TAG_USAGE                   0x00U
#define  HID_LOCAL_ITEM_TAG_USAGE_MIN               0x01U
#define  HID_LOCAL_ITEM_TAG_USAGE_MAX               0x02U
#define  HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX        0x03U
#define  HID_LOCAL_ITEM_TAG_DESIGNATOR_MIN          0x04U
#define  HID_LOCAL_ITEM_TAG_DESIGNATOR_MAX          0x05U
#define  HID_LOCAL_ITEM_TAG_STRING_INDEX            0x07U
#define  HID_LOCAL_ITEM_TAG_STRING_MIN              0x08U
#define  HID_LOCAL_ITEM_TAG_STRING_MAX              0x09U
#define  HID_LOCAL_ITEM_TAG_DELIMITER               0x0AU
