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
    usb_rcu_config();
    usb_vbus_config();
#ifdef USB_HOST_ENABLE
    tusb_init();
#endif
    usb_intr_config();
}

void amk_usb_task(void)
{
#ifdef USB_HOST_ENABLE
    tuh_task();
#endif
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

    //tuh_hid_receive_report(dev_addr, instance);
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    amk_printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
    //tuh_hid_receive_report(dev_addr, instance);
}
#endif
