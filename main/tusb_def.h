/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/**
 * usb_def.h
 *    common usb definitions
 */

#pragma once

#include <stdint.h>

#define TU_STRING(x)      #x              ///< stringify without expand
#define TU_XSTRING(x)     TU_STRING(x)    ///< expand then stringify
#define TU_STRCAT(a, b)   a##b            ///< concat without expand
#define TU_XSTRCAT(a, b)  TU_STRCAT(a, b) ///< expand then concat

#ifndef __COUNTER__
  #define _TU_COUNTER_ __LINE__
#else
  #define _TU_COUNTER_ __COUNTER__
#endif

// Compile-time Assert
#if __STDC_VERSION__ >= 201112L
  #define TU_VERIFY_STATIC   _Static_assert
#else
  #define TU_VERIFY_STATIC(const_expr, _mess) enum { TU_XSTRCAT(_verify_static_, _TU_COUNTER_) = 1/(!!(const_expr)) }
#endif

//--------------------------------------------------------------------+
// Compiler porting with Attribute and Endian
//--------------------------------------------------------------------+
#define TU_ATTR_ALIGNED(Bytes)        __attribute__ ((aligned(Bytes)))
#define TU_ATTR_SECTION(sec_name)     __attribute__ ((section(#sec_name)))
#define TU_ATTR_PACKED                __attribute__ ((packed))
#define TU_ATTR_PREPACKED
#define TU_ATTR_WEAK                  __attribute__ ((weak))
#define TU_ATTR_DEPRECATED(mess)      __attribute__ ((deprecated(mess))) // warn if function with this attribute is used
#define TU_ATTR_UNUSED                __attribute__ ((unused))           // Function/Variable is meant to be possibly unused
#define TU_ATTR_USED                  __attribute__ ((used))             // Function/Variable is meant to be used

//--------------------------------------------------------------------+
// Macros Helper
//--------------------------------------------------------------------+
#define TU_ARRAY_SIZE(_arr)   ( sizeof(_arr) / sizeof(_arr[0]) )
#define TU_MIN(_x, _y)        ( ( (_x) < (_y) ) ? (_x) : (_y) )
#define TU_MAX(_x, _y)        ( ( (_x) > (_y) ) ? (_x) : (_y) )

#define TU_U16_HIGH(u16)      ((uint8_t) (((u16) >> 8) & 0x00ff))
#define TU_U16_LOW(u16)       ((uint8_t) ((u16)       & 0x00ff))
#define U16_TO_U8S_BE(u16)    TU_U16_HIGH(u16), TU_U16_LOW(u16)
#define U16_TO_U8S_LE(u16)    TU_U16_LOW(u16), TU_U16_HIGH(u16)

#define U32_B1_U8(u32)        ((uint8_t) (((u32) >> 24) & 0x000000ff)) // MSB
#define U32_B2_U8(u32)        ((uint8_t) (((u32) >> 16) & 0x000000ff))
#define U32_B3_U8(u32)        ((uint8_t) (((u32) >>  8) & 0x000000ff))
#define U32_B4_U8(u32)        ((uint8_t) ((u32)         & 0x000000ff)) // LSB

#define U32_TO_U8S_BE(u32)    U32_B1_U8(u32), U32_B2_U8(u32), U32_B3_U8(u32), U32_B4_U8(u32)
#define U32_TO_U8S_LE(u32)    U32_B4_U8(u32), U32_B3_U8(u32), U32_B2_U8(u32), U32_B1_U8(u32)

#define TU_BIT(n)             (1U << (n))
#define TU_BIT(n)             (1U << (n))


/*------------------------------------------------------------------*/
/* Count number of arguments of __VA_ARGS__
 * - reference https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
 * - _GET_NTH_ARG() takes args >= N (64) but only expand to Nth one (64th)
 * - _RSEQ_N() is reverse sequential to N to add padding to have
 * Nth position is the same as the number of arguments
 * - ##__VA_ARGS__ is used to deal with 0 paramerter (swallows comma)
 *------------------------------------------------------------------*/
#ifndef TU_ARGS_NUM

#define TU_ARGS_NUM(...) 	 NARG_(_0, ##__VA_ARGS__,_RSEQ_N())

#define NARG_(...)        _GET_NTH_ARG(__VA_ARGS__)
#define _GET_NTH_ARG( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define _RSEQ_N() \
         62,61,60,                      \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0
#endif

/*------------------------------------------------------------------*/
/* CONSTANTS
 *------------------------------------------------------------------*/

/// defined base on EHCI specs value for Endpoint Speed
typedef enum
{
  TUSB_SPEED_FULL = 0,
  TUSB_SPEED_LOW     ,
  TUSB_SPEED_HIGH
}tusb_speed_t;

/// defined base on USB Specs Endpoint's bmAttributes
typedef enum
{
  TUSB_XFER_CONTROL = 0 ,
  TUSB_XFER_ISOCHRONOUS ,
  TUSB_XFER_BULK        ,
  TUSB_XFER_INTERRUPT
}tusb_xfer_type_t;

typedef enum
{
  TUSB_DIR_OUT = 0,
  TUSB_DIR_IN  = 1,

  TUSB_DIR_IN_MASK = 0x80
}tusb_dir_t;

/// USB Descriptor Types
typedef enum
{
  TUSB_DESC_DEVICE                = 0x01,
  TUSB_DESC_CONFIGURATION         = 0x02,
  TUSB_DESC_STRING                = 0x03,
  TUSB_DESC_INTERFACE             = 0x04,
  TUSB_DESC_ENDPOINT              = 0x05,
  TUSB_DESC_DEVICE_QUALIFIER      = 0x06,
  TUSB_DESC_OTHER_SPEED_CONFIG    = 0x07,
  TUSB_DESC_INTERFACE_POWER       = 0x08,
  TUSB_DESC_OTG                   = 0x09,
  TUSB_DESC_DEBUG                 = 0x0A,
  TUSB_DESC_INTERFACE_ASSOCIATION = 0x0B,

  TUSB_DESC_BOS                   = 0x0F,
  TUSB_DESC_DEVICE_CAPABILITY     = 0x10,

  TUSB_DESC_FUNCTIONAL            = 0x21,

  // Class Specific Descriptor
  TUSB_DESC_CS_DEVICE             = 0x21,
  TUSB_DESC_CS_CONFIGURATION      = 0x22,
  TUSB_DESC_CS_STRING             = 0x23,
  TUSB_DESC_CS_INTERFACE          = 0x24,
  TUSB_DESC_CS_ENDPOINT           = 0x25,

  TUSB_DESC_SUPERSPEED_ENDPOINT_COMPANION     = 0x30,
  TUSB_DESC_SUPERSPEED_ISO_ENDPOINT_COMPANION = 0x31
}tusb_desc_type_t;

typedef enum
{
  TUSB_REQ_GET_STATUS        = 0  ,
  TUSB_REQ_CLEAR_FEATURE     = 1  ,
  TUSB_REQ_RESERVED          = 2  ,
  TUSB_REQ_SET_FEATURE       = 3  ,
  TUSB_REQ_RESERVED2         = 4  ,
  TUSB_REQ_SET_ADDRESS       = 5  ,
  TUSB_REQ_GET_DESCRIPTOR    = 6  ,
  TUSB_REQ_SET_DESCRIPTOR    = 7  ,
  TUSB_REQ_GET_CONFIGURATION = 8  ,
  TUSB_REQ_SET_CONFIGURATION = 9  ,
  TUSB_REQ_GET_INTERFACE     = 10 ,
  TUSB_REQ_SET_INTERFACE     = 11 ,
  TUSB_REQ_SYNCH_FRAME       = 12
}tusb_request_code_t;

typedef enum
{
  TUSB_REQ_FEATURE_EDPT_HALT     = 0,
  TUSB_REQ_FEATURE_REMOTE_WAKEUP = 1,
  TUSB_REQ_FEATURE_TEST_MODE     = 2
}tusb_request_feature_selector_t;

typedef enum
{
  TUSB_REQ_TYPE_STANDARD = 0,
  TUSB_REQ_TYPE_CLASS,
  TUSB_REQ_TYPE_VENDOR,
  TUSB_REQ_TYPE_INVALID
} tusb_request_type_t;

typedef enum
{
  TUSB_REQ_RCPT_DEVICE =0,
  TUSB_REQ_RCPT_INTERFACE,
  TUSB_REQ_RCPT_ENDPOINT,
  TUSB_REQ_RCPT_OTHER
} tusb_request_recipient_t;

// https://www.usb.org/defined-class-codes
typedef enum
{
  TUSB_CLASS_UNSPECIFIED          = 0    ,
  TUSB_CLASS_AUDIO                = 1    ,
  TUSB_CLASS_CDC                  = 2    ,
  TUSB_CLASS_HID                  = 3    ,
  TUSB_CLASS_RESERVED_4           = 4    ,
  TUSB_CLASS_PHYSICAL             = 5    ,
  TUSB_CLASS_IMAGE                = 6    ,
  TUSB_CLASS_PRINTER              = 7    ,
  TUSB_CLASS_MSC                  = 8    ,
  TUSB_CLASS_HUB                  = 9    ,
  TUSB_CLASS_CDC_DATA             = 10   ,
  TUSB_CLASS_SMART_CARD           = 11   ,
  TUSB_CLASS_RESERVED_12          = 12   ,
  TUSB_CLASS_CONTENT_SECURITY     = 13   ,
  TUSB_CLASS_VIDEO                = 14   ,
  TUSB_CLASS_PERSONAL_HEALTHCARE  = 15   ,
  TUSB_CLASS_AUDIO_VIDEO          = 16   ,

  TUSB_CLASS_DIAGNOSTIC           = 0xDC ,
  TUSB_CLASS_WIRELESS_CONTROLLER  = 0xE0 ,
  TUSB_CLASS_MISC                 = 0xEF ,
  TUSB_CLASS_APPLICATION_SPECIFIC = 0xFE ,
  TUSB_CLASS_VENDOR_SPECIFIC      = 0xFF
}tusb_class_code_t;

typedef enum
{
  MISC_SUBCLASS_COMMON = 2
}misc_subclass_type_t;

typedef enum
{
  MISC_PROTOCOL_IAD = 1
}misc_protocol_type_t;

typedef enum
{
  APP_SUBCLASS_USBTMC = 0x03,
  APP_SUBCLASS_DFU_RUNTIME = 0x01
} app_subclass_type_t;

