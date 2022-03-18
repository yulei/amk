/**
 * @file hostusb.c
 */

#include <stdint.h>
#include <stdbool.h>

#include "hostusb.h"
#include "usb_common.h"
#include "hid_report_parser.h"
#include "usb_interface.h"
#include "tusb.h"
#include "amk_printf.h"

extern uint8_t amk_led_state;
static uint8_t current_dev_addr;

typedef struct {
    uint8_t dev_addr;
    uint8_t itf_num;
    uint8_t valid;
    uint8_t type;
} dev_hid_instance_t;

static dev_hid_instance_t hid_instances[ITF_MAX] = {0};

static dev_hid_instance_t* allocate_itf(uint8_t dev_addr, uint8_t instance)
{
    dev_hid_instance_t *itf = NULL;
    for (int i = 0; i < ITF_MAX; i++) {
        if (hid_instances[i].valid == 0) {
            itf = &hid_instances[i];
            itf->dev_addr = dev_addr;
            itf->itf_num = instance;
            break;
        }
    }

    return itf;
}

static dev_hid_instance_t* get_itf(uint8_t dev_addr, uint8_t instance)
{
    dev_hid_instance_t *itf = NULL;
    for (int i = 0; i < ITF_MAX; i++) {
        if ((hid_instances[i].valid != 0) && (hid_instances[i].dev_addr == dev_addr) && (hid_instances[i].itf_num == instance)){
            itf = &hid_instances[i];
            break;
        }
    }

    return itf;
}

static dev_hid_instance_t* get_itf_by_type(uint8_t dev_addr, uint8_t type)
{
    dev_hid_instance_t *itf = NULL;
    for (int i = 0; i < ITF_MAX; i++) {
        if ((hid_instances[i].valid != 0) && (hid_instances[i].dev_addr == dev_addr) && (hid_instances[i].type == type)){
            itf = &hid_instances[i];
            break;
        }
    }

    return itf;
}

static void free_itf(uint8_t dev_addr, uint8_t instance)
{
    dev_hid_instance_t *itf = get_itf(dev_addr, instance);
    if (itf) {
        itf->valid = 0;
    }
}


static void process_dev_report(dev_hid_instance_t *itf, uint8_t const *report, uint16_t len)
{
    // read data out
    static uint8_t buf[64];
    uint32_t report_size = 64 > len ? len : 64;
    memcpy(buf, report, report_size);
    uint8_t *p_buf = NULL;
    uint8_t report_id = 0;

    switch(itf->type) {
    case ITF_HID_MOUSE:
        report_id = HID_REPORT_ID_MOUSE;
        report_size = 5;
        p_buf = &buf[0];
        break;
    case ITF_HID_KEYBOARD:
        report_id = HID_REPORT_ID_KEYBOARD;
        p_buf = &buf[0];
        break;
    case ITF_HID_EXTRA: {
        report_id = HID_REPORT_ID_UNKNOWN;
        report_size = 0;
        itf_report_desc_t * ird = rp_get(itf->itf_num);
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
    case ITF_HID_RAW:
        amk_printf("HID Event: should never get data from RAW interface\n");
        break;
    default:
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

static void usb_host_update_led(void)
{
    static uint8_t prev_led = 0;
    if (prev_led != amk_led_state) {
        prev_led = amk_led_state;
        dev_hid_instance_t* itf = get_itf_by_type(current_dev_addr, ITF_HID_KEYBOARD);
        if (!itf) {
            amk_printf("can't get KEYBOARD interface\n");
            return;
        }

        if (!tuh_hid_set_report(current_dev_addr, itf->itf_num, 0, HID_REPORT_TYPE_OUTPUT, &prev_led, 1)) {
            amk_printf("failed to send out led report\n");
        }
    }
}

void usb_host_task(void)
{
    usb_host_update_led();
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    amk_printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
    amk_printf("VID = %04x, PID = %04x\r\n", vid, pid);

    dev_hid_instance_t *itf = allocate_itf(dev_addr, instance);
    if (!itf) {
        amk_printf("Cant allocat itf: dev_addr=%d, instance=%d\n", dev_addr, instance);
        return;
    }

    itf->type = ITF_HID_UNKNOWN;

    if (rp_parse(itf->itf_num, desc_report, desc_len)) {
        itf_report_desc_t * ird = rp_get(itf->itf_num);
        if (ird->desc_count == 1) {
            switch (ird->desc[0].rd_type) {
            case RDI_KEYBOARD:
                itf->type = ITF_HID_KEYBOARD;
                itf->valid = 1;
                break;
            case RDI_MOUSE:
                itf->type = ITF_HID_MOUSE;
                itf->valid = 1;
                break;
            case RDI_RAW:
                itf->type = ITF_HID_RAW;
                itf->valid = 1;
                break;
            default:
                break;
            }
        } else {
            itf->type = ITF_HID_EXTRA;
            itf->valid = 1;
        }
    }

    amk_printf("HID interface[%d]: get report descriptor, type=%d\n", itf->itf_num, itf->type);
    // request to receive report
    // tuh_hid_report_received_cb() will be invoked when report is available
    if (!tuh_hid_receive_report(dev_addr, instance))
    {
        amk_printf("Error: cannot request to receive report\r\n");
    }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    free_itf(dev_addr, instance);

    amk_printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
    dev_hid_instance_t *itf = get_itf(dev_addr, instance);
    if (!itf) {
        amk_printf("Received report at invalid itf: dev_addr=%d, instance=%d\n", dev_addr, instance);
        return;
    }

    process_dev_report(itf, report, len);

    // continue to request to receive report
    if ( !tuh_hid_receive_report(dev_addr, instance) )
    {
        amk_printf("Error: cannot request to receive report\r\n");
    }
}
