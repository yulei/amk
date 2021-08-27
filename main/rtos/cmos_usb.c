/**
 * @file cmos_usb.c
 */

#include "generic_hal.h"
#include "cmsis_os2.h"
#include "tusb.h"
#include "usb_interface.h"
#include "amk_printf.h"
#include "report.h"

void usb_init(void)
{
    tud_init(0);
}

void usb_task(void)
{
    tud_task();
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
    tud_remote_wakeup();
}

void usb_connect(bool on)
{
    if (on) {
        tud_connect();
    } else {
        tud_disconnect();
    }
}

static inline void wait_interface(uint8_t itf)
{
    while( !tud_hid_n_ready(itf)) {
        osDelay(1);
    }
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        wait_interface(ITF_NUM_HID_KBD);
        if (!tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size)) {
            amk_printf("failed to sent keyboard report\n");
        } else {
            report_keyboard_t *keyboard = (report_keyboard_t*)data;
            for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
                if(keyboard->keys[i] == KC_CAPS) {
                    osDelay(100); // fix caps lock under mac
                    amk_printf("delay 100ms after sent caps\n");
                }
            }
        }
        break;
    case HID_REPORT_ID_MOUSE:
        wait_interface(ITF_NUM_HID_OTHER);
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, data, (uint8_t)size)) {
            amk_printf("failed to sent mouse report\n");
        }
        break;
    case HID_REPORT_ID_SYSTEM:
        wait_interface(ITF_NUM_HID_OTHER);
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, data, (uint8_t)size)) {
            amk_printf("failed to sent system report\n");
        }
        break;
    case HID_REPORT_ID_CONSUMER:
        wait_interface(ITF_NUM_HID_OTHER);
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_CONSUMER, data, (uint8_t)size)) {
            amk_printf("failed to sent consumer report\n");
        }
        break;
    }
}
