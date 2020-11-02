/**
 * usb_hal.c
 */

#include "usb_interface.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "usb_device.h"
#include "usb_host.h"
#include "usbh_hid.h"
#include "usb_descriptors.h"
#include "usbd_composite.h"
#include "rtt.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

void usb_init(void)
{
    MX_USB_DEVICE_Init();
    MX_USB_HOST_Init();
}

void usb_task(void)
{
    MX_USB_HOST_Process();
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


void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    rtt_printf("HID event, ready=%d, size=%d\n", HID_Handle->DataReady, HID_Handle->length);

    static uint8_t buf[16];
    USBH_HID_FifoRead(&HID_Handle->fifo, buf, 8);
    rtt_printf("HID data, [%x%x%x%x%x%x%x%x]\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    extern USBD_HandleTypeDef hUsbDeviceFS;
    USBD_COMP_Send(&hUsbDeviceFS, HID_REPORT_ID_KEYBOARD, buf, 8);
}
