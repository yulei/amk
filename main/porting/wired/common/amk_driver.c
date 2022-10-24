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

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "suspend.h"
#include "action_util.h"
#include "mousekey.h"
#include "wait.h"
#include "amk_indicator.h"
#include "amk_boot.h"
#include "amk_profile.h"
#include "amk_keymap.h"
#include "amk_printf.h"

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

#ifdef NKRO_AUTO_ENABLE
#include "amk_action_util.h"
#endif

uint32_t usb_setting = 0;
uint8_t amk_led_state = 0;
uint8_t amk_macro_state = 0;
uint32_t amk_macro_delay = 0;

static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

static void remote_wakeup(void);

host_driver_t amk_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

void amk_driver_init(void)
{
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

    amk_printf("keyboard_init\n");
    keyboard_init();
    boot_init();
    host_set_driver(&amk_driver);
    amk_keymap_init();

#ifdef RF_ENABLE
    amk_printf("rf_driver_init\n");
    rf_driver_init(false);
#endif
    amk_indicator_init();
}

void amk_driver_task(void)
{
    if (!(usb_setting&USB_SWITCH_BIT)) {
        if (usb_suspended() ) {
            if (suspend_wakeup_condition()) {
                // wake up remote
                amk_printf("suspend_wakeup, usb_setting=%x\n", usb_setting);
                remote_wakeup();
                wait_ms(1000);
    #ifdef RTOS_ENABLE
                NVIC_SystemReset();
    #else
        #ifdef SUSPEND_RECONNECT
                usb_connect(false);
                wait_ms(100);
                usb_connect(true);
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
        rf_driver_put_report(CMD_KEY_REPORT, report, sizeof(report_keyboard_t));
    } else 
#endif
    {
        if (!usb_suspended()) {
#ifdef NKRO_AUTO_ENABLE
            amk_report_t *arp = (amk_report_t *)report;
            if (arp->nkro_mode) {
                usb_send_report(HID_REPORT_ID_NKRO, &arp->nkro.raw[0], sizeof(amk_nkro_t));
            } else {
                usb_send_report(HID_REPORT_ID_KEYBOARD, &arp->std.raw[0], sizeof(report_keyboard_t));
            }
#else
            usb_send_report(HID_REPORT_ID_KEYBOARD, report, sizeof(report_keyboard_t));
#endif
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

void send_system(uint16_t data)
{
#ifdef RF_ENABLE
    if(usb_setting & USB_OUTPUT_RF) {
        rf_driver_put_report(CMD_SYSTEM_REPORT, &data, sizeof(data));
    } else 
#endif
    {
        if (!usb_suspended()) {
            usb_send_report(HID_REPORT_ID_SYSTEM, &data, sizeof(data));
        }
    }
}

void send_consumer(uint16_t data)
{
#ifdef RF_ENABLE
    if(usb_setting & USB_OUTPUT_RF) {
        rf_driver_put_report(CMD_CONSUMER_REPORT, &data, sizeof(data));
    } else 
#endif
    {
        if (!usb_suspended()) {
            usb_send_report(HID_REPORT_ID_CONSUMER, &data, sizeof(data));
        }
    }
}

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