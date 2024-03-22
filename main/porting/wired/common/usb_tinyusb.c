/**
 * usb_tinyusb.c
 */

#include "amk_hal.h"

#include "tusb.h"

#include "amk_usb.h"
#include "usb_descriptors.h"
#include "report.h"
#include "wait.h"
#include "amk_printf.h"

#ifdef VIAL_ENABLE
#include "raw_hid.h"
#endif

#ifdef HAL_HOST_ENABLE
#include "usb_host.h"
#include "usbh_def.h"
#include "usbh_hid_multi.h"
bool usb_led_event = false;
#endif

#define PRINT_REPORT

void amk_usb_init(void)
{
    tusb_init();
#ifdef HAL_HOST_ENABLE
    MX_USB_HOST_Init();
#endif
}

void amk_usb_task(void)
{
    tud_task();
#ifdef CDC_ENABLE
    void cdc_task(void);
    cdc_task();
#endif
#ifdef HAL_HOST_ENABLE
    MX_USB_HOST_Process();
#endif
}

bool amk_usb_itf_ready(uint32_t type)
{
    switch(type) {
#ifdef KEYBOARD_ENABLE
    case HID_REPORT_ID_KEYBOARD:
    case HID_REPORT_ID_MACRO:
        return tud_hid_n_ready(ITF_NUM_HID_KBD);
#endif

#ifdef HID_OTHER_ENABLE
    case HID_REPORT_ID_MOUSE:
    case HID_REPORT_ID_SYSTEM:
    case HID_REPORT_ID_CONSUMER:
    case HID_REPORT_ID_NKRO:
        return tud_hid_n_ready(ITF_NUM_HID_OTHER);
#endif
    case HID_REPORT_ID_DELAY:
        return true;
    default:
        break;
    }
    return false;
}

__attribute__((weak))
void hook_send_report(uint32_t type, const void* data)
{
}

bool amk_usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
    //return true;

    hook_send_report(report_type, data);

    switch(report_type) {
#ifdef KEYBOARD_ENABLE
    case HID_REPORT_ID_KEYBOARD:
        if (!tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size)) {
            amk_printf("failed to sent keyboard report\n");
            return false;
        }
#ifdef PRINT_REPORT
        {
            uint8_t * p = (uint8_t*)data;
            amk_printf("Key:%x-%x-%x-%x-%x-%x-%x-%x\n", p[0],p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
        }
#endif
        break;
#endif

#ifdef HID_OTHER_ENABLE
    case HID_REPORT_ID_MOUSE:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, data, (uint8_t)size)) {
            amk_printf("failed to sent mouse report\n");
            return false;
        }
#ifdef PRINT_REPORT
        {
            report_mouse_t* mouse = (report_mouse_t*)data;
            amk_printf("Mouse: size=%d, sizeofmouse=%d, button:%x, x:%d, y:%d, vertical wheel:%d, horizontal wheel:%d\n",
                    size, sizeof(report_mouse_t), mouse->buttons, mouse->x, mouse->y, mouse->v, mouse->h);
        }
#endif
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
    case HID_REPORT_ID_NKRO:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_NKRO, data, (uint8_t)size)) {
            amk_printf("failed to sent nkro report\n");
            return false;
        }
        break;
#endif
    case HID_REPORT_ID_DELAY:
        {
            uint16_t delay = *((uint16_t*)data);
            amk_printf("Delay report: duration=%d\n", delay);
            //while(delay--) {
                wait_ms(delay);
            //}
        }
        break;
    default:
        amk_printf("unknonw report type: %ld\n", report_type);
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
__attribute__((weak))
void usb_state_kb(uint8_t state) {}

void tud_mount_cb(void)
{
    usb_state_kb(USB_STATE_MOUNTED);
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    usb_state_kb(USB_STATE_UNMOUNTED);
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    usb_state_kb(USB_STATE_SUSPENDED);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    usb_state_kb(USB_STATE_RESUMED);
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
        amk_printf("Vial GetReport Data: size=%d\n", reqlen);
        return 0;
    }
#endif
    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
__attribute__((weak))
bool vial_process_kb(uint8_t *data, uint8_t length)
{
    return false;
}

extern uint8_t amk_led_state;
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    //amk_printf("Set Report: itf=%d, id=%d, type=%d\n", itf, report_id, report_type);
    (void) report_id;
#ifdef KEYBOARD_ENABLE
    if (itf == ITF_NUM_HID_KBD && report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize) {
            amk_led_state = buffer[0];
            //amk_printf("Set Report Data: size=%d, state=%x\n", bufsize, buffer[0]);
            #ifdef HAL_HOST_ENABLE
            usb_led_event = true;
            #endif

        }
    }
#endif

#ifdef VIAL_ENABLE
    if (itf == ITF_NUM_VIAL) {
        //amk_printf("VIAL process data: size=%d\n", bufsize);
       if (vial_process_kb((uint8_t*)buffer, (uint8_t)bufsize)) {
            return;
       }

       raw_hid_receive((uint8_t*)buffer, (uint8_t)bufsize);
    }
#endif
}

#ifdef CDC_ENABLE
void cdc_output(const char* buf, uint32_t size)
{
    tud_cdc_write(buf, size);
    tud_cdc_write_flush();
}

void cdc_task(void)
{
    if ( tud_cdc_available() )
    {
      // read datas
        static char buf[256];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));
        (void) count;

        amk_printf("%s\n", buf);
    }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void) itf;
    (void) rts;
    // TODO set some indicator
    if ( dtr )
    {
        // Terminal connected
    }else
    {
        // Terminal disconnected
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
    (void) itf;
}

#endif

#ifdef HAL_HOST_ENABLE

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *phid = (HID_HandleTypeDef *) phost->pActiveClass->pData;
    HID_InterfaceTypeDef *itf = &phid->itfs[phid->current];
    amk_printf("HID event, itf=%d, ready=%d, size=%d, type=%d\n", phid->current, itf->data_ready, itf->report_size, itf->type);

    // read data out
    static uint8_t buf[64];
    uint8_t *p_buf = NULL;
    uint8_t report_id = 0;
    uint8_t report_size = 0;
    memset(&buf[0], 0, sizeof(buf));
    switch(itf->type) {
    case HID_MOUSE:
        report_id = HID_REPORT_ID_MOUSE;
        //buf[0] = report_id;
        USBH_HID_FifoRead(&itf->fifo, &buf[0], itf->report_size);
        report_size = 5;
        p_buf = &buf[0];
        break;
    case HID_KEYBOARD:
        report_id = HID_REPORT_ID_KEYBOARD;
        report_size = itf->report_size;
        p_buf = &buf[0];
        USBH_HID_FifoRead(&itf->fifo, &buf[0], itf->report_size);
        break;
    case HID_EXTRA: {
        report_id = HID_REPORT_ID_UNKNOWN;
        report_size = 0;
        itf_report_desc_t * ird = rp_get(phid->current);
        USBH_HID_FifoRead(&itf->fifo, &buf[0], itf->report_size);
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
    case HID_RAW:
        USBH_HID_FifoRead(&itf->fifo, buf, itf->report_size);
        amk_printf("HID Event: should never get data from RAW interface\n");
        break;
    default:
        USBH_HID_FifoRead(&itf->fifo, buf, itf->report_size);
        amk_printf("HID Event: unknown interface\n");
        break;
    }
    if (report_id == HID_REPORT_ID_UNKNOWN) {
        // do nothing
        amk_printf("HID Event: unknown report\n");
        return;
    }

    amk_usb_itf_send_report(report_id, p_buf, report_size);
}
#endif