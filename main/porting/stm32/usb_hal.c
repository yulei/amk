/**
 * usb_hal.c
 */

#include "usb_interface.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "usb_device.h"
#include "usb_descriptors.h"
#include "usbd_composite.h"
#include "amk_printf.h"
#include "usb_host.h"
#include "usbh_hid_multi.h"
#include "report_parser.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

void usb_init(void)
{
    MX_USB_DEVICE_Init();
#ifdef USB_HOST_ENABLE
    MX_USB_HOST_Init();
#endif
}

void usb_task(void)
{
#ifdef USB_HOST_ENABLE
    MX_USB_HOST_Process();
#endif
}

bool usb_ready(void)
{
    return (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED);
}

bool usb_suspended(void)
{
    return (hUsbDeviceFS.dev_state == USBD_STATE_SUSPENDED);
}

void usb_remote_wakeup(void)
{
    PCD_HandleTypeDef *pcd = (PCD_HandleTypeDef *)(hUsbDeviceFS.pData);
    HAL_PCD_ActivateRemoteWakeup(pcd);
    HAL_Delay(5);
    HAL_PCD_DeActivateRemoteWakeup(pcd);
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        USBD_COMP_Send(&hUsbDeviceFS, HID_REPORT_ID_KEYBOARD, (uint8_t*)data, size);
        break;
    case HID_REPORT_ID_MOUSE:
        USBD_COMP_Send(&hUsbDeviceFS, HID_REPORT_ID_MOUSE, (uint8_t*)data, size);
        break;
    case HID_REPORT_ID_SYSTEM:
        USBD_COMP_Send(&hUsbDeviceFS, HID_REPORT_ID_SYSTEM, (uint8_t*)data, size);
        break;
    case HID_REPORT_ID_CONSUMER:
        USBD_COMP_Send(&hUsbDeviceFS, HID_REPORT_ID_CONSUMER, (uint8_t*)data, size);
        break;
    }
}

#ifdef USB_HOST_ENABLE
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *itf = &phid->itfs[phid->current];
    amk_printf("HID event, itf=%d, ready=%d, size=%d\n", phid->current, itf->data_ready, itf->report_size);

    // read data out
    static uint8_t buf[64];
    uint8_t report_id = 0;
    uint8_t report_size = 0;
    memset(&buf[0], 0, sizeof(buf));
    switch(itf->type) {
    case HID_MOUSE:
        report_id = HID_REPORT_ID_MOUSE;
        buf[0] = report_id;
        USBH_HID_FifoRead(&itf->fifo, &buf[1], itf->report_size);
        report_size = 5 + 1;
        break;
    case HID_KEYBOARD:
        report_id = HID_REPORT_ID_KEYBOARD;
        report_size = itf->report_size;
        USBH_HID_FifoRead(&itf->fifo, &buf[0], itf->report_size);
        break;
    case HID_EXTRA: {
        report_id = HID_REPORT_ID_UNKNOWN;
        report_size = 0;
        itf_report_desc_t * ird = rp_get(phid->current);
        USBH_HID_FifoRead(&itf->fifo, &buf[0], itf->report_size);
        for (int i = 0; i < ird->desc_count; i++) {
            if ( buf[0] == ird->desc[i].report_id) {
                // found
                switch (ird->desc[i].rd_type) {
                    case RDI_KEYBOARD:
                        if (ird->desc[i].keyboard.keycode_size > 8) {
                            // nkro?
                            report_id = HID_REPORT_ID_NKRO;
                            buf[0] = report_id;
                            report_size = NKRO_KEYCODE_SIZE;
                        } else {
                            if (itf->report_size > 8) {
                                memcpy(&buf[0], &buf[1], 8);// removed the report id
                            }
                            report_id = HID_REPORT_ID_KEYBOARD;
                            report_size = 8;
                        }
                    break;
                    case RDI_MOUSE:
                        report_id = HID_REPORT_ID_MOUSE;
                        buf[0] = report_id;
                        report_size = 6;
                    break;
                    case RDI_SYSTEM:
                        report_id = HID_REPORT_ID_SYSTEM;
                        buf[0] = report_id;
                        report_size = 3;
                    break;
                    case RDI_CONSUMER:
                        report_id = HID_REPORT_ID_CONSUMER;
                        buf[0] = report_id;
                        report_size = 3;
                    break;
                }
                break;
            }
        }
        } break;
    case HID_RAW:
        USBH_HID_FifoRead(&itf->fifo, buf, itf->report_size);
        amk_printf("HID Event: should never get data from RAW interface\n");
        break;
    default:
        USBH_HID_FifoRead(&itf->fifo, buf, itf->report_size);
        amk_printf("HID Event: unknown interface\n");
        break;
    }
    if (report_id == HID_REPORT_ID_UNKNOWN) {
        // do nothing
        amk_printf("HID Event: unknown report\n");
        return;
    }

    extern USBD_HandleTypeDef hUsbDeviceFS;
    USBD_COMP_Send(&hUsbDeviceFS, report_id, buf, report_size);
}
#endif

#ifdef WEBUSB_ENABLE
void webusb_task(void)
{}
#endif