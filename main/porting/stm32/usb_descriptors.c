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

#include <string.h>
#include "usb_descriptors.h"
#include "util.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device TU_ATTR_ALIGNED(4) =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0210, // at least 2.1 or 3.x for BOS & webUSB
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = VENDOR_ID,
    .idProduct          = PRODUCT_ID,
    .bcdDevice          = DEVICE_VER,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t* tud_descriptor_device_cb(void)
{
  return (uint8_t *) &desc_device;
}

uint32_t tud_descriptor_device_size(void)
{
  return sizeof(desc_device);
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+
static uint8_t desc_hid_report_kbd[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD(),
};

static uint8_t desc_hid_report_other[] =
{
  TUD_HID_REPORT_DESC_MOUSE           ( HID_REPORT_ID(REPORT_ID_MOUSE) ),
  TUD_HID_REPORT_DESC_SYSTEM_CONTROL  ( HID_REPORT_ID(REPORT_ID_SYSTEM) ),
  TUD_HID_REPORT_DESC_CONSUMER        ( HID_REPORT_ID(REPORT_ID_CONSUMER) ),
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t* tud_descriptor_hid_report_kbd_cb(void)
{
  return (uint8_t*)(&desc_hid_report_kbd[0]);
}

uint32_t tud_descriptor_hid_report_kbd_size(void)
{
  return sizeof(desc_hid_report_kbd);
}

uint8_t* tud_descriptor_hid_report_other_cb(void)
{
  return (uint8_t*)(&desc_hid_report_other[0]);
}

uint32_t tud_descriptor_hid_report_other_size(void)
{
  return sizeof(desc_hid_report_other);
}
//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+


#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN + TUD_VENDOR_DESC_LEN)


static uint8_t desc_configuration[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, 3, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),
  // Interface number, string index, protocol, report descriptor len, EP In & Out address, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_SIZE, CFG_TUD_HID_POLL_INTERVAL),
  TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_SIZE, CFG_TUD_HID_POLL_INTERVAL),

  // Interface number, string index, EP Out & IN address, EP size
  TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, EPNUM_VENDOR, 0x80|EPNUM_VENDOR, CFG_TUD_VENDOR_EP_SIZE),
};

static uint8_t desc_hid_kbd[] = {
  TUD_HID_DESCRIPTOR(ITF_NUM_HID_KBD, 0, HID_PROTOCOL_KEYBOARD, sizeof(desc_hid_report_kbd), 0x80|EPNUM_HID_KBD, CFG_TUD_HID_EP_SIZE, CFG_TUD_HID_POLL_INTERVAL),
};

static uint8_t desc_hid_other[] = {
  TUD_HID_DESCRIPTOR(ITF_NUM_HID_OTHER, 0, HID_PROTOCOL_NONE, sizeof(desc_hid_report_other), 0x80|EPNUM_HID_OTHER, CFG_TUD_HID_EP_SIZE, CFG_TUD_HID_POLL_INTERVAL),
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t* tud_descriptor_configuration_cb(void)
{
  return (uint8_t*)(&desc_configuration[0]);
}

uint32_t tud_descriptor_configuration_size(void)
{
  return sizeof(desc_configuration);
}

uint8_t* tud_descriptor_hid_kbd_cb(void)
{
  return (uint8_t*)(&desc_hid_kbd[0]);
}

uint32_t tud_descriptor_hid_kbd_size(void)
{
  return sizeof(desc_hid_kbd);
}

uint8_t* tud_descriptor_hid_other_cb(void)
{
  return (uint8_t*)(&desc_hid_other[0]);
}

uint32_t tud_descriptor_hid_other_size(void)
{
  return sizeof(desc_hid_other);
}
//--------------------------------------------------------------------+
// BOS Descriptor
//--------------------------------------------------------------------+

/* Microsoft OS 2.0 registry property descriptor
Per MS requirements https://msdn.microsoft.com/en-us/library/windows/hardware/hh450799(v=vs.85).aspx
device should create DeviceInterfaceGUIDs. It can be done by driver and
in case of real PnP solution device should expose MS "Microsoft OS 2.0
registry property descriptor". Such descriptor can insert any record
into Windows registry per device/configuration/interface. In our case it
will insert "DeviceInterfaceGUIDs" multistring property.
GUID is freshly generated and should be OK to use.
https://developers.google.com/web/fundamentals/native-hardware/build-for-webusb/
(Section Microsoft OS compatibility descriptors)
*/

#define BOS_TOTAL_LEN      (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

#define MS_OS_20_DESC_LEN  0xB2

// BOS Descriptor is required for webUSB
static uint8_t desc_bos[] =
{
  // total length, number of device caps
  TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

  // Vendor Code, iLandingPage
  TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

  // Microsoft OS 2.0 descriptor
  TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)
};

uint8_t* tud_descriptor_bos_cb(void)
{
  return (uint8_t*)(&desc_bos[0]);
}

uint32_t tud_descriptor_bos_size(void)
{
  return BOS_TOTAL_LEN;
}

#define URL  "iatkb.com"

const tusb_desc_webusb_url_t desc_url = {
  .bLength         = 3 + sizeof(URL) - 1,
  .bDescriptorType = 3, // WEBUSB URL type
  .bScheme         = 1, // 0: http, 1: https
  .url             = URL
};

uint8_t* tud_descriptor_url_cb(void)
{
    return (uint8_t*)&desc_url;
}

uint32_t tud_descriptor_url_size(void)
{
    return desc_url.bLength;
}

static uint8_t desc_ms_os_20[] =
{
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

uint8_t* tud_descriptor_msos20_cb(void)
{
    return desc_ms_os_20; 
}

uint32_t tud_descriptor_msos20_size(void)
{
    return MS_OS_20_DESC_LEN;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr [] =
{
  (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
  STR(MANUFACTURER),              // 1: Manufacturer
  STR(PRODUCT),                   // 2: Product
  "123456",                       // 3: Serials, should use chip ID
  "HID Config",                   // 4: Device configuration 
  "HID Keyboard",                 // 5: Hid keyboard
  "WebUSB",                       // 6: Webusb interface
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t* tud_descriptor_string_cb(uint8_t index, uint16_t* length)
{
  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }
  else
  {
    // Convert ASCII string into UTF-16

    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
  *length = _desc_str[0] & 0xFF;

  return (uint8_t*)_desc_str;
}
