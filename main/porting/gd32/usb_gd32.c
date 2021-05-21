/**
 * @file usb_gd32.c
 *  interface implementation
 */

#include "usb_interface.h"
#include "usb_conf.h"
#include "drv_usb_dev.h"
#include "drv_usb_hw.h"
#include "report_queue.h"
#include "wait.h"
#include "amk_printf.h"
#include "usb_descriptor.h"

static usb_core_driver hid_comp;
static usb_desc hid_comp_desc;
static usb_class_core hid_comp_core;

static hid_report_queue_t report_queue;

static bool usb_itf_ready(uint32_t type);
static bool usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size);

void usb_init(void)
{
    hid_report_queue_init(&report_queue);
    usb_rcu_config();
    usb_timer_init();
    usbd_init(&hid_comp, USB_CORE_ENUM_FS, &hid_comp_desc, &hid_comp_class);
    usb_intr_config();
}

void usb_task(void)
{
    if (!hid_report_queue_empty(&report_queue)) {
        hid_report_t* item = hid_report_queue_peek(&report_queue);
        if (usb_itf_ready(item->type)) {
            amk_printf("ITF ready, type:%d, send report\n", item->type);
            hid_report_t report;
            hid_report_queue_get(&report_queue, &report);
            usb_itf_send_report(report.type, report.data, report.size);
        }
    }
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

static bool usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
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
    return true;
}

bool usb_ready(void)
{
    return (hid_comp.dev.cur_status == USBD_CONFIGURED);
}

bool usb_suspended(void)
{
    return (hid_comp.dev.cur_status == USBD_SUSPENDED);
}

void usb_remote_wakeup(void)
{
    usb_rwkup_set(&hid_comp);
    wait_ms(5);
    usb_rwkup_reset(&hid_comp);
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    hid_report_t item;
    memcpy(item.data, data, size);
    item.type = report_type;
    item.size = size;
    hid_report_queue_put(&report_queue, &item);
}