typedef enum
{
  DEVICE_CAPABILITY_WIRELESS_USB               = 0x01,
  DEVICE_CAPABILITY_USB20_EXTENSION            = 0x02,
  DEVICE_CAPABILITY_SUPERSPEED_USB             = 0x03,
  DEVICE_CAPABILITY_CONTAINER_id               = 0x04,
  DEVICE_CAPABILITY_PLATFORM                   = 0x05,
  DEVICE_CAPABILITY_POWER_DELIVERY             = 0x06,
  DEVICE_CAPABILITY_BATTERY_INFO               = 0x07,
  DEVICE_CAPABILITY_PD_CONSUMER_PORT           = 0x08,
  DEVICE_CAPABILITY_PD_PROVIDER_PORT           = 0x09,
  DEVICE_CAPABILITY_SUPERSPEED_PLUS            = 0x0A,
  DEVICE_CAPABILITY_PRECESION_TIME_MEASUREMENT = 0x0B,
  DEVICE_CAPABILITY_WIRELESS_USB_EXT           = 0x0C,
  DEVICE_CAPABILITY_BILLBOARD                  = 0x0D,
  DEVICE_CAPABILITY_AUTHENTICATION             = 0x0E,
  DEVICE_CAPABILITY_BILLBOARD_EX               = 0x0F,
  DEVICE_CAPABILITY_CONFIGURATION_SUMMARY      = 0x10
}device_capability_type_t;

enum {
  TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP = TU_BIT(5),
  TUSB_DESC_CONFIG_ATT_SELF_POWERED  = TU_BIT(6),
};

#define TUSB_DESC_CONFIG_POWER_MA(x)  ((x)/2)

enum
{
  INTERFACE_INVALID_NUMBER = 0xff
};


typedef enum
{
  MS_OS_20_SET_HEADER_DESCRIPTOR       = 0x00,
  MS_OS_20_SUBSET_HEADER_CONFIGURATION = 0x01,
  MS_OS_20_SUBSET_HEADER_FUNCTION      = 0x02,
  MS_OS_20_FEATURE_COMPATBLE_ID        = 0x03,
  MS_OS_20_FEATURE_REG_PROPERTY        = 0x04,
  MS_OS_20_FEATURE_MIN_RESUME_TIME     = 0x05,
  MS_OS_20_FEATURE_MODEL_ID            = 0x06,
  MS_OS_20_FEATURE_CCGP_DEVICE         = 0x07,
  MS_OS_20_FEATURE_VENDOR_REVISION     = 0x08
} microsoft_os_20_type_t;

//--------------------------------------------------------------------+
// USB Descriptors
//--------------------------------------------------------------------+

/// USB Device Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength            ; ///< Size of this descriptor in bytes.
  uint8_t  bDescriptorType    ; ///< DEVICE Descriptor Type.
  uint16_t bcdUSB             ; ///< BUSB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H). This field identifies the release of the USB Specification with which the device and its descriptors are compliant.

  uint8_t  bDeviceClass       ; ///< Class code (assigned by the USB-IF). \li If this field is reset to zero, each interface within a configuration specifies its own class information and the various interfaces operate independently. \li If this field is set to a value between 1 and FEH, the device supports different class specifications on different interfaces and the interfaces may not operate independently. This value identifies the class definition used for the aggregate interfaces. \li If this field is set to FFH, the device class is vendor-specific.
  uint8_t  bDeviceSubClass    ; ///< Subclass code (assigned by the USB-IF). These codes are qualified by the value of the bDeviceClass field. \li If the bDeviceClass field is reset to zero, this field must also be reset to zero. \li If the bDeviceClass field is not set to FFH, all values are reserved for assignment by the USB-IF.
  uint8_t  bDeviceProtocol    ; ///< Protocol code (assigned by the USB-IF). These codes are qualified by the value of the bDeviceClass and the bDeviceSubClass fields. If a device supports class-specific protocols on a device basis as opposed to an interface basis, this code identifies the protocols that the device uses as defined by the specification of the device class. \li If this field is reset to zero, the device does not use class-specific protocols on a device basis. However, it may use classspecific protocols on an interface basis. \li If this field is set to FFH, the device uses a vendor-specific protocol on a device basis.
  uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid). For HS devices is fixed to 64.

  uint16_t idVendor           ; ///< Vendor ID (assigned by the USB-IF).
  uint16_t idProduct          ; ///< Product ID (assigned by the manufacturer).
  uint16_t bcdDevice          ; ///< Device release number in binary-coded decimal.
  uint8_t  iManufacturer      ; ///< Index of string descriptor describing manufacturer.
  uint8_t  iProduct           ; ///< Index of string descriptor describing product.
  uint8_t  iSerialNumber      ; ///< Index of string descriptor describing the device's serial number.

  uint8_t  bNumConfigurations ; ///< Number of possible configurations.
} tusb_desc_device_t;

// USB Binary Device Object Store (BOS) Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength         ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType ; ///< CONFIGURATION Descriptor Type
  uint16_t wTotalLength    ; ///< Total length of data returned for this descriptor
  uint8_t  bNumDeviceCaps  ; ///< Number of device capability descriptors in the BOS
} tusb_desc_bos_t;

/// USB Configuration Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength             ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType     ; ///< CONFIGURATION Descriptor Type
  uint16_t wTotalLength        ; ///< Total length of data returned for this configuration. Includes the combined length of all descriptors (configuration, interface, endpoint, and class- or vendor-specific) returned for this configuration.

  uint8_t  bNumInterfaces      ; ///< Number of interfaces supported by this configuration
  uint8_t  bConfigurationValue ; ///< Value to use as an argument to the SetConfiguration() request to select this configuration.
  uint8_t  iConfiguration      ; ///< Index of string descriptor describing this configuration
  uint8_t  bmAttributes        ; ///< Configuration characteristics \n D7: Reserved (set to one)\n D6: Self-powered \n D5: Remote Wakeup \n D4...0: Reserved (reset to zero) \n D7 is reserved and must be set to one for historical reasons. \n A device configuration that uses power from the bus and a local source reports a non-zero value in bMaxPower to indicate the amount of bus power required and sets D6. The actual power source at runtime may be determined using the GetStatus(DEVICE) request (see USB 2.0 spec Section 9.4.5). \n If a device configuration supports remote wakeup, D5 is set to one.
  uint8_t  bMaxPower           ; ///< Maximum power consumption of the USB device from the bus in this specific configuration when the device is fully operational. Expressed in 2 mA units (i.e., 50 = 100 mA).
} tusb_desc_configuration_t;

/// USB Interface Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength            ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType    ; ///< INTERFACE Descriptor Type

  uint8_t  bInterfaceNumber   ; ///< Number of this interface. Zero-based value identifying the index in the array of concurrent interfaces supported by this configuration.
  uint8_t  bAlternateSetting  ; ///< Value used to select this alternate setting for the interface identified in the prior field
  uint8_t  bNumEndpoints      ; ///< Number of endpoints used by this interface (excluding endpoint zero). If this value is zero, this interface only uses the Default Control Pipe.
  uint8_t  bInterfaceClass    ; ///< Class code (assigned by the USB-IF). \li A value of zero is reserved for future standardization. \li If this field is set to FFH, the interface class is vendor-specific. \li All other values are reserved for assignment by the USB-IF.
  uint8_t  bInterfaceSubClass ; ///< Subclass code (assigned by the USB-IF). \n These codes are qualified by the value of the bInterfaceClass field. \li If the bInterfaceClass field is reset to zero, this field must also be reset to zero. \li If the bInterfaceClass field is not set to FFH, all values are reserved for assignment by the USB-IF.
  uint8_t  bInterfaceProtocol ; ///< Protocol code (assigned by the USB). \n These codes are qualified by the value of the bInterfaceClass and the bInterfaceSubClass fields. If an interface supports class-specific requests, this code identifies the protocols that the device uses as defined by the specification of the device class. \li If this field is reset to zero, the device does not use a class-specific protocol on this interface. \li If this field is set to FFH, the device uses a vendor-specific protocol for this interface.
  uint8_t  iInterface         ; ///< Index of string descriptor describing this interface
} tusb_desc_interface_t;

/// USB Endpoint Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength          ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType  ; ///< ENDPOINT Descriptor Type

  uint8_t  bEndpointAddress ; ///< The address of the endpoint on the USB device described by this descriptor. The address is encoded as follows: \n Bit 3...0: The endpoint number \n Bit 6...4: Reserved, reset to zero \n Bit 7: Direction, ignored for control endpoints 0 = OUT endpoint 1 = IN endpoint.

  struct TU_ATTR_PACKED {
    uint8_t xfer  : 2;
    uint8_t sync  : 2;
    uint8_t usage : 2;
    uint8_t       : 2;
  } bmAttributes     ; ///< This field describes the endpoint's attributes when it is configured using the bConfigurationValue. \n Bits 1..0: Transfer Type \n- 00 = Control \n- 01 = Isochronous \n- 10 = Bulk \n- 11 = Interrupt \n If not an isochronous endpoint, bits 5..2 are reserved and must be set to zero. If isochronous, they are defined as follows: \n Bits 3..2: Synchronization Type \n- 00 = No Synchronization \n- 01 = Asynchronous \n- 10 = Adaptive \n- 11 = Synchronous \n Bits 5..4: Usage Type \n- 00 = Data endpoint \n- 01 = Feedback endpoint \n- 10 = Implicit feedback Data endpoint \n- 11 = Reserved \n Refer to Chapter 5 of USB 2.0 specification for more information. \n All other bits are reserved and must be reset to zero. Reserved bits must be ignored by the host.

  struct TU_ATTR_PACKED {
    uint16_t size           : 11; ///< Maximum packet size this endpoint is capable of sending or receiving when this configuration is selected. \n For isochronous endpoints, this value is used to reserve the bus time in the schedule, required for the per-(micro)frame data payloads. The pipe may, on an ongoing basis, actually use less bandwidth than that reserved. The device reports, if necessary, the actual bandwidth used via its normal, non-USB defined mechanisms. \n For all endpoints, bits 10..0 specify the maximum packet size (in bytes). \n For high-speed isochronous and interrupt endpoints: \n Bits 12..11 specify the number of additional transaction opportunities per microframe: \n- 00 = None (1 transaction per microframe) \n- 01 = 1 additional (2 per microframe) \n- 10 = 2 additional (3 per microframe) \n- 11 = Reserved \n Bits 15..13 are reserved and must be set to zero.
    uint16_t hs_period_mult : 2;
    uint16_t : 0;
  }wMaxPacketSize;

  uint8_t  bInterval        ; ///< Interval for polling endpoint for data transfers. Expressed in frames or microframes depending on the device operating speed (i.e., either 1 millisecond or 125 us units). \n- For full-/high-speed isochronous endpoints, this value must be in the range from 1 to 16. The bInterval value is used as the exponent for a \f$ 2^(bInterval-1) \f$ value; e.g., a bInterval of 4 means a period of 8 (\f$ 2^(4-1) \f$). \n- For full-/low-speed interrupt endpoints, the value of this field may be from 1 to 255. \n- For high-speed interrupt endpoints, the bInterval value is used as the exponent for a \f$ 2^(bInterval-1) \f$ value; e.g., a bInterval of 4 means a period of 8 (\f$ 2^(4-1) \f$) . This value must be from 1 to 16. \n- For high-speed bulk/control OUT endpoints, the bInterval must specify the maximum NAK rate of the endpoint. A value of 0 indicates the endpoint never NAKs. Other values indicate at most 1 NAK each bInterval number of microframes. This value must be in the range from 0 to 255. \n Refer to Chapter 5 of USB 2.0 specification for more information.
} tusb_desc_endpoint_t;

