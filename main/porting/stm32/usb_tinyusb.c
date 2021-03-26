/**
 * usb_tinyusb.c
 */

#include "usb_interface.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "amk_printf.h"
#include "report_queue.h"

static hid_report_queue_t report_queue;

static bool usb_itf_ready(uint32_t type);
static bool usb_itf_send_report(uint32_t type, const void* data, uint32_t size);

void usb_init(void)
{
    tusb_init();
    hid_report_queue_init(&report_queue);
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

    tud_task();
}

static bool usb_itf_ready(uint32_t type)
{
    switch(type) {
    case HID_REPORT_ID_KEYBOARD:
        return tud_hid_n_ready(ITF_NUM_HID_KBD);
    case HID_REPORT_ID_MOUSE:
    case HID_REPORT_ID_SYSTEM:
    case HID_REPORT_ID_CONSUMER:
        return tud_hid_n_ready(ITF_NUM_HID_OTHER);
    default:
        break;
    }
    return false;
}

static bool usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        if (!tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size)) {
            amk_printf("failed to sent keyboard report\n");
            return false;
        }
        break;
    case HID_REPORT_ID_MOUSE:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, data, (uint8_t)size)) {
                amk_printf("failed to sent mouse report\n");
                return false;
        }
        break;
    case HID_REPORT_ID_SYSTEM:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, data, (uint8_t)size)) {
            amk_printf("failed to sent system report\n");
            return false;
        }
        break;
    case HID_REPORT_ID_CONSUMER:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_CONSUMER, data, (uint8_t)size)) {
            amk_printf("failed to sent consumer report\n");
            return false;
        }
        break;
    default:
        amk_printf("unknonw report type: %d\n", report_type);
        return false;
    }
    return true;
}

bool usb_ready(void)
{
    return tud_ready();
}

bool usb_suspended(void)
{
    return tud_suspended();
}

void usb_remote_wakeup(void)
{
#if defined(STM32F103xB)
    tud_remote_wakeup();
#elif defined(STM32F405xx) || defined(STM32F411xE) || defined(STM32F722xx)
#define USB_DEVICE     ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
    if ((USB_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS) {
        USB_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
        HAL_Delay(5);
        USB_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
    }
#else
#error "Unsupport MCU on remote wakeup"
#endif
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    hid_report_t item;
    memcpy(item.data, data, size);
    item.type = report_type;
    item.size = size;
    hid_report_queue_put(&report_queue, &item);
}

//=============================
// tusb callback
//=============================
// Invoked when device is mounted
void tud_mount_cb(void) { }

// Invoked when device is unmounted
void tud_umount_cb(void) { }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
extern uint8_t amk_led_state;
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    //amk_printf("Set Report:\n")
    (void) report_id;
    if (itf == ITF_NUM_HID_KBD && report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize) {
            amk_led_state = buffer[0];
            amk_printf("Set Report Data: size=%d, state=%x\n", bufsize, buffer[0]);
        }
    }
}