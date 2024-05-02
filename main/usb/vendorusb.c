/**
 * @file vendorusb.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "tusb.h"
#include "usb_common.h"
#include "vendorusb.h"
#include "amk_printf.h"

#ifndef VENDORUSB_DEBUG
#define VENDORUSB_DEBUG 1
#endif

#if VENDORUSB_DEBUG
#define vu_debug  amk_printf
#else
#define vu_debug(...)
#endif

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
uint8_t const desc_bos[] = {
    // total length, number of device caps
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

    // Vendor Code, iLandingPage
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

    // Microsoft OS 2.0 descriptor
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)
};

uint8_t const * tud_descriptor_bos_cb(void)
{
    return desc_bos;
}


uint8_t const desc_ms_os_20[] = {
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


#define WEBUSB_URL  "docs.iatkb.com"

const tusb_desc_webusb_url_t desc_url = {
    .bLength         = 3 + sizeof(WEBUSB_URL) - 1,
    .bDescriptorType = 3, // WEBUSB URL type
    .bScheme         = 1, // 0: http, 1: https
    .url             = WEBUSB_URL
};

//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    // nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;

    switch (request->bmRequestType_bit.type) {
        case TUSB_REQ_TYPE_VENDOR:
            {
                switch (request->bRequest) {
                    case VENDOR_REQUEST_WEBUSB:
                    // match vendor request in BOS descriptor
                    // Get landing page url
                    return tud_control_xfer(rhport, request, (void*)(uintptr_t) &desc_url, desc_url.bLength);

                    case VENDOR_REQUEST_MICROSOFT:
                    {
                        vu_debug("vendor request microsoft, index=%d\n", request->wIndex);
                        if ( request->wIndex == 7 ) {
                            // Get Microsoft OS 2.0 compatible descriptor
                            uint16_t total_len;
                            memcpy(&total_len, desc_ms_os_20+8, 2);

                            return tud_control_xfer(rhport, request, (void*)(uintptr_t) desc_ms_os_20, total_len);

                        } else {
                            return false;
                        }
                    }
                    break;
                    default:
                    break;
                }
            }
            break;

        case TUSB_REQ_TYPE_CLASS:
            {
                vu_debug("webusb class request: %d\n", request->bRequest);
            }
            break;

        default:
            break;
    }

    // stall unknown request
    return false;
}

#include "amk_protocol.h"
#include "anim_file.h"

static uint32_t process_file_command(uint8_t *msg, uint32_t size)
{
    if (msg[0] != amk_protocol_prefix) {
        vu_debug("not a valid amk protocol\n");
        msg[0] = amk_protocol_fail;
        return 1;
    }

    uint8_t cmd = msg[1];
    switch(cmd) {
    case amk_protocol_open_file:
        {
            uint8_t index = msg[2];
            uint8_t read = msg[3];
            msg[2] = amk_protocol_fail;

            struct amk_anim_file_info info;
            if (read) {
                if (amk_anim_file_get_info(index, &info)) {
                    if (amk_anim_file_open(&info, read)) {
                        msg[2] = amk_protocol_ok;
                        msg[3] = index;
                    }
                }
            } else {
                memcpy(info.name, &msg[4], ANIM_FILE_NAME_MAX);
                if (amk_anim_file_open(&info, read)) {
                    msg[2] = amk_protocol_ok;
                    msg[3] = info.index;
                }
            }
        }
        return 4;
    case amk_protocol_write_file:
        {
            //return 0xFF;
            uint8_t index = msg[2];
            uint8_t size = msg[3];
            uint32_t offset = msg[4] | (msg[5]<<8) | (msg[6]<<16) | (msg[7]<<24);
            msg[2] = amk_protocol_fail;
            struct amk_anim_file_info info;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_write(&info, &msg[8], offset, size)) {
                    msg[2] = amk_protocol_ok;
                }
            }
        }
        return 0xFF;
    case amk_protocol_read_file:
        {
            uint8_t index = msg[2];
            uint8_t size = msg[3];
            uint32_t offset = msg[4] | (msg[5]<<8) | (msg[6]<<16) | (msg[7]<<24);
            msg[2] = amk_protocol_fail;
            struct amk_anim_file_info info;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_read(&info, &msg[8], offset, size)) {
                    msg[2] = amk_protocol_ok;
                }
            }
        }
        return 3;
    case amk_protocol_close_file:
        {
            uint8_t index = msg[2];
            struct amk_anim_file_info info;
            msg[2] = amk_protocol_fail;
            if (amk_anim_file_get_info(index, &info)) {
                if (amk_anim_file_close(&info)) {
                    msg[2] = amk_protocol_ok;
                }
            }
            vu_debug("close file at:%d, result=%d\n", index, msg[2]);
        }
        return 3;
    default:
        break;
    }

    msg[0] = amk_protocol_fail;
    return 1;
}

void vendorusb_task(void)
{
    if (tud_vendor_available()) {
        uint8_t buf[64];
        uint32_t count = tud_vendor_read(buf, sizeof(buf));
        uint32_t num = process_file_command(buf, count);
        if (num == 0xFF) {
            tud_vendor_read_flush();
            return;
        }

        tud_vendor_write(buf, num);
        tud_vendor_flush();
    }
}