/// USB Other Speed Configuration Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength             ; ///< Size of descriptor
  uint8_t  bDescriptorType     ; ///< Other_speed_Configuration Type
  uint16_t wTotalLength        ; ///< Total length of data returned

  uint8_t  bNumInterfaces      ; ///< Number of interfaces supported by this speed configuration
  uint8_t  bConfigurationValue ; ///< Value to use to select configuration
  uint8_t  IConfiguration      ; ///< Index of string descriptor
  uint8_t  bmAttributes        ; ///< Same as Configuration descriptor
  uint8_t  bMaxPower           ; ///< Same as Configuration descriptor
} tusb_desc_other_speed_t;

/// USB Device Qualifier Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength            ; ///< Size of descriptor
  uint8_t  bDescriptorType    ; ///< Device Qualifier Type
  uint16_t bcdUSB             ; ///< USB specification version number (e.g., 0200H for V2.00)

  uint8_t  bDeviceClass       ; ///< Class Code
  uint8_t  bDeviceSubClass    ; ///< SubClass Code
  uint8_t  bDeviceProtocol    ; ///< Protocol Code
  uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for other speed
  uint8_t  bNumConfigurations ; ///< Number of Other-speed Configurations
  uint8_t  bReserved          ; ///< Reserved for future use, must be zero
} tusb_desc_device_qualifier_t;

/// USB Interface Association Descriptor (IAD ECN)
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength           ; ///< Size of descriptor
  uint8_t bDescriptorType   ; ///< Other_speed_Configuration Type

  uint8_t bFirstInterface   ; ///< Index of the first associated interface.
  uint8_t bInterfaceCount   ; ///< Total number of associated interfaces.

  uint8_t bFunctionClass    ; ///< Interface class ID.
  uint8_t bFunctionSubClass ; ///< Interface subclass ID.
  uint8_t bFunctionProtocol ; ///< Interface protocol ID.

  uint8_t iFunction         ; ///< Index of the string descriptor describing the interface association.
} tusb_desc_interface_assoc_t;

// USB String Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength         ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType ; ///< Descriptor Type
  uint16_t unicode_string[];
} tusb_desc_string_t;

// USB Binary Device Object Store (BOS)
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength;
  uint8_t bDescriptorType ;
  uint8_t bDevCapabilityType;
  uint8_t bReserved;
  uint8_t PlatformCapabilityUUID[16];
  uint8_t CapabilityData[];
} tusb_desc_bos_platform_t;

// USB WebuSB URL Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bScheme;
  char    url[];
} tusb_desc_webusb_url_t;

/*------------------------------------------------------------------*/
/* Types
 *------------------------------------------------------------------*/
typedef struct TU_ATTR_PACKED{
  union {
    struct TU_ATTR_PACKED {
      uint8_t recipient :  5; ///< Recipient type tusb_request_recipient_t.
      uint8_t type      :  2; ///< Request type tusb_request_type_t.
      uint8_t direction :  1; ///< Direction type. tusb_dir_t
    } bmRequestType_bit;

    uint8_t bmRequestType;
  };

  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} tusb_control_request_t;

TU_VERIFY_STATIC( sizeof(tusb_control_request_t) == 8, "mostly compiler option issue");

//--------------------------------------------------------------------+
// Binary Device Object Store (BOS) Descriptor Templates
//--------------------------------------------------------------------+

#define TUD_BOS_DESC_LEN      5

// total length, number of device caps
#define TUD_BOS_DESCRIPTOR(_total_len, _caps_num) \
  5, TUSB_DESC_BOS, U16_TO_U8S_LE(_total_len), _caps_num

// Device Capability Platform 128-bit UUID + Data
#define TUD_BOS_PLATFORM_DESCRIPTOR(...) \
  4+TU_ARGS_NUM(__VA_ARGS__), TUSB_DESC_DEVICE_CAPABILITY, DEVICE_CAPABILITY_PLATFORM, 0x00, __VA_ARGS__

//------------- WebUSB BOS Platform -------------//

// Descriptor Length
#define TUD_BOS_WEBUSB_DESC_LEN         24

// Vendor Code, iLandingPage
#define TUD_BOS_WEBUSB_DESCRIPTOR(_vendor_code, _ipage) \
  TUD_BOS_PLATFORM_DESCRIPTOR(TUD_BOS_WEBUSB_UUID, U16_TO_U8S_LE(0x0100), _vendor_code, _ipage)

#define TUD_BOS_WEBUSB_UUID   \
  0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47, \
  0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65

//------------- Microsoft OS 2.0 Platform -------------//
#define TUD_BOS_MICROSOFT_OS_DESC_LEN   28

// Total Length of descriptor set, vendor code
#define TUD_BOS_MS_OS_20_DESCRIPTOR(_desc_set_len, _vendor_code) \
  TUD_BOS_PLATFORM_DESCRIPTOR(TUD_BOS_MS_OS_20_UUID, U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(_desc_set_len), _vendor_code, 0)

#define TUD_BOS_MS_OS_20_UUID \
  0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C, \
  0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F

//--------------------------------------------------------------------+
// Configuration & Interface Descriptor Templates
//--------------------------------------------------------------------+

//------------- Configuration -------------//
#define TUD_CONFIG_DESC_LEN   (9)

// Config number, interface count, string index, total length, attribute, power in mA
#define TUD_CONFIG_DESCRIPTOR(config_num, _itfcount, _stridx, _total_len, _attribute, _power_ma) \
  9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(_total_len), _itfcount, config_num, _stridx, TU_BIT(7) | _attribute, (_power_ma)/2

//------------- CDC -------------//

// Length of template descriptor: 66 bytes
#define TUD_CDC_DESC_LEN  (8+9+5+5+4+5+7+9+7+7)

// CDC Descriptor Template
// Interface number, string index, EP notification address and size, EP data address (out, in) and size.
#define TUD_CDC_DESCRIPTOR(_itfnum, _stridx, _ep_notif, _ep_notif_size, _epout, _epin, _epsize) \
  /* Interface Associate */\
  8, TUSB_DESC_INTERFACE_ASSOCIATION, _itfnum, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, 0,\
  /* CDC Control Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, _stridx,\
  /* CDC Header */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),\
  /* CDC Call */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0, (uint8_t)((_itfnum) + 1),\
  /* CDC ACM: support line request */\
  4, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 2,\
  /* CDC Union */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, _itfnum, (uint8_t)((_itfnum) + 1),\
  /* Endpoint Notification */\
  7, TUSB_DESC_ENDPOINT, _ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_notif_size), 16,\
  /* CDC Data Interface */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 0, 2, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0

//------------- MSC -------------//

// Length of template descriptor: 23 bytes
#define TUD_MSC_DESC_LEN    (9 + 7 + 7)

// Interface number, string index, EP Out & EP In address, EP size
#define TUD_MSC_DESCRIPTOR(_itfnum, _stridx, _epout, _epin, _epsize) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 2, TUSB_CLASS_MSC, MSC_SUBCLASS_SCSI, MSC_PROTOCOL_BOT, _stridx,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0

//------------- HID -------------//

// Length of template descriptor: 25 bytes
#define TUD_HID_DESC_LEN    (9 + 9 + 7)

// HID Input only descriptor
// Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
#define TUD_HID_DESCRIPTOR(_itfnum, _stridx, _boot_protocol, _report_desc_len, _epin, _epsize, _ep_interval) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 1, TUSB_CLASS_HID, (uint8_t)((_boot_protocol) ? HID_SUBCLASS_BOOT : 0), _boot_protocol, _stridx,\
  /* HID descriptor */\
  9, HID_DESC_TYPE_HID, U16_TO_U8S_LE(0x0111), 0, 1, HID_DESC_TYPE_REPORT, U16_TO_U8S_LE(_report_desc_len),\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_epsize), _ep_interval

// Length of template descriptor: 32 bytes
#define TUD_HID_INOUT_DESC_LEN    (9 + 9 + 7 + 7)

// HID Input & Output descriptor
// Interface number, string index, protocol, report descriptor len, EP OUT & IN address, size & polling interval
#define TUD_HID_INOUT_DESCRIPTOR(_itfnum, _stridx, _boot_protocol, _report_desc_len, _epout, _epin, _epsize, _ep_interval) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 2, TUSB_CLASS_HID, (uint8_t)((_boot_protocol) ? HID_SUBCLASS_BOOT : 0), _boot_protocol, _stridx,\
  /* HID descriptor */\
  9, HID_DESC_TYPE_HID, U16_TO_U8S_LE(0x0111), 0, 1, HID_DESC_TYPE_REPORT, U16_TO_U8S_LE(_report_desc_len),\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_epsize), _ep_interval, \
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_epsize), _ep_interval

//------------- MIDI -------------//

