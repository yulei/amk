/**
 * @file usbd_webusb.c
 */

#include "usbd_webusb.h"
#include "usbd_ctlreq.h"
#include "usb_descriptors.h"

static uint8_t  USBD_WEBUSB_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user); 
static uint8_t  USBD_WEBUSB_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t  USBD_WEBUSB_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t  USBD_WEBUSB_Write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user);

usbd_class_interface_t USBD_WEBUSB = {
    USBD_WEBUSB_Setup,
    USBD_WEBUSB_DataIn,
    USBD_WEBUSB_DataOut,
    USBD_WEBUSB_Write,
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

static uint8_t  USBD_WEBUSB_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    USBD_WEBUSB_HandleTypeDef* hwusb = (USBD_WEBUSB_HandleTypeDef*)user;
    hwusb->state = WEBUSB_IDLE;
    return USBD_OK;
}

static uint8_t  USBD_WEBUSB_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    return USBD_OK;
}

static uint8_t  USBD_WEBUSB_Write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user)
{
    USBD_WEBUSB_HandleTypeDef* hwusb = (USBD_WEBUSB_HandleTypeDef*)user;
    if (hwusb->state == WEBUSB_IDLE) {
        hwusb->state = WEBUSB_BUSY;
        return USBD_LL_Transmit(pdev, epnum, data, size);
    } else {
        return USBD_BUSY;
    }
}