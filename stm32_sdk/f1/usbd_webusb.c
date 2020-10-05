/**
 * @file usbd_webusb.c
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_webusb.h"
#include "usbd_ctlreq.h"
#include "usb_descriptors.h"

static uint8_t  USBD_WEBUSB_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user); 
static uint8_t  USBD_WEBUSB_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t  USBD_WEBUSB_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);

usbd_class_interface_t USBD_WEBUSB = {
    USBD_WEBUSB_Setup,
    USBD_WEBUSB_DataIn,
    USBD_WEBUSB_DataOut,
};

static uint8_t  USBD_WEBUSB_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user)
{
    uint16_t len = 0U;
    uint8_t *pbuf = NULL;
    USBD_StatusTypeDef ret = USBD_OK;

    if ((req->bmRequest&USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
        switch (req->bRequest) {
        case VENDOR_REQUEST_WEBUSB: {
            len = tud_descriptor_url_size();
            pbuf = tud_descriptor_url_cb();
        } break;
        case VENDOR_REQUEST_MICROSOFT: {
            if ( req->wIndex == 7 ) {
                len = tud_descriptor_msos20_size();
                pbuf = tud_descriptor_msos20_cb();
            }
        } break;
        default:
            break;
        }
    }

    if (len > 0) {
        ret = USBD_CtlSendData(pdev, pbuf, len);
    }

    return ret;
}

uint8_t USBD_WEBUSB_Send(USBD_HandleTypeDef  *pdev, uint8_t *data, uint16_t size)
{
    return USBD_OK;
}

static uint8_t  USBD_WEBUSB_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    return USBD_OK;
}

static uint8_t  USBD_WEBUSB_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    return USBD_OK;
}