#define TUD_MIDI_DESC_HEAD_LEN (9 + 9 + 9 + 7)
#define TUD_MIDI_DESC_HEAD(_itfnum,  _stridx, _numcables) \
  /* Audio Control (AC) Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 0, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_CONTROL, AUDIO_PROTOCOL_V1, _stridx,\
  /* AC Header */\
  9, TUSB_DESC_CS_INTERFACE, AUDIO_CS_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(0x0009), 1, (uint8_t)((_itfnum) + 1),\
  /* MIDI Streaming (MS) Interface */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum) + 1), 0, 2, TUSB_CLASS_AUDIO, AUDIO_SUBCLASS_MIDI_STREAMING, AUDIO_PROTOCOL_V1, 0,\
  /* MS Header */\
  7, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_HEADER, U16_TO_U8S_LE(0x0100), U16_TO_U8S_LE(7 + (_numcables) * TUD_MIDI_DESC_JACK_LEN)

#define TUD_MIDI_JACKID_IN_EMB(_cablenum) \
  (uint8_t)(((_cablenum) - 1) * 4 + 1)

#define TUD_MIDI_JACKID_IN_EXT(_cablenum) \
  (uint8_t)(((_cablenum) - 1) * 4 + 2)

#define TUD_MIDI_JACKID_OUT_EMB(_cablenum) \
  (uint8_t)(((_cablenum) - 1) * 4 + 3)

#define TUD_MIDI_JACKID_OUT_EXT(_cablenum) \
  (uint8_t)(((_cablenum) - 1) * 4 + 4)

#define TUD_MIDI_DESC_JACK_LEN (6 + 6 + 9 + 9)
#define TUD_MIDI_DESC_JACK(_cablenum) \
  /* MS In Jack (Embedded) */\
  6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_IN_EMB(_cablenum), 0,\
  /* MS In Jack (External) */\
  6, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_IN_JACK, MIDI_JACK_EXTERNAL, TUD_MIDI_JACKID_IN_EXT(_cablenum), 0,\
  /* MS Out Jack (Embedded), connected to In Jack External */\
  9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EMBEDDED, TUD_MIDI_JACKID_OUT_EMB(_cablenum), 1, TUD_MIDI_JACKID_IN_EXT(_cablenum), 1, 0,\
  /* MS Out Jack (External), connected to In Jack Embedded */\
  9, TUSB_DESC_CS_INTERFACE, MIDI_CS_INTERFACE_OUT_JACK, MIDI_JACK_EXTERNAL, TUD_MIDI_JACKID_OUT_EXT(_cablenum), 1, TUD_MIDI_JACKID_IN_EMB(_cablenum), 1, 0

#define TUD_MIDI_DESC_EP_LEN(_numcables) (7 + 4 + (_numcables))
#define TUD_MIDI_DESC_EP(_epout, _epsize, _numcables) \
  /* Endpoint */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* MS Endpoint (connected to embedded jack) */\
  (uint8_t)(4 + (_numcables)), TUSB_DESC_CS_ENDPOINT, MIDI_CS_ENDPOINT_GENERAL, _numcables

// Length of template descriptor (88 bytes)
#define TUD_MIDI_DESC_LEN (TUD_MIDI_DESC_HEAD_LEN + TUD_MIDI_DESC_JACK_LEN + TUD_MIDI_DESC_EP_LEN(1) * 2)

// MIDI simple descriptor
// - 1 Embedded Jack In connected to 1 External Jack Out
// - 1 Embedded Jack out connected to 1 External Jack In
#define TUD_MIDI_DESCRIPTOR(_itfnum, _stridx, _epout, _epin, _epsize) \
  TUD_MIDI_DESC_HEAD(_itfnum, _stridx, 1),\
  TUD_MIDI_DESC_JACK(1),\
  TUD_MIDI_DESC_EP(_epout, _epsize, 1),\
  TUD_MIDI_JACKID_IN_EMB(1),\
  TUD_MIDI_DESC_EP(_epin, _epsize, 1),\
  TUD_MIDI_JACKID_OUT_EMB(1)


//------------- TUD_USBTMC/USB488 -------------//
#define TUD_USBTMC_APP_CLASS    (TUSB_CLASS_APPLICATION_SPECIFIC)
#define TUD_USBTMC_APP_SUBCLASS 0x03u

#define TUD_USBTMC_PROTOCOL_STD    0x00u
#define TUD_USBTMC_PROTOCOL_USB488 0x01u

//   Interface number, number of endpoints, EP string index, USB_TMC_PROTOCOL*, bulk-out endpoint ID,
//   bulk-in endpoint ID
#define TUD_USBTMC_IF_DESCRIPTOR(_itfnum, _bNumEndpoints, _stridx, _itfProtocol) \
  /* Interface */ \
  0x09, TUSB_DESC_INTERFACE, _itfnum, 0x00, _bNumEndpoints, TUD_USBTMC_APP_CLASS, TUD_USBTMC_APP_SUBCLASS, _itfProtocol, _stridx

#define TUD_USBTMC_IF_DESCRIPTOR_LEN 9u

#define TUD_USBTMC_BULK_DESCRIPTORS(_epout, _epin, _bulk_epsize) \
  /* Endpoint Out */ \
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_bulk_epsize), 0u, \
  /* Endpoint In */ \
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_bulk_epsize), 0u

#define TUD_USBTMC_BULK_DESCRIPTORS_LEN (7u+7u)

/* optional interrupt endpoint */ \
// _int_pollingInterval : for LS/FS, expressed in frames (1ms each). 16 may be a good number?
#define TUD_USBTMC_INT_DESCRIPTOR(_ep_interrupt, _ep_interrupt_size, _int_pollingInterval ) \
  7, TUSB_DESC_ENDPOINT, _ep_interrupt, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_interrupt_size), 0x16

#define TUD_USBTMC_INT_DESCRIPTOR_LEN (7u)


//------------- Vendor -------------//
#define TUD_VENDOR_DESC_LEN  (9+7+7)

// Interface number, string index, EP Out & IN address, EP size
#define TUD_VENDOR_DESCRIPTOR(_itfnum, _stridx, _epout, _epin, _epsize) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0x00, 0x00, _stridx,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0

//------------- DFU Runtime -------------//
#define TUD_DFU_APP_CLASS    (TUSB_CLASS_APPLICATION_SPECIFIC)
#define TUD_DFU_APP_SUBCLASS 0x01u

// Length of template descriptr: 18 bytes
#define TUD_DFU_RT_DESC_LEN (9 + 9)

// DFU runtime descriptor
// Interface number, string index, attributes, detach timeout, transfer size
#define TUD_DFU_RT_DESCRIPTOR(_itfnum, _stridx, _attr, _timeout, _xfer_size) \
  /* Interface */ \
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 0, TUD_DFU_APP_CLASS, TUD_DFU_APP_SUBCLASS, DFU_PROTOCOL_RT, _stridx, \
  /* Function */ \
  9, DFU_DESC_FUNCTIONAL, _attr, U16_TO_U8S_LE(_timeout), U16_TO_U8S_LE(_xfer_size), U16_TO_U8S_LE(0x0101)


//------------- CDC-ECM -------------//

// Length of template descriptor: 71 bytes
#define TUD_CDC_ECM_DESC_LEN  (8+9+5+5+13+7+9+9+7+7)

// CDC-ECM Descriptor Template
// Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
#define TUD_CDC_ECM_DESCRIPTOR(_itfnum, _desc_stridx, _mac_stridx, _ep_notif, _ep_notif_size, _epout, _epin, _epsize, _maxsegmentsize) \
  /* Interface Association */\
  8, TUSB_DESC_INTERFACE_ASSOCIATION, _itfnum, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ETHERNET_NETWORKING_CONTROL_MODEL, 0, 0,\
  /* CDC Control Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ETHERNET_NETWORKING_CONTROL_MODEL, 0, _desc_stridx,\
  /* CDC-ECM Header */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),\
  /* CDC-ECM Union */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, _itfnum, (uint8_t)((_itfnum) + 1),\
  /* CDC-ECM Functional Descriptor */\
  13, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ETHERNET_NETWORKING, _mac_stridx, 0, 0, 0, 0, U16_TO_U8S_LE(_maxsegmentsize), U16_TO_U8S_LE(0), 0,\
  /* Endpoint Notification */\
  7, TUSB_DESC_ENDPOINT, _ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_notif_size), 1,\
  /* CDC Data Interface (default inactive) */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 0, 0, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
  /* CDC Data Interface (alternative active) */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 1, 2, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0


//------------- RNDIS -------------//

#if 0
  /* Windows XP */
  #define TUD_RNDIS_ITF_CLASS    TUSB_CLASS_CDC
  #define TUD_RNDIS_ITF_SUBCLASS CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL
  #define TUD_RNDIS_ITF_PROTOCOL 0xFF /* CDC_COMM_PROTOCOL_MICROSOFT_RNDIS */
#else
  /* Windows 7+ */
  #define TUD_RNDIS_ITF_CLASS    TUSB_CLASS_WIRELESS_CONTROLLER
  #define TUD_RNDIS_ITF_SUBCLASS 0x01
  #define TUD_RNDIS_ITF_PROTOCOL 0x03
#endif

// Length of template descriptor: 66 bytes
#define TUD_RNDIS_DESC_LEN  (8+9+5+5+4+5+7+9+7+7)

// RNDIS Descriptor Template
// Interface number, string index, EP notification address and size, EP data address (out, in) and size.
#define TUD_RNDIS_DESCRIPTOR(_itfnum, _stridx, _ep_notif, _ep_notif_size, _epout, _epin, _epsize) \
  /* Interface Association */\
  8, TUSB_DESC_INTERFACE_ASSOCIATION, _itfnum, 2, TUD_RNDIS_ITF_CLASS, TUD_RNDIS_ITF_SUBCLASS, TUD_RNDIS_ITF_PROTOCOL, 0,\
  /* CDC Control Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 1, TUD_RNDIS_ITF_CLASS, TUD_RNDIS_ITF_SUBCLASS, TUD_RNDIS_ITF_PROTOCOL, _stridx,\
  /* CDC-ACM Header */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0110),\
  /* CDC Call Management */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0, (uint8_t)((_itfnum) + 1),\
  /* ACM */\
  4, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 0,\
  /* CDC Union */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, _itfnum, (uint8_t)((_itfnum) + 1),\
  /* Endpoint Notification */\
  7, TUSB_DESC_ENDPOINT, _ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_notif_size), 1,\
  /* CDC Data Interface */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 0, 2, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0

