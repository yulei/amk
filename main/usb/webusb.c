/**
 * webusb.c
 */

#include "tusb.h"
#include "usb_descriptors.h"

#if TINYUSB_ENABLE

//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    // nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP ) return true;

    switch (request->bRequest) {
        case VENDOR_REQUEST_WEBUSB:
        // match vendor request in BOS descriptor
        // Get landing page url
        return tud_control_xfer(rhport, request, (void*) &desc_url, desc_url.bLength);

        case VENDOR_REQUEST_MICROSOFT:
            if ( request->wIndex == 7 ) {
                // Get Microsoft OS 2.0 compatible descriptor
                uint16_t total_len;
                memcpy(&total_len, desc_ms_os_20+8, 2);

                return tud_control_xfer(rhport, request, (void*) desc_ms_os_20, total_len);
            }else {
                return false;
            }

        //case 0x22:
        // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
        // connect and disconnect.
        //web_serial_connected = (request->wValue != 0);

        // Always lit LED if connected
        //if ( web_serial_connected )
        //{
        //    board_led_write(true);
        //    blink_interval_ms = BLINK_ALWAYS_ON;

        //    tud_vendor_write_str("\r\nTinyUSB WebUSB device example\r\n");
        //}else
        //{
        //    blink_interval_ms = BLINK_MOUNTED;
        //}

        // response with status OK
        //return tud_control_status(rhport, request);

        default:
            // stall unknown request
            return false;
    }

    return true;
}

static uint8_t webusb_buffer[CFG_TUD_VENDOR_EPSIZE];
void webusb_task(void)
{
    if (tud_vendor_n_available(ITF_NUM_VENDOR)) {
        uint32_t readed = tud_vendor_n_read(ITF_NUM_VENDOR, webusb_buffer, CFG_TUD_VENDOR_EPSIZE);
        if (readed > 0) {
            // just write back
            tud_vendor_n_write(ITF_NUM_VENDOR, webusb_buffer, readed);
        }
    }
}

#endif