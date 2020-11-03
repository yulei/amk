/**
 * usb_tinyusb.c
 */

#include "usb_interface.h"
#include "usb_descriptors.h"
#include "generic_hal.h"

void usb_init(void)
{
    tusb_init();
}

void usb_task(void)
{
    tud_task();
}

bool usb_ready(void)
{
    return tud_hid_ready();
}

bool usb_suspended(void)
{
    return tud_suspended();
}

void usb_remote_wakeup(void)
{
#if defined(OPT_MCU_STM32F1)
    tud_remote_wakeup();
#elif defined(OPT_MCU_STM32F4)
#define USB_DEVICE     ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
    USB_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
    HAL_Delay(5);
    USB_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
#else

#endif
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size);
        break;
    case HID_REPORT_ID_MOUSE:
        tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, data, (uint8_t)size);
        break;
    case HID_REPORT_ID_SYSTEM:
        tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, data, (uint8_t)size);
        break;
    case HID_REPORT_ID_CONSUMER:
        tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_CONSUMER, data, (uint8_t)size);
      break;
    }
}

//=============================
// tusb callback
//=============================
// Invoked when device is mounted
void tud_mount_cb(void) { }

// Invoked when device is unmounted
void tud_umount_cb(void) { }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
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
    (void) report_id;
    if (itf == ITF_NUM_HID_KBD && report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize) {
            amk_led_state = buffer[0];
        }
    }
}