//------------- BT Radio -------------//
#define TUD_BT_APP_CLASS                    (TUSB_CLASS_WIRELESS_CONTROLLER)
#define TUD_BT_APP_SUBCLASS                 0x01
#define TUD_BT_PROTOCOL_PRIMARY_CONTROLLER  0x01
#define TUD_BT_PROTOCOL_AMP_CONTROLLER      0x02

#ifndef CFG_TUD_BTH_ISO_ALT_COUNT
#define CFG_TUD_BTH_ISO_ALT_COUNT 0
#endif

// Length of template descriptor: 30 bytes + number of ISO alternatives * 23
#define TUD_BTH_DESC_LEN (9 + 7 + 7 + 7 + (CFG_TUD_BTH_ISO_ALT_COUNT) * (9 + 7 + 7))

/* Primary Interface */
#define TUD_BTH_PRI_ITF(_itfnum, _stridx, _ep_evt, _ep_evt_size, _ep_evt_interval, _ep_in, _ep_out, _ep_size) \
  9, TUSB_DESC_INTERFACE, _itfnum, _stridx, 3, TUD_BT_APP_CLASS, TUD_BT_APP_SUBCLASS, TUD_BT_PROTOCOL_PRIMARY_CONTROLLER, 0, \
  /* Endpoint In for events */ \
  7, TUSB_DESC_ENDPOINT, _ep_evt, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_evt_size), _ep_evt_interval, \
  /* Endpoint In for ACL data */ \
  7, TUSB_DESC_ENDPOINT, _ep_in, TUSB_XFER_BULK, U16_TO_U8S_LE(_ep_size), 1, \
  /* Endpoint Out for ACL data */ \
  7, TUSB_DESC_ENDPOINT, _ep_out, TUSB_XFER_BULK, U16_TO_U8S_LE(_ep_size), 1

#define TUD_BTH_ISO_ITF(_itfnum, _alt, _ep_in, _ep_out, _n) ,\
  /* Interface with 2 endpoints */ \
  9, TUSB_DESC_INTERFACE, _itfnum, _alt, 2, TUD_BT_APP_CLASS, TUD_BT_APP_SUBCLASS, TUD_BT_PROTOCOL_PRIMARY_CONTROLLER, 0, \
  /* Isochronous endpoints */ \
  7, TUSB_DESC_ENDPOINT, _ep_in, TUSB_XFER_ISOCHRONOUS, U16_TO_U8S_LE(_n), 1, \
  7, TUSB_DESC_ENDPOINT, _ep_out, TUSB_XFER_ISOCHRONOUS, U16_TO_U8S_LE(_n), 1

#define _FIRST(a, ...) a
#define _REST(a, ...) __VA_ARGS__

#define TUD_BTH_ISO_ITF_0(_itfnum, ...)
#define TUD_BTH_ISO_ITF_1(_itfnum, _ep_in, _ep_out, ...) TUD_BTH_ISO_ITF(_itfnum, (CFG_TUD_BTH_ISO_ALT_COUNT) - 1, _ep_in, _ep_out, _FIRST(__VA_ARGS__))
#define TUD_BTH_ISO_ITF_2(_itfnum, _ep_in, _ep_out, ...) TUD_BTH_ISO_ITF(_itfnum, (CFG_TUD_BTH_ISO_ALT_COUNT) - 2, _ep_in, _ep_out, _FIRST(__VA_ARGS__)) \
	TUD_BTH_ISO_ITF_1(_itfnum, _ep_in, _ep_out, _REST(__VA_ARGS__))
#define TUD_BTH_ISO_ITF_3(_itfnum, _ep_in, _ep_out, ...) TUD_BTH_ISO_ITF(_itfnum, (CFG_TUD_BTH_ISO_ALT_COUNT) - 3, _ep_in, _ep_out, _FIRST(__VA_ARGS__)) \
	TUD_BTH_ISO_ITF_2(_itfnum, _ep_in, _ep_out, _REST(__VA_ARGS__))
#define TUD_BTH_ISO_ITF_4(_itfnum, _ep_in, _ep_out, ...) TUD_BTH_ISO_ITF(_itfnum, (CFG_TUD_BTH_ISO_ALT_COUNT) - 4, _ep_in, _ep_out, _FIRST(__VA_ARGS__)) \
	TUD_BTH_ISO_ITF_3(_itfnum, _ep_in, _ep_out, _REST(__VA_ARGS__))
#define TUD_BTH_ISO_ITF_5(_itfnum, _ep_in, _ep_out, ...) TUD_BTH_ISO_ITF(_itfnum, (CFG_TUD_BTH_ISO_ALT_COUNT) - 5, _ep_in, _ep_out, _FIRST(__VA_ARGS__)) \
	TUD_BTH_ISO_ITF_4(_itfnum, _ep_in, _ep_out, _REST(__VA_ARGS__))
#define TUD_BTH_ISO_ITF_6(_itfnum, _ep_in, _ep_out, ...) TUD_BTH_ISO_ITF(_itfnum, (CFG_TUD_BTH_ISO_ALT_COUNT) - 6, _ep_in, _ep_out, _FIRST(__VA_ARGS__)) \
	TUD_BTH_ISO_ITF_5(_itfnum, _ep_in, _ep_out, _REST(__VA_ARGS__))

#define TUD_BTH_ISO_ITFS(_itfnum, _ep_in, _ep_out, ...) \
	TU_XSTRCAT(TUD_BTH_ISO_ITF_, CFG_TUD_BTH_ISO_ALT_COUNT)(_itfnum, _ep_in, _ep_out, __VA_ARGS__)

// BT Primary controller descriptor
// Interface number, string index, attributes, event endpoint, event endpoint size, interval, data in, data out, data endpoint size, iso endpoint sizes
#define TUD_BTH_DESCRIPTOR(_itfnum, _stridx, _ep_evt, _ep_evt_size, _ep_evt_interval, _ep_in, _ep_out, _ep_size,...) \
  TUD_BTH_PRI_ITF(_itfnum, _stridx, _ep_evt, _ep_evt_size, _ep_evt_interval, _ep_in, _ep_out, _ep_size) \
  TUD_BTH_ISO_ITFS(_itfnum + 1, _ep_in + 1, _ep_out + 1, __VA_ARGS__)

//--------------------------------------------------------------------+
// Common Definitions
//--------------------------------------------------------------------+
/** \defgroup ClassDriver_HID_Common Common Definitions
 *  @{ */

 /// USB HID Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength;         /**< Numeric expression that is the total size of the HID descriptor */
  uint8_t  bDescriptorType; /**< Constant name specifying type of HID descriptor. */

  uint16_t bcdHID;          /**< Numeric expression identifying the HID Class Specification release */
  uint8_t  bCountryCode;    /**< Numeric expression identifying country code of the localized hardware.  */
  uint8_t  bNumDescriptors; /**< Numeric expression specifying the number of class descriptors */

  uint8_t  bReportType;     /**< Type of HID class report. */
  uint16_t wReportLength;   /**< the total size of the Report descriptor. */
} tusb_hid_descriptor_hid_t;

/// HID Subclass
typedef enum
{
  HID_SUBCLASS_NONE = 0, ///< No Subclass
  HID_SUBCLASS_BOOT = 1  ///< Boot Interface Subclass
}hid_subclass_type_t;

/// HID Protocol
typedef enum
{
  HID_PROTOCOL_NONE     = 0, ///< None
  HID_PROTOCOL_KEYBOARD = 1, ///< Keyboard
  HID_PROTOCOL_MOUSE    = 2  ///< Mouse
}hid_protocol_type_t;

/// HID Descriptor Type
typedef enum
{
  HID_DESC_TYPE_HID      = 0x21, ///< HID Descriptor
  HID_DESC_TYPE_REPORT   = 0x22, ///< Report Descriptor
  HID_DESC_TYPE_PHYSICAL = 0x23  ///< Physical Descriptor
}hid_descriptor_type_t;

/// HID Request Report Type
typedef enum
{
  HID_REPORT_TYPE_INVALID = 0,
  HID_REPORT_TYPE_INPUT,      ///< Input
  HID_REPORT_TYPE_OUTPUT,     ///< Output
  HID_REPORT_TYPE_FEATURE     ///< Feature
}hid_report_type_t;

/// HID Class Specific Control Request
typedef enum
{
  HID_REQ_CONTROL_GET_REPORT   = 0x01, ///< Get Report
  HID_REQ_CONTROL_GET_IDLE     = 0x02, ///< Get Idle
  HID_REQ_CONTROL_GET_PROTOCOL = 0x03, ///< Get Protocol
  HID_REQ_CONTROL_SET_REPORT   = 0x09, ///< Set Report
  HID_REQ_CONTROL_SET_IDLE     = 0x0a, ///< Set Idle
  HID_REQ_CONTROL_SET_PROTOCOL = 0x0b  ///< Set Protocol
}hid_request_type_t;

/// HID Country Code
typedef enum
{
  HID_LOCAL_NotSupported = 0   , ///< NotSupported
  HID_LOCAL_Arabic             , ///< Arabic
  HID_LOCAL_Belgian            , ///< Belgian
  HID_LOCAL_Canadian_Bilingual , ///< Canadian_Bilingual
  HID_LOCAL_Canadian_French    , ///< Canadian_French
  HID_LOCAL_Czech_Republic     , ///< Czech_Republic
  HID_LOCAL_Danish             , ///< Danish
  HID_LOCAL_Finnish            , ///< Finnish
  HID_LOCAL_French             , ///< French
  HID_LOCAL_German             , ///< German
  HID_LOCAL_Greek              , ///< Greek
  HID_LOCAL_Hebrew             , ///< Hebrew
  HID_LOCAL_Hungary            , ///< Hungary
  HID_LOCAL_International      , ///< International
  HID_LOCAL_Italian            , ///< Italian
  HID_LOCAL_Japan_Katakana     , ///< Japan_Katakana
  HID_LOCAL_Korean             , ///< Korean
  HID_LOCAL_Latin_American     , ///< Latin_American
  HID_LOCAL_Netherlands_Dutch  , ///< Netherlands/Dutch
  HID_LOCAL_Norwegian          , ///< Norwegian
  HID_LOCAL_Persian_Farsi      , ///< Persian (Farsi)
  HID_LOCAL_Poland             , ///< Poland
  HID_LOCAL_Portuguese         , ///< Portuguese
  HID_LOCAL_Russia             , ///< Russia
  HID_LOCAL_Slovakia           , ///< Slovakia
  HID_LOCAL_Spanish            , ///< Spanish
  HID_LOCAL_Swedish            , ///< Swedish
  HID_LOCAL_Swiss_French       , ///< Swiss/French
  HID_LOCAL_Swiss_German       , ///< Swiss/German
  HID_LOCAL_Switzerland        , ///< Switzerland
  HID_LOCAL_Taiwan             , ///< Taiwan
  HID_LOCAL_Turkish_Q          , ///< Turkish-Q
  HID_LOCAL_UK                 , ///< UK
  HID_LOCAL_US                 , ///< US
  HID_LOCAL_Yugoslavia         , ///< Yugoslavia
  HID_LOCAL_Turkish_F            ///< Turkish-F
} hid_country_code_t;

