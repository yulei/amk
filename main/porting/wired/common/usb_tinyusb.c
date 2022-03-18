/**
 * usb_tinyusb.c
 */

#include "tusb.h"

#include "amk_usb.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "report_queue.h"
#include "report.h"
#include "wait.h"
#include "amk_keymap.h"
#include "amk_printf.h"

#ifdef VIAL_ENABLE
#include "vial_porting.h"
#endif

void amk_usb_init(void)
{
    tusb_init();
}

void amk_usb_task(void)
{
    tud_task();
#ifdef TINYUSB_HOST_ENABLE
    tuh_task();
#endif
}

bool amk_usb_itf_ready(uint32_t type)
{
    switch(type) {
    case HID_REPORT_ID_KEYBOARD:
    case HID_REPORT_ID_MACRO:
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

bool amk_usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
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

bool amk_usb_ready(void)
{
    return tud_ready();
}

bool amk_usb_suspended(void)
{
    return tud_suspended();
}

void amk_usb_remote_wakeup(void)
{
    tud_remote_wakeup();
}

void amk_usb_connect(bool on)
{
    if(on) {
        tud_connect();
    } else {
        tud_disconnect();
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

#ifdef VIAL_ENABLE
    if (itf == ITF_NUM_VIAL) {
        //amk_printf("Vial GetReport Data: size=%d\n", reqlen);
        return 0;
    }
#endif
    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
extern uint8_t amk_led_state;
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    //amk_printf("Set Report: itf=%d, id=%d, type=%d\n", itf, report_id, report_type);
    (void) report_id;
    if (itf == ITF_NUM_HID_KBD && report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize) {
            amk_led_state = buffer[0];
            //amk_printf("Set Report Data: size=%d, state=%x\n", bufsize, buffer[0]);
        }
    }

#ifdef VIAL_ENABLE
    if (itf == ITF_NUM_VIAL) {
        //amk_printf("VIAL process data: size=%d\n", bufsize);
        vial_process((uint8_t*)buffer, (uint8_t)bufsize);
    }
#endif
}
