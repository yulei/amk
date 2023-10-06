/**
 * @file amk_driver.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generic_hal.h"
#include "amk_driver.h"

#include "usb_descriptors.h"
#include "usb_interface.h"

#include "amk_indicator.h"
#include "amk_profile.h"
#include "amk_printf.h"
#include "qmk_driver.h"

#ifdef SCREEN_ENABLE
#include "render.h"
#endif

#ifdef MSC_ENABLE
#include "mscusb.h"
#endif

#ifdef MSC_ENABLE
#include "audiousb.h"
#endif

#ifdef RGB_ENABLE
#include "rgb_led.h"
#endif

#ifdef RF_ENABLE
#include "rf_driver.h"
#endif

#ifdef NKRO_ENABLE
#include "keycode_config.h"
#endif

#ifndef SUSPEND_WAKEUP_DELAY
#define SUSPEND_WAKEUP_DELAY    0
#endif

uint32_t usb_setting = 0;
uint8_t amk_led_state = 0;
uint8_t amk_macro_state = 0;
uint32_t amk_macro_delay = 0;

static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_extra(report_extra_t *report);

#ifdef KEYBOARD_ENABLE
static void remote_wakeup(void);
#endif

host_driver_t amk_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_extra,
};

__attribute__((weak))
void amk_driver_pre_init(void)
{}

__attribute__((weak))
void amk_driver_post_init(void)
{}

void amk_driver_init(void)
{
    amk_driver_pre_init();

#ifdef MSC_ENABLE
    amk_printf("msc_init\n");
    msc_init();
#endif

#ifdef AUDIO_ENABLE
    amk_printf("audio_init\n");
    audio_init();
#endif

#ifdef SCREEN_ENABLE
    amk_printf("render_init\n");
    render_init();
#endif

#ifdef KEYBOARD_ENABLE
    amk_printf("keyboard_init\n");
    //keyboard_init();
    //boot_init();
    qmk_driver_init();
    host_set_driver(&amk_driver);
    //amk_keymap_init();
#endif

#ifdef RGB_ENABLE
    amk_printf("rgb_led_init\n");
    rgb_led_init();
#endif

#ifdef RF_ENABLE
    amk_printf("rf_driver_init\n");
    rf_driver_init(false);
#endif

    amk_indicator_init();

    amk_driver_post_init();
}

__attribute__((weak))
void amk_driver_task_kb(void)
{}

void amk_driver_task(void)
{
#ifdef KEYBOARD_ENABLE
    if (!(usb_setting&USB_SWITCH_BIT)) {
        if (usb_suspended() ) {
            if (suspend_wakeup_condition()) {
                // wake up remote
                amk_printf("suspend_wakeup, usb_setting=%lx\n", usb_setting);
                remote_wakeup();
                wait_ms(SUSPEND_WAKEUP_DELAY);
    #ifdef RTOS_ENABLE
                NVIC_SystemReset();
    #else
        #ifndef NO_SUSPEND_RECONNECT
                usb_connect(false);
                wait_ms(500);
            #ifdef SUSPEND_RESET
                NVIC_SystemReset();
            #else
                usb_connect(true);
            #endif
        #endif
    #endif
            }
        } else {
            //if (usb_ready()) {
    //            keyboard_task();
            //}
        }
    }

    keyboard_task();
#endif

    amk_driver_task_kb();
}

// tmk integration
uint8_t keyboard_leds(void)
{
    return amk_led_state;
}

void send_keyboard(report_keyboard_t *report)
{
#ifdef RF_ENABLE
    if(usb_setting & USB_OUTPUT_RF) {
    #ifdef NKRO_ENABLE
        if(keymap_config.nkro) {
            rf_driver_put_report(CMD_NKRO_REPORT, &report->nkro, sizeof(struct nkro_report));
        } else 
    #endif
        {
            rf_driver_put_report(CMD_KEY_REPORT, report, KEYBOARD_REPORT_SIZE);
        }
    } else 
#endif
    {
        if (!usb_suspended()) {
        #ifdef NKRO_ENABLE
            if(keymap_config.nkro) {
                usb_send_report(HID_REPORT_ID_NKRO, &report->nkro, sizeof(struct nkro_report));
                //amk_printf("NKRO report to queue\n");
                return;
            }
        #endif
            //amk_printf("STD report to queue\n");
            usb_send_report(HID_REPORT_ID_KEYBOARD, report, KEYBOARD_REPORT_SIZE);
        }
    }
}

void send_mouse(report_mouse_t *report)
{
#ifdef RF_ENABLE
    if(usb_setting & USB_OUTPUT_RF) {
        rf_driver_put_report(CMD_MOUSE_REPORT, report, sizeof(report_mouse_t));
    } else 
#endif
    {
        if (!usb_suspended()) {
            usb_send_report(HID_REPORT_ID_MOUSE, report, sizeof(report_mouse_t));
        }
    }
}

void send_extra(report_extra_t *report)
{
#ifdef RF_ENABLE
    if(usb_setting & USB_OUTPUT_RF) {
        if (report->report_id == REPORT_ID_CONSUMER) {
            rf_driver_put_report(CMD_CONSUMER_REPORT, &report->usage, sizeof(uint16_t));
        } else {
            rf_driver_put_report(CMD_SYSTEM_REPORT, &report->usage, sizeof(uint16_t));
        }
    } else 
#endif
    {
        if (!usb_suspended()) {
            if (report->report_id == REPORT_ID_CONSUMER) {
                usb_send_report(HID_REPORT_ID_CONSUMER, &report->usage, sizeof(uint16_t));
            } else {
                usb_send_report(HID_REPORT_ID_SYSTEM, &report->usage, sizeof(uint16_t));
            }
        }
    }
}

#ifdef KEYBOARD_ENABLE
void remote_wakeup(void)
{
    usb_remote_wakeup();

    suspend_wakeup_init();
    // cleanup the host keyboard state
    send_keyboard_report();
    #ifdef HID_OTHER_ENABLE
    mousekey_send();
    #endif
}
#endif