/** @} */

//--------------------------------------------------------------------+
// MOUSE
//--------------------------------------------------------------------+
/** \addtogroup ClassDriver_HID_Mouse Mouse
 *  @{ */

/// Standard HID Boot Protocol Mouse Report.
typedef struct TU_ATTR_PACKED
{
  uint8_t buttons; /**< buttons mask for currently pressed buttons in the mouse. */
  int8_t  x;       /**< Current delta x movement of the mouse. */
  int8_t  y;       /**< Current delta y movement on the mouse. */
  int8_t  wheel;   /**< Current delta wheel movement on the mouse. */
  int8_t  pan;     // using AC Pan
} hid_mouse_report_t;

/// Standard Mouse Buttons Bitmap
typedef enum
{
  MOUSE_BUTTON_LEFT     = TU_BIT(0), ///< Left button
  MOUSE_BUTTON_RIGHT    = TU_BIT(1), ///< Right button
  MOUSE_BUTTON_MIDDLE   = TU_BIT(2), ///< Middle button
  MOUSE_BUTTON_BACKWARD = TU_BIT(3), ///< Backward button,
  MOUSE_BUTTON_FORWARD  = TU_BIT(4), ///< Forward button,
}hid_mouse_button_bm_t;

/// @}

//--------------------------------------------------------------------+
// Keyboard
//--------------------------------------------------------------------+
/** \addtogroup ClassDriver_HID_Keyboard Keyboard
 *  @{ */

/// Standard HID Boot Protocol Keyboard Report.
typedef struct TU_ATTR_PACKED
{
  uint8_t modifier;   /**< Keyboard modifier (KEYBOARD_MODIFIER_* masks). */
  uint8_t reserved;   /**< Reserved for OEM use, always set to 0. */
  uint8_t keycode[6]; /**< Key codes of the currently pressed keys. */
} hid_keyboard_report_t;

/// Keyboard modifier codes bitmap
typedef enum
{
  KEYBOARD_MODIFIER_LEFTCTRL   = TU_BIT(0), ///< Left Control
  KEYBOARD_MODIFIER_LEFTSHIFT  = TU_BIT(1), ///< Left Shift
  KEYBOARD_MODIFIER_LEFTALT    = TU_BIT(2), ///< Left Alt
  KEYBOARD_MODIFIER_LEFTGUI    = TU_BIT(3), ///< Left Window
  KEYBOARD_MODIFIER_RIGHTCTRL  = TU_BIT(4), ///< Right Control
  KEYBOARD_MODIFIER_RIGHTSHIFT = TU_BIT(5), ///< Right Shift
  KEYBOARD_MODIFIER_RIGHTALT   = TU_BIT(6), ///< Right Alt
  KEYBOARD_MODIFIER_RIGHTGUI   = TU_BIT(7)  ///< Right Window
}hid_keyboard_modifier_bm_t;

typedef enum
{
  KEYBOARD_LED_NUMLOCK    = TU_BIT(0), ///< Num Lock LED
  KEYBOARD_LED_CAPSLOCK   = TU_BIT(1), ///< Caps Lock LED
  KEYBOARD_LED_SCROLLLOCK = TU_BIT(2), ///< Scroll Lock LED
  KEYBOARD_LED_COMPOSE    = TU_BIT(3), ///< Composition Mode
  KEYBOARD_LED_KANA       = TU_BIT(4) ///< Kana mode
}hid_keyboard_led_bm_t;

/// @}

//--------------------------------------------------------------------+
// REPORT DESCRIPTOR
//--------------------------------------------------------------------+
//------------- ITEM & TAG -------------//
#define HID_REPORT_DATA_0(data)
#define HID_REPORT_DATA_1(data) , data
#define HID_REPORT_DATA_2(data) , U16_TO_U8S_LE(data)
#define HID_REPORT_DATA_3(data) , U32_TO_U8S_LE(data)

#define HID_REPORT_ITEM(data, tag, type, size) \
  (((tag) << 4) | ((type) << 2) | (size)) HID_REPORT_DATA_##size(data)

#define RI_TYPE_MAIN   0
#define RI_TYPE_GLOBAL 1
#define RI_TYPE_LOCAL  2

//------------- MAIN ITEMS 6.2.2.4 -------------//
#define HID_INPUT(x)           HID_REPORT_ITEM(x,  8, RI_TYPE_MAIN, 1)
#define HID_OUTPUT(x)          HID_REPORT_ITEM(x,  9, RI_TYPE_MAIN, 1)
#define HID_COLLECTION(x)      HID_REPORT_ITEM(x, 10, RI_TYPE_MAIN, 1)
#define HID_FEATURE(x)         HID_REPORT_ITEM(x, 11, RI_TYPE_MAIN, 1)
#define HID_COLLECTION_END     HID_REPORT_ITEM(x, 12, RI_TYPE_MAIN, 0)

//------------- INPUT, OUTPUT, FEATURE 6.2.2.5 -------------//
#define HID_DATA             (0<<0)
#define HID_CONSTANT         (1<<0)

#define HID_ARRAY            (0<<1)
#define HID_VARIABLE         (1<<1)

#define HID_ABSOLUTE         (0<<2)
#define HID_RELATIVE         (1<<2)

#define HID_WRAP_NO          (0<<3)
#define HID_WRAP             (1<<3)

#define HID_LINEAR           (0<<4)
#define HID_NONLINEAR        (1<<4)

#define HID_PREFERRED_STATE  (0<<5)
#define HID_PREFERRED_NO     (1<<5)

#define HID_NO_NULL_POSITION (0<<6)
#define HID_NULL_STATE       (1<<6)

#define HID_NON_VOLATILE     (0<<7)
#define HID_VOLATILE         (1<<7)

#define HID_BITFIELD         (0<<8)
#define HID_BUFFERED_BYTES   (1<<8)

//------------- COLLECTION ITEM 6.2.2.6 -------------//
enum {
  HID_COLLECTION_PHYSICAL = 0,
  HID_COLLECTION_APPLICATION,
  HID_COLLECTION_LOGICAL,
  HID_COLLECTION_REPORT,
  HID_COLLECTION_NAMED_ARRAY,
  HID_COLLECTION_USAGE_SWITCH,
  HID_COLLECTION_USAGE_MODIFIER
};

//------------- GLOBAL ITEMS 6.2.2.7 -------------//
#define HID_USAGE_PAGE(x)         HID_REPORT_ITEM(x, 0, RI_TYPE_GLOBAL, 1)
#define HID_USAGE_PAGE_N(x, n)    HID_REPORT_ITEM(x, 0, RI_TYPE_GLOBAL, n)

#define HID_LOGICAL_MIN(x)        HID_REPORT_ITEM(x, 1, RI_TYPE_GLOBAL, 1)
#define HID_LOGICAL_MIN_N(x, n)   HID_REPORT_ITEM(x, 1, RI_TYPE_GLOBAL, n)

#define HID_LOGICAL_MAX(x)        HID_REPORT_ITEM(x, 2, RI_TYPE_GLOBAL, 1)
#define HID_LOGICAL_MAX_N(x, n)   HID_REPORT_ITEM(x, 2, RI_TYPE_GLOBAL, n)

#define HID_PHYSICAL_MIN(x)       HID_REPORT_ITEM(x, 3, RI_TYPE_GLOBAL, 1)
#define HID_PHYSICAL_MIN_N(x, n)  HID_REPORT_ITEM(x, 3, RI_TYPE_GLOBAL, n)

#define HID_PHYSICAL_MAX(x)       HID_REPORT_ITEM(x, 4, RI_TYPE_GLOBAL, 1)
#define HID_PHYSICAL_MAX_N(x, n)  HID_REPORT_ITEM(x, 4, RI_TYPE_GLOBAL, n)

#define HID_UNIT_EXPONENT(x)      HID_REPORT_ITEM(x, 5, RI_TYPE_GLOBAL, 1)
#define HID_UNIT_EXPONENT_N(x, n) HID_REPORT_ITEM(x, 5, RI_TYPE_GLOBAL, n)

#define HID_UNIT(x)               HID_REPORT_ITEM(x, 6, RI_TYPE_GLOBAL, 1)
#define HID_UNIT_N(x, n)          HID_REPORT_ITEM(x, 6, RI_TYPE_GLOBAL, n)

#define HID_REPORT_SIZE(x)        HID_REPORT_ITEM(x, 7, RI_TYPE_GLOBAL, 1)
#define HID_REPORT_SIZE_N(x, n)   HID_REPORT_ITEM(x, 7, RI_TYPE_GLOBAL, n)

#define HID_REPORT_ID(x)          HID_REPORT_ITEM(x, 8, RI_TYPE_GLOBAL, 1),
#define HID_REPORT_ID_N(x)        HID_REPORT_ITEM(x, 8, RI_TYPE_GLOBAL, n),

#define HID_REPORT_COUNT(x)       HID_REPORT_ITEM(x, 9, RI_TYPE_GLOBAL, 1)
#define HID_REPORT_COUNT_N(x, n)  HID_REPORT_ITEM(x, 9, RI_TYPE_GLOBAL, n)

#define HID_PUSH                  HID_REPORT_ITEM(x, 10, RI_TYPE_GLOBAL, 0)
#define HID_POP                   HID_REPORT_ITEM(x, 11, RI_TYPE_GLOBAL, 0)

//------------- LOCAL ITEMS 6.2.2.8 -------------//
#define HID_USAGE(x)              HID_REPORT_ITEM(x, 0, RI_TYPE_LOCAL, 1)
#define HID_USAGE_N(x, n)         HID_REPORT_ITEM(x, 0, RI_TYPE_LOCAL, n)

