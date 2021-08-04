/**
 * usb_tinyusb.c
 */

#include "usb_interface.h"
#include "usb_descriptors.h"
#include "generic_hal.h"
#include "report_queue.h"
#include "report.h"
#include "wait.h"
#include "amk_keymap.h"
#include "amk_printf.h"

#ifdef VIAL_ENABLE
#include "vial_porting.h"
#include "vial_macro.h"
#endif

static hid_report_queue_t report_queue;

#ifdef VIAL_ENABLE
static hid_report_queue_t macro_queue;
extern uint8_t amk_macro_state;
extern uint32_t amk_macro_delay;
static void process_macro(hid_report_queue_t *queue);
#endif

static bool usb_itf_ready(uint32_t type);
static bool usb_itf_send_report(uint32_t type, const void* data, uint32_t size);
static void process_report_queue(hid_report_queue_t *queue);

void usb_init(void)
{
    tusb_init();
    hid_report_queue_init(&report_queue);

#ifdef VIAL_ENABLE
    hid_report_queue_init(&macro_queue);
#endif
}

void usb_task(void)
{
#ifdef VIAL_ENABLE
    if (!hid_report_queue_empty(&macro_queue)) {
        process_report_queue(&macro_queue);
    } else 
#endif
    if (!hid_report_queue_empty(&report_queue)) {
#ifdef VIAL_ENABLE
        hid_report_t* item = hid_report_queue_peek(&report_queue);
        if (item->type == HID_REPORT_ID_MACRO) {
            process_macro(&report_queue);
        } else
#endif
        {
            process_report_queue(&report_queue);
        }
    }

#ifdef VIAL_ENABLE
    vial_task();
#endif
    tud_task();
}

static bool usb_itf_ready(uint32_t type)
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

static bool usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
    switch(report_type) {
    case HID_REPORT_ID_KEYBOARD:
        if (!tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size)) {
            amk_printf("failed to sent keyboard report\n");
            return false;
        } else {
            report_keyboard_t *report = (report_keyboard_t*)data;
            for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
                if(report->keys[i] == KC_CAPS) {
                    wait_ms(100); // for fixing caps under mac
                    amk_printf("delay 50ms after sent caps\n");
                }
            }
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

static void process_report_queue(hid_report_queue_t * queue)
{
    hid_report_t* item = hid_report_queue_peek(queue);
    if (usb_itf_ready(item->type)) {
        amk_printf("ITF ready, type:%d, send report\n", item->type);
        hid_report_t report;
        hid_report_queue_get(queue, &report);
        usb_itf_send_report(report.type, report.data, report.size);
    }
}

#ifdef VIAL_ENABLE
static void process_macro(hid_report_queue_t *queue)
{
    hid_report_t *item = hid_report_queue_peek(queue);
    amk_macro_t *macro = (amk_macro_t*)&(item->data[0]);
    macro->delay = 0;
    amk_macro_state = 1;
    if (!vial_macro_play(macro->id, &macro->offset, &macro->delay)) {
        // macro finished
        hid_report_queue_pop(queue);
    }
    if (macro->delay) {
        amk_macro_delay = macro->delay;
    }
    amk_macro_state = 0;
}
#endif

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
    hid_report_queue_init(&report_queue);
    tud_remote_wakeup();
}

void usb_connect(bool on)
{
    if(on) {
        tud_connect();
    } else {
        tud_disconnect();
    }
}

void usb_send_report(uint8_t report_type, const void* data, size_t size)
{
    hid_report_t item;
    memcpy(item.data, data, size);
    item.type = report_type;
    item.size = size;
#ifdef VIAL_ENABLE
    if (amk_macro_state) {
        item.delay = amk_macro_delay;
        amk_macro_delay = 0;
    } else
#endif
    {
        item.delay = 0;
    }

#ifdef VIAL_ENABLE
    if (amk_macro_state) {
        hid_report_queue_put(&macro_queue, &item);
    } else
#endif
    {
        hid_report_queue_put(&report_queue, &item);
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