/**
 * @file gd32_tinyusb.c
 * 
 */

#include "amk_usb.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "drv_usb_hw.h"
#include "tusb.h"
#include "amk_printf.h"

#include "report.h"
#include "wait.h"

void amk_usb_init(void)
{
    //usb_rcu_config();

#if defined(USB_HOST_ENABLE) || defined(USB_DEVICE_ENABLE)
    //usb_vbus_config();
    tusb_init();
#endif

    //usb_intr_config();
}

void amk_usb_task(void)
{
#ifdef USB_HOST_ENABLE
    tuh_task();
#endif

#ifdef USB_DEVICE_ENABLE
    tud_task();
#endif
    //amk_printf("amk usb task\n");
}

bool amk_usb_itf_ready(uint32_t type)
{
    return true;
}

bool amk_usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
    return true;
}

bool amk_usb_ready(void)
{
    return true;
}

bool amk_usb_suspended(void)
{
    return false;
}

void amk_usb_remote_wakeup(void)
{
}

void amk_usb_connect(bool on)
{
}

#ifdef USB_HOST_ENABLE

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
    (void)desc_report;
    (void)desc_len;
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    amk_printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
    amk_printf("VID = %04x, PID = %04x\r\n", vid, pid);

    tuh_hid_receive_report(dev_addr, instance);
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    amk_printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
    amk_printf("HID report received:\n");
    for (int i = 0; i < len; i++) {
        amk_printf("%02X", report[i]);
        if( i!=len-1) amk_printf("-");
    }
    amk_printf("\n");

    tuh_hid_receive_report(dev_addr, instance);
}
#endif

#ifdef USB_DEVICE_ENABLE

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

#endif