#define HID_USAGE_MIN(x)          HID_REPORT_ITEM(x, 1, RI_TYPE_LOCAL, 1)
#define HID_USAGE_MIN_N(x, n)     HID_REPORT_ITEM(x, 1, RI_TYPE_LOCAL, n)

#define HID_USAGE_MAX(x)          HID_REPORT_ITEM(x, 2, RI_TYPE_LOCAL, 1)
#define HID_USAGE_MAX_N(x, n)     HID_REPORT_ITEM(x, 2, RI_TYPE_LOCAL, n)

//--------------------------------------------------------------------+
// Usage Table
//--------------------------------------------------------------------+

/// HID Usage Table - Table 1: Usage Page Summary
enum {
  HID_USAGE_PAGE_DESKTOP         = 0x01,
  HID_USAGE_PAGE_SIMULATE        = 0x02,
  HID_USAGE_PAGE_VIRTUAL_REALITY = 0x03,
  HID_USAGE_PAGE_SPORT           = 0x04,
  HID_USAGE_PAGE_GAME            = 0x05,
  HID_USAGE_PAGE_GENERIC_DEVICE  = 0x06,
  HID_USAGE_PAGE_KEYBOARD        = 0x07,
  HID_USAGE_PAGE_LED             = 0x08,
  HID_USAGE_PAGE_BUTTON          = 0x09,
  HID_USAGE_PAGE_ORDINAL         = 0x0a,
  HID_USAGE_PAGE_TELEPHONY       = 0x0b,
  HID_USAGE_PAGE_CONSUMER        = 0x0c,
  HID_USAGE_PAGE_DIGITIZER       = 0x0d,
  HID_USAGE_PAGE_PID             = 0x0f,
  HID_USAGE_PAGE_UNICODE         = 0x10,
  HID_USAGE_PAGE_ALPHA_DISPLAY   = 0x14,
  HID_USAGE_PAGE_MEDICAL         = 0x40,
  HID_USAGE_PAGE_MONITOR         = 0x80, //0x80 - 0x83
  HID_USAGE_PAGE_POWER           = 0x84, // 0x084 - 0x87
  HID_USAGE_PAGE_BARCODE_SCANNER = 0x8c,
  HID_USAGE_PAGE_SCALE           = 0x8d,
  HID_USAGE_PAGE_MSR             = 0x8e,
  HID_USAGE_PAGE_CAMERA          = 0x90,
  HID_USAGE_PAGE_ARCADE          = 0x91,
  HID_USAGE_PAGE_VENDOR          = 0xFF00 // 0xFF00 - 0xFFFF
};

/// HID Usage Table - Table 6: Generic Desktop Page
enum {
  HID_USAGE_DESKTOP_POINTER                               = 0x01,
  HID_USAGE_DESKTOP_MOUSE                                 = 0x02,
  HID_USAGE_DESKTOP_JOYSTICK                              = 0x04,
  HID_USAGE_DESKTOP_GAMEPAD                               = 0x05,
  HID_USAGE_DESKTOP_KEYBOARD                              = 0x06,
  HID_USAGE_DESKTOP_KEYPAD                                = 0x07,
  HID_USAGE_DESKTOP_MULTI_AXIS_CONTROLLER                 = 0x08,
  HID_USAGE_DESKTOP_TABLET_PC_SYSTEM                      = 0x09,
  HID_USAGE_DESKTOP_X                                     = 0x30,
  HID_USAGE_DESKTOP_Y                                     = 0x31,
  HID_USAGE_DESKTOP_Z                                     = 0x32,
  HID_USAGE_DESKTOP_RX                                    = 0x33,
  HID_USAGE_DESKTOP_RY                                    = 0x34,
  HID_USAGE_DESKTOP_RZ                                    = 0x35,
  HID_USAGE_DESKTOP_SLIDER                                = 0x36,
  HID_USAGE_DESKTOP_DIAL                                  = 0x37,
  HID_USAGE_DESKTOP_WHEEL                                 = 0x38,
  HID_USAGE_DESKTOP_HAT_SWITCH                            = 0x39,
  HID_USAGE_DESKTOP_COUNTED_BUFFER                        = 0x3a,
  HID_USAGE_DESKTOP_BYTE_COUNT                            = 0x3b,
  HID_USAGE_DESKTOP_MOTION_WAKEUP                         = 0x3c,
  HID_USAGE_DESKTOP_START                                 = 0x3d,
  HID_USAGE_DESKTOP_SELECT                                = 0x3e,
  HID_USAGE_DESKTOP_VX                                    = 0x40,
  HID_USAGE_DESKTOP_VY                                    = 0x41,
  HID_USAGE_DESKTOP_VZ                                    = 0x42,
  HID_USAGE_DESKTOP_VBRX                                  = 0x43,
  HID_USAGE_DESKTOP_VBRY                                  = 0x44,
  HID_USAGE_DESKTOP_VBRZ                                  = 0x45,
  HID_USAGE_DESKTOP_VNO                                   = 0x46,
  HID_USAGE_DESKTOP_FEATURE_NOTIFICATION                  = 0x47,
  HID_USAGE_DESKTOP_RESOLUTION_MULTIPLIER                 = 0x48,
  HID_USAGE_DESKTOP_SYSTEM_CONTROL                        = 0x80,
  HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN                     = 0x81,
  HID_USAGE_DESKTOP_SYSTEM_SLEEP                          = 0x82,
  HID_USAGE_DESKTOP_SYSTEM_WAKE_UP                        = 0x83,
  HID_USAGE_DESKTOP_SYSTEM_CONTEXT_MENU                   = 0x84,
  HID_USAGE_DESKTOP_SYSTEM_MAIN_MENU                      = 0x85,
  HID_USAGE_DESKTOP_SYSTEM_APP_MENU                       = 0x86,
  HID_USAGE_DESKTOP_SYSTEM_MENU_HELP                      = 0x87,
  HID_USAGE_DESKTOP_SYSTEM_MENU_EXIT                      = 0x88,
  HID_USAGE_DESKTOP_SYSTEM_MENU_SELECT                    = 0x89,
  HID_USAGE_DESKTOP_SYSTEM_MENU_RIGHT                     = 0x8A,
  HID_USAGE_DESKTOP_SYSTEM_MENU_LEFT                      = 0x8B,
  HID_USAGE_DESKTOP_SYSTEM_MENU_UP                        = 0x8C,
  HID_USAGE_DESKTOP_SYSTEM_MENU_DOWN                      = 0x8D,
  HID_USAGE_DESKTOP_SYSTEM_COLD_RESTART                   = 0x8E,
  HID_USAGE_DESKTOP_SYSTEM_WARM_RESTART                   = 0x8F,
  HID_USAGE_DESKTOP_DPAD_UP                               = 0x90,
  HID_USAGE_DESKTOP_DPAD_DOWN                             = 0x91,
  HID_USAGE_DESKTOP_DPAD_RIGHT                            = 0x92,
  HID_USAGE_DESKTOP_DPAD_LEFT                             = 0x93,
  HID_USAGE_DESKTOP_SYSTEM_DOCK                           = 0xA0,
  HID_USAGE_DESKTOP_SYSTEM_UNDOCK                         = 0xA1,
  HID_USAGE_DESKTOP_SYSTEM_SETUP                          = 0xA2,
  HID_USAGE_DESKTOP_SYSTEM_BREAK                          = 0xA3,
  HID_USAGE_DESKTOP_SYSTEM_DEBUGGER_BREAK                 = 0xA4,
  HID_USAGE_DESKTOP_APPLICATION_BREAK                     = 0xA5,
  HID_USAGE_DESKTOP_APPLICATION_DEBUGGER_BREAK            = 0xA6,
  HID_USAGE_DESKTOP_SYSTEM_SPEAKER_MUTE                   = 0xA7,
  HID_USAGE_DESKTOP_SYSTEM_HIBERNATE                      = 0xA8,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_INVERT                 = 0xB0,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_INTERNAL               = 0xB1,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_EXTERNAL               = 0xB2,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_BOTH                   = 0xB3,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_DUAL                   = 0xB4,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_TOGGLE_INT_EXT         = 0xB5,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_SWAP_PRIMARY_SECONDARY = 0xB6,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_LCD_AUTOSCALE          = 0xB7
};


/// HID Usage Table: Consumer Page (0x0C)
/// Only contains controls that supported by Windows (whole list is too long)
enum
{
  // Generic Control
  HID_USAGE_CONSUMER_CONTROL                           = 0x0001,

  // Power Control
  HID_USAGE_CONSUMER_POWER                             = 0x0030,
  HID_USAGE_CONSUMER_RESET                             = 0x0031,
  HID_USAGE_CONSUMER_SLEEP                             = 0x0032,

  // Screen Brightness
  HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT              = 0x006F,
  HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT              = 0x0070,

  // These HID usages operate only on mobile systems (battery powered) and
  // require Windows 8 (build 8302 or greater).
  HID_USAGE_CONSUMER_WIRELESS_RADIO_CONTROLS           = 0x000C,
  HID_USAGE_CONSUMER_WIRELESS_RADIO_BUTTONS            = 0x00C6,
  HID_USAGE_CONSUMER_WIRELESS_RADIO_LED                = 0x00C7,
  HID_USAGE_CONSUMER_WIRELESS_RADIO_SLIDER_SWITCH      = 0x00C8,

  // Media Control
  HID_USAGE_CONSUMER_PLAY_PAUSE                        = 0x00CD,
  HID_USAGE_CONSUMER_SCAN_NEXT                         = 0x00B5,
  HID_USAGE_CONSUMER_SCAN_PREVIOUS                     = 0x00B6,
  HID_USAGE_CONSUMER_STOP                              = 0x00B7,
  HID_USAGE_CONSUMER_VOLUME                            = 0x00E0,
  HID_USAGE_CONSUMER_MUTE                              = 0x00E2,
  HID_USAGE_CONSUMER_BASS                              = 0x00E3,
  HID_USAGE_CONSUMER_TREBLE                            = 0x00E4,
  HID_USAGE_CONSUMER_BASS_BOOST                        = 0x00E5,
  HID_USAGE_CONSUMER_VOLUME_INCREMENT                  = 0x00E9,
  HID_USAGE_CONSUMER_VOLUME_DECREMENT                  = 0x00EA,
  HID_USAGE_CONSUMER_BASS_INCREMENT                    = 0x0152,
  HID_USAGE_CONSUMER_BASS_DECREMENT                    = 0x0153,
  HID_USAGE_CONSUMER_TREBLE_INCREMENT                  = 0x0154,
  HID_USAGE_CONSUMER_TREBLE_DECREMENT                  = 0x0155,

