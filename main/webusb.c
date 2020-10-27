/**
 * webusb.c
 */

#include "tusb.h"
#include "usb_descriptors.h"

// Invoked when received VENDOR control request
bool tud_vendor_control_request_cb(uint8_t rhport, tusb_control_request_t const * request)
{
    switch (request->bRequest) {
    case VENDOR_REQUEST_WEBUSB:
        // match vendor request in BOS descriptor
        // Get landing page url
        return tud_control_xfer(rhport, request, tud_descriptor_url_cb(), tud_descriptor_url_size());

    case VENDOR_REQUEST_MICROSOFT:
        if ( request->wIndex == 7 ) {
            // Get Microsoft OS 2.0 compatible descriptor
            uint16_t total_len;
            memcpy(&total_len, tud_descriptor_msos20_cb()+8, 2);

            return tud_control_xfer(rhport, request, tud_descriptor_msos20_cb(), tud_descriptor_msos20_size());
        } else {
            return false;
        }

    case 0x22:
        // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
        // connect and disconnect.
        // web_serial_connected = (request->wValue != 0);

        // response with status OK
        return tud_control_status(rhport, request);

    default:
        // stall unknown request
        return false;
    }

    return true;
}

// Invoked when DATA Stage of VENDOR's request is complete
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const * request)
{
    (void) rhport;
    (void) request;

    // nothing to do
    return true;
}

void webusb_task(void)
{
    if (tud_vendor_available()) {

    }
}