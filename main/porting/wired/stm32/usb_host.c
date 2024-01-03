/**
 * @file usb_host.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
*/

#include "usb_interface.h"
#include "usbh_hid_multi.h"

#include "amk_printf.h"

extern USBD_HandleTypeDef hUsbHost;

void hal_host_init(void)
{
    MX_USB_HOST_Init();
}

void hal_host_task(void)
{
    MX_USB_HOST_Process();
}

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *itf = &phid->itfs[phid->current];
    amk_printf("HID event, itf=%d, ready=%d, size=%d, type=%d\n", phid->current, itf->data_ready, itf->report_size, itf->type);

    // read data out
    static uint8_t buf[64];
    uint8_t *p_buf = NULL;
    uint8_t report_id = 0;
    uint8_t report_size = 0;
    memset(&buf[0], 0, sizeof(buf));
    switch(itf->type) {
    case HID_MOUSE:
        report_id = HID_REPORT_ID_MOUSE;
        //buf[0] = report_id;
        USBH_HID_FifoRead(&itf->fifo, &buf[0], itf->report_size);
        report_size = 5;
        p_buf = &buf[0];
        break;
    case HID_KEYBOARD:
        report_id = HID_REPORT_ID_KEYBOARD;
        report_size = itf->report_size;
        p_buf = &buf[0];
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
                        p_buf = &buf[1];
                        if (ird->desc[i].keyboard.keycode_size > 8) {
                            // nkro?
                            report_id = HID_REPORT_ID_NKRO;
                            //buf[0] = report_id;
                            report_size = NKRO_KEYCODE_SIZE;
                        } else {
                            report_id = HID_REPORT_ID_KEYBOARD;
                            report_size = 8;
                        }
                    break;
                    case RDI_MOUSE:
                        report_id = HID_REPORT_ID_MOUSE;
                        //buf[0] = report_id;
                        p_buf = &buf[1];
                        report_size = 5;
                    break;
                    case RDI_SYSTEM:
                        report_id = HID_REPORT_ID_SYSTEM;
                        //buf[0] = report_id;
                        p_buf = &buf[1];
                        report_size = 2;
                    break;
                    case RDI_CONSUMER:
                        report_id = HID_REPORT_ID_CONSUMER;
                        //buf[0] = report_id;
                        p_buf = &buf[1];
                        report_size = 2;
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

    usb_send_report(report_id, p_buf, report_size);
}