  // Application Launcher
  HID_USAGE_CONSUMER_AL_CONSUMER_CONTROL_CONFIGURATION = 0x0183,
  HID_USAGE_CONSUMER_AL_EMAIL_READER                   = 0x018A,
  HID_USAGE_CONSUMER_AL_CALCULATOR                     = 0x0192,
  HID_USAGE_CONSUMER_AL_LOCAL_BROWSER                  = 0x0194,

  // Browser/Explorer Specific
  HID_USAGE_CONSUMER_AC_SEARCH                         = 0x0221,
  HID_USAGE_CONSUMER_AC_HOME                           = 0x0223,
  HID_USAGE_CONSUMER_AC_BACK                           = 0x0224,
  HID_USAGE_CONSUMER_AC_FORWARD                        = 0x0225,
  HID_USAGE_CONSUMER_AC_STOP                           = 0x0226,
  HID_USAGE_CONSUMER_AC_REFRESH                        = 0x0227,
  HID_USAGE_CONSUMER_AC_BOOKMARKS                      = 0x022A,

  // Mouse Horizontal scroll
  HID_USAGE_CONSUMER_AC_PAN                            = 0x0238,
};

/* --------------------------------------------------------------------+
 * HID Report Descriptor Template
 *
 * Convenient for declaring popular HID device (keyboard, mouse, consumer,
 * gamepad etc...). Templates take "HID_REPORT_ID(n)" as input, leave
 * empty if multiple reports is not used
 *
 * - Only 1 report: no parameter
 *      uint8_t const report_desc[] = { TUD_HID_REPORT_DESC_KEYBOARD() };
 *
 * - Multiple Reports: "HID_REPORT_ID(ID)" must be passed to template
 *      uint8_t const report_desc[] =
 *      {
 *          TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(1) ) ,
 *          TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(2) )
 *      };
 *--------------------------------------------------------------------*/

// Keyboard Report Descriptor Template
#define TUD_HID_REPORT_DESC_KEYBOARD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 8 bits Modifier Keys (Shfit, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 224                                    )  ,\
      HID_USAGE_MAX    ( 231                                    )  ,\
      HID_LOGICAL_MIN  ( 0                                      )  ,\
      HID_LOGICAL_MAX  ( 1                                      )  ,\
      HID_REPORT_COUNT ( 8                                      )  ,\
      HID_REPORT_SIZE  ( 1                                      )  ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
      /* 8 bit reserved */ \
      HID_REPORT_COUNT ( 1                                      )  ,\
      HID_REPORT_SIZE  ( 8                                      )  ,\
      HID_INPUT        ( HID_CONSTANT                           )  ,\
    /* 6-byte Keycodes */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 0                                   )     ,\
      HID_USAGE_MAX    ( 255                                 )     ,\
      HID_LOGICAL_MIN  ( 0                                   )     ,\
      HID_LOGICAL_MAX  ( 255                                 )     ,\
      HID_REPORT_COUNT ( 6                                   )     ,\
      HID_REPORT_SIZE  ( 8                                   )     ,\
      HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )     ,\
    /* 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   )       ,\
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

// Mouse Report Descriptor Template
#define TUD_HID_REPORT_DESC_MOUSE(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP      )                   ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_MOUSE     )                   ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION  )                   ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    HID_USAGE      ( HID_USAGE_DESKTOP_POINTER )                   ,\
    HID_COLLECTION ( HID_COLLECTION_PHYSICAL   )                   ,\
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON  )                   ,\
        HID_USAGE_MIN   ( 1                                      ) ,\
        HID_USAGE_MAX   ( 5                                      ) ,\
        HID_LOGICAL_MIN ( 0                                      ) ,\
        HID_LOGICAL_MAX ( 1                                      ) ,\
        /* Left, Right, Middle, Backward, Forward buttons */ \
        HID_REPORT_COUNT( 5                                      ) ,\
        HID_REPORT_SIZE ( 1                                      ) ,\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
        /* 3 bit padding */ \
        HID_REPORT_COUNT( 1                                      ) ,\
        HID_REPORT_SIZE ( 3                                      ) ,\
        HID_INPUT       ( HID_CONSTANT                           ) ,\
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP )                   ,\
        /* X, Y position [-127, 127] */ \
        HID_USAGE       ( HID_USAGE_DESKTOP_X                    ) ,\
        HID_USAGE       ( HID_USAGE_DESKTOP_Y                    ) ,\
        HID_LOGICAL_MIN ( 0x81                                   ) ,\
        HID_LOGICAL_MAX ( 0x7f                                   ) ,\
        HID_REPORT_COUNT( 2                                      ) ,\
        HID_REPORT_SIZE ( 8                                      ) ,\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ) ,\
        /* Verital wheel scroll [-127, 127] */ \
        HID_USAGE       ( HID_USAGE_DESKTOP_WHEEL                )  ,\
        HID_LOGICAL_MIN ( 0x81                                   )  ,\
        HID_LOGICAL_MAX ( 0x7f                                   )  ,\
        HID_REPORT_COUNT( 1                                      )  ,\
        HID_REPORT_SIZE ( 8                                      )  ,\
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE )  ,\
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_CONSUMER ), \
       /* Horizontal wheel scroll [-127, 127] */ \
        HID_USAGE_N     ( HID_USAGE_CONSUMER_AC_PAN, 2           ), \
        HID_LOGICAL_MIN ( 0x81                                   ), \
        HID_LOGICAL_MAX ( 0x7f                                   ), \
        HID_REPORT_COUNT( 1                                      ), \
        HID_REPORT_SIZE ( 8                                      ), \
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), \
    HID_COLLECTION_END                                            , \
  HID_COLLECTION_END \

// Consumer Control Report Descriptor Template
#define TUD_HID_REPORT_DESC_CONSUMER(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER    )              ,\
  HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL )              ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )              ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    HID_LOGICAL_MIN  ( 0x00                                ) ,\
    HID_LOGICAL_MAX_N( 0x03FF, 2                           ) ,\
    HID_USAGE_MIN    ( 0x00                                ) ,\
    HID_USAGE_MAX_N  ( 0x03FF, 2                           ) ,\
    HID_REPORT_COUNT ( 1                                   ) ,\
    HID_REPORT_SIZE  ( 16                                  ) ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ) ,\
  HID_COLLECTION_END \

/* System Control Report Descriptor Template
 * 0x00 - do nothing
 * 0x01 - Power Off
 * 0x02 - Standby
 * 0x04 - Wake Host
 */
#define TUD_HID_REPORT_DESC_SYSTEM_CONTROL(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )        ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_SYSTEM_CONTROL )        ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION       )        ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 2 bit system power control */ \
    HID_LOGICAL_MIN  ( 1                                   ) ,\
    HID_LOGICAL_MAX  ( 3                                   ) ,\
    HID_REPORT_COUNT ( 1                                   ) ,\
    HID_REPORT_SIZE  ( 2                                   ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_SYSTEM_SLEEP      ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_SYSTEM_WAKE_UP    ) ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ) ,\
    /* 6 bit padding */ \
    HID_REPORT_COUNT ( 1                                   ) ,\
    HID_REPORT_SIZE  ( 6                                   ) ,\
    HID_INPUT        ( HID_CONSTANT                        ) ,\
  HID_COLLECTION_END \

// Gamepad Report Descriptor Template
// with 16 buttons and 2 joysticks with following layout
// | Button Map (2 bytes) |  X | Y | Z | Rz
#define TUD_HID_REPORT_DESC_GAMEPAD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )        ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_GAMEPAD  )        ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )        ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 16 bit Button Map */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_BUTTON                  ) ,\
    HID_USAGE_MIN    ( 1                                      ) ,\
    HID_USAGE_MAX    ( 16                                     ) ,\
    HID_LOGICAL_MIN  ( 0                                      ) ,\
    HID_LOGICAL_MAX  ( 1                                      ) ,\
    HID_REPORT_COUNT ( 16                                     ) ,\
    HID_REPORT_SIZE  ( 1                                      ) ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE    ) ,\
    /* X, Y, Z, Rz (min -127, max 127 ) */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_DESKTOP                 ) ,\
    HID_LOGICAL_MIN  ( 0x81                                   ) ,\
    HID_LOGICAL_MAX  ( 0x7f                                   ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_X                    ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_Y                    ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_Z                    ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_RZ                   ) ,\
    HID_REPORT_COUNT ( 4                                      ) ,\
    HID_REPORT_SIZE  ( 8                                      ) ,\
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
  HID_COLLECTION_END \

// HID Generic Input & Output
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_GENERIC_INOUT(report_size, ...) \
    HID_USAGE_PAGE_N ( HID_USAGE_PAGE_VENDOR, 2   ),\
    HID_USAGE        ( 0x01                       ),\
    HID_COLLECTION   ( HID_COLLECTION_APPLICATION ),\
      /* Report ID if any */\
      __VA_ARGS__ \
      /* Input */ \
      HID_USAGE       ( 0x02                                   ),\
      HID_LOGICAL_MIN ( 0x00                                   ),\
      HID_LOGICAL_MAX ( 0xff                                   ),\
      HID_REPORT_SIZE ( 8                                      ),\
      HID_REPORT_COUNT( report_size                            ),\
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
      /* Output */ \
      HID_USAGE       ( 0x03                                    ),\
      HID_LOGICAL_MIN ( 0x00                                    ),\
      HID_LOGICAL_MAX ( 0xff                                    ),\
      HID_REPORT_SIZE ( 8                                       ),\
      HID_REPORT_COUNT( report_size                             ),\
      HID_OUTPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ),\
    HID_COLLECTION_END \

//=======================================================================
// usb configuration
//=======================================================================
enum {
  ITF_NUM_HID,
  ITF_NUM_VENDOR,
  ITF_NUM_TOTAL
};

#define EPNUM_HID     0x81
#define EPNUM_VENDOR  0x02

#define CFG_TUD_HID_BUFSIZE       16
#define CFG_TUD_HID_POLL_INTERVAL 10
#define CFG_TUD_ENDPOINT0_SIZE    64
#define CFG_TUD_VENDOR_EP_SIZE    64
