/**
 * @file usbd_hid.c
 */

#include "usbd_hid.h"
#include "usbd_ctlreq.h"
#include "usb_descriptors.h"
#include "rtt.h"

static uint8_t  USBD_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user);
static uint8_t  USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
static uint8_t  USBD_HID_Write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user);

usbd_class_interface_t USBD_HID = {
    USBD_HID_Setup,
    USBD_HID_DataIn,
    NULL,
    USBD_HID_Write,
};

static uint8_t  USBD_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user)
{
    USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef *)user;
    uint16_t len = 0U;
    uint8_t *pbuf = NULL;
    uint16_t status_info = 0U;
    USBD_StatusTypeDef ret = USBD_OK;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
        case USB_REQ_TYPE_CLASS :
            switch (req->bRequest) {
                case HID_REQ_CONTROL_SET_PROTOCOL:
                hhid->Protocol = (uint8_t)(req->wValue);
                break;

                case HID_REQ_CONTROL_GET_PROTOCOL:
                USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->Protocol, 1U);
                break;

                case HID_REQ_CONTROL_SET_IDLE:
                hhid->IdleState = (uint8_t)(req->wValue >> 8);
                break;

                case HID_REQ_CONTROL_GET_IDLE:
                USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->IdleState, 1U);
                break;

                default:
                USBD_CtlError(pdev, req);
                ret = USBD_FAIL;
                break;
        } break;
        case USB_REQ_TYPE_STANDARD:
            switch (req->bRequest) {
                case USB_REQ_GET_STATUS:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    } break;
                case USB_REQ_GET_DESCRIPTOR:
                    if (req->wValue >> 8 == HID_DESC_TYPE_REPORT) {
                        if (hhid->IsKeyboard) {
                        len = tud_descriptor_hid_report_kbd_size();
                        pbuf = (uint8_t*)tud_descriptor_hid_report_kbd_cb();
                        } else {
                        len = tud_descriptor_hid_report_other_size();
                        pbuf = (uint8_t*)tud_descriptor_hid_report_other_cb();
                        }
                    } else if (req->wValue >> 8 == HID_DESC_TYPE_HID) {
                        if (hhid->IsKeyboard) {
                            len = tud_descriptor_hid_kbd_size();
                            pbuf = (uint8_t*)tud_descriptor_hid_kbd_cb();
                        } else {
                            len = tud_descriptor_hid_other_size();
                            pbuf = (uint8_t*)tud_descriptor_hid_other_cb();
                        }
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                        break;
                    }
                    USBD_CtlSendData(pdev, pbuf, len);
                    break;
                case USB_REQ_GET_INTERFACE :
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->AltSetting, 1U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_SET_INTERFACE :
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        hhid->AltSetting = (uint8_t)(req->wValue);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                break;
            } break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
        }

    return ret;
}

static uint8_t  USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user)
{
    ((USBD_HID_HandleTypeDef *)user)->state = HIDD_IDLE;
    return USBD_OK;
}

static uint8_t  USBD_HID_Write(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user)
{
    USBD_HID_HandleTypeDef* hhid = (USBD_HID_HandleTypeDef*)user;
    rtt_printf("USBD HID Write: state=%d, size=%d\n", hhid->state, size);
    if (hhid->state == HIDD_IDLE) {
        hhid->state = HIDD_BUSY;
        return USBD_LL_Transmit(pdev, epnum, data, size);
    } else {
        return USBD_BUSY;
    }
}
