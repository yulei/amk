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
#include "report_queue.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

static hid_report_queue_t report_queue;

static bool usb_itf_ready(uint32_t type);

void usb_init(void)
{
    MX_USB_DEVICE_Init();
#ifdef USB_HOST_ENABLE
    MX_USB_HOST_Init();
#endif
    hid_report_queue_init(&report_queue);
}

void usb_task(void)
{
    if (!hid_report_queue_empty(&report_queue)) {
        amk_printf("dequeue buffered report: size=%d, head=%d, tail=%d\n", hid_report_queue_size(&report_queue), report_queue.head, report_queue.tail);
        hid_report_t* item = hid_report_queue_peek(&report_queue);
        if (usb_itf_ready(item->type)) {
            amk_printf("ITF ready, type:%d, send report\n", item->type);
            hid_report_t report;
            hid_report_queue_get(&report_queue, &report);
            usb_send_report(report.type, report.data, report.size);
        }
    }

#ifdef USB_HOST_ENABLE
    MX_USB_HOST_Process();
#endif
}

static bool usb_itf_ready(uint32_t type)
{
    switch(type) {
    case HID_REPORT_ID_KEYBOARD:
        return usbd_comp_itf_ready(&hUsbDeviceFS, ITF_NUM_HID_KBD);
    case HID_REPORT_ID_MOUSE:
    case HID_REPORT_ID_SYSTEM:
    case HID_REPORT_ID_CONSUMER:
        return usbd_comp_itf_ready(&hUsbDeviceFS, ITF_NUM_HID_OTHER);
    default:
        break;
    }
    return false;
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
    if (!usb_itf_ready(report_type)) {
        amk_printf("Keyboard interface busy or not ready, enqueue report\n");
        hid_report_t item;
        memcpy(item.data, data, size);
        item.type = report_type;
        item.size = size;
        hid_report_queue_put(&report_queue, &item);
    } else {
        switch(report_type) {
        case HID_REPORT_ID_KEYBOARD:
            usbd_comp_send(&hUsbDeviceFS, HID_REPORT_ID_KEYBOARD, (uint8_t*)data, size);
            break;
        case HID_REPORT_ID_MOUSE:
            usbd_comp_send(&hUsbDeviceFS, HID_REPORT_ID_MOUSE, (uint8_t*)data, size);
            break;
        case HID_REPORT_ID_SYSTEM:
            usbd_comp_send(&hUsbDeviceFS, HID_REPORT_ID_SYSTEM, (uint8_t*)data, size);
            break;
        case HID_REPORT_ID_CONSUMER:
            usbd_comp_send(&hUsbDeviceFS, HID_REPORT_ID_CONSUMER, (uint8_t*)data, size);
            break;
        }
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
    usbd_comp_send(&hUsbDeviceFS, report_id, buf, report_size);
}
#endif

#ifdef WEBUSB_ENABLE
void webusb_task(void)
{}
#endif