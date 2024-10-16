/**
 * @file usb_interface.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "amk_hal.h"
#include "amk_eeprom.h"
#include "usb_interface.h"
#include "amk_usb.h"
#include "usb_descriptors.h"
#include "report.h"
#include "action_util.h"
#include "mousekey.h"
#include "wait.h"
#include "amk_printf.h"
#include "amk_ring_buffer.h"

#ifndef USBITF_DEBUG
#define USBITF_DEBUG 0
#endif

#if USBITF_DEBUG
#define usbitf_debug  amk_printf
#else
#define usbitf_debug(...)
#endif

#ifndef REPORT_QUEUE_SIZE
#define REPORT_QUEUE_SIZE   2048
#endif

static uint8_t report_queue_buf[REPORT_QUEUE_SIZE];

static ring_buffer_t report_queue;
static bool report_play_macro;
static uint8_t report_play_macro_id;

bool usb_is_play_macro(uint8_t id)
{
    if (report_play_macro) {
        return id == report_play_macro_id;
    }

    return false;
}

void usb_break_macro(void)
{
    if (report_play_macro) {
        amk_printf("Macro break, drop all queued report\n");
        rb_init(&report_queue, report_queue_buf, REPORT_QUEUE_SIZE);
        // cleanup state
        send_keyboard_report();
        #ifdef HID_OTHER_ENABLE
        mousekey_send();
        #endif
        amk_usb_clear_busy();
        report_play_macro = false;
    }
}

void usb_init(void)
{
#ifdef USE_HS_USB
    usb_polling_rate = eeconfig_read_usb();
    amk_printf("Current polling rate is: %d\n", usb_polling_rate);
    if (usb_polling_rate != 0 && !(usb_setting&USB_MSC_BIT)) {
        usb_setting |= USB_HS_BIT;
    }
#endif

    amk_usb_init();
    report_play_macro = false;

    rb_init(&report_queue, report_queue_buf, REPORT_QUEUE_SIZE);
}

void usb_task_usb(void)
{
    amk_usb_task();
}

void usb_task_report(void)
{
    if (!amk_usb_busy() && rb_used_count(&report_queue)){
        uint8_t type = rb_peek_byte(&report_queue);
        if (amk_usb_itf_ready(type)) {
            usbitf_debug("ITF ready, type:%d, send report\n", item->type);
            uint32_t size = 0;
            switch(type) {
            case HID_REPORT_ID_KEYBOARD:
                size = KEYBOARD_REPORT_SIZE;
            break;
            case HID_REPORT_ID_MOUSE:
                size = sizeof(report_mouse_t);
            break;
            case HID_REPORT_ID_SYSTEM:
                size = sizeof(uint16_t);
            break;
            case HID_REPORT_ID_CONSUMER:
                size = sizeof(uint16_t);
            break;
            #ifdef NKRO_ENABLE
            case HID_REPORT_ID_NKRO:
                size = sizeof(struct nkro_report);
            break;
            #endif
            case HID_REPORT_ID_DELAY:
                size = sizeof(uint16_t);
            break;
            case HID_REPORT_ID_MACRO_BEGIN:
                size = 1;
                break;
            case HID_REPORT_ID_MACRO_END:
                size = 1;
                break;
            default:
                amk_printf("usb task report: can't be here, drop all data\n");
                rb_init(&report_queue, report_queue_buf, REPORT_QUEUE_SIZE);
            break;
            }
            if (size != 0) {
                static uint8_t data[64];
                type = rb_read_byte(&report_queue);
                rb_read(&report_queue, data, size);
                if (type == HID_REPORT_ID_MACRO_BEGIN) {
                    report_play_macro = true;
                    report_play_macro_id = data[0];
                } else if (type == HID_REPORT_ID_MACRO_END) {
                    report_play_macro = false;
                    amk_printf("usb task report: macro(%d) end\n", data[0]);
                } else {
                    amk_usb_itf_send_report(type, data, size);
                }
            }
        } else {
            usbitf_debug("ITF not ready\n");
        }
    }
}

void usb_task(void)
{
    usb_task_report();
    usb_task_usb();
}

bool usb_ready(void)
{
    return amk_usb_ready();
}

bool usb_suspended(void)
{
    return amk_usb_suspended();
}

void usb_remote_wakeup(void)
{
    rb_init(&report_queue, report_queue_buf, REPORT_QUEUE_SIZE);

    amk_usb_remote_wakeup();
}

void usb_connect(bool on)
{
    amk_usb_connect(on);
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    if (usb_setting & USB_NO_OUTPUT) return; 

    rb_write_byte(&report_queue, report_type);
    rb_write(&report_queue, data, size);
}
