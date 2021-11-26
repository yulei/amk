/**
 * generic_board.c
 */

#include "generic_hal.h"
#include "board.h"
#include "usb_descriptors.h"
#include "usb_interface.h"
#include "amk_keymap.h"
#include "amk_printf.h"

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "suspend.h"
#include "action_util.h"
#include "mousekey.h"
#include "wait.h"
#include "amk_boot.h"

#ifdef SCREEN_ENABLE
#include "screen.h"
#endif

#ifdef WEBUSB_ENABLE
#include "webusb.h"
#endif

#ifdef MSC_ENABLE
#include "mscusb.h"
#endif

#ifdef RGB_ENABLE
#include "rgb_led.h"
#endif

#ifdef RF_ENABLE
#include "rf_driver.h"
#endif

uint32_t usb_setting = 0;

extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

/**
 * tmk related stuff
 */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

uint8_t amk_led_state = 0;
uint8_t amk_macro_state = 0;
uint32_t amk_macro_delay = 0;

static void remote_wakeup(void);

/* host struct */
host_driver_t amk_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

static void amk_init(void);

void board_init(void)
{
    amk_printf("system_init\n");
    system_init();
    amk_printf("custom_board_init\n");
    custom_board_init();
#ifndef RTOS_ENABLE
    amk_printf("usb_init\n");
    usb_init();
#endif
    amk_printf("amk_init\n");
    amk_init();
    amk_printf("board_init end\n");

#ifdef RF_ENABLE
    rf_driver_init(true);
    amk_printf("rf_driver_initend\n");
#endif
}


void board_task(void)
{
#ifndef RTOS_ENABLE
    usb_task();
#endif

    if (usb_suspended()) {
        if (suspend_wakeup_condition()) {
            // wake up remote
            remote_wakeup();
            //usb_connect(false);
            wait_ms(100);
            //usb_connect(true);
        }
    } else {
        //if (usb_ready()) {
            keyboard_task();
        //}
    }

#ifdef SCREEN_ENABLE
    screen_task();
#endif

#if defined(MSC_ENABLE) && !defined(RTOS_ENABLE)
    msc_task();
#endif

#ifdef WEBUSB_ENABLE
    webusb_task();
#endif

#ifdef RGB_ENABLE
    rgb_led_task();
#endif

#ifdef RF_ENABLE
    rf_driver_task();
#endif

    custom_board_task();
}

static void amk_init(void)
{
#if defined(MSC_ENABLE) && !defined(RTOS_ENABLE)
    amk_printf("msc_init\n");
    msc_init();
#endif

    amk_printf("keyboard_init\n");
    keyboard_init();
    boot_init();
    host_set_driver(&amk_driver);
    amk_keymap_init();
#ifdef RF_ENABLE
    rf_driver_init(true);
#endif
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
        usb_send_report(HID_REPORT_ID_KEYBOARD, report, sizeof(report_keyboard_t));
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
        usb_send_report(HID_REPORT_ID_MOUSE, report, sizeof(report_mouse_t));
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
        usb_send_report(HID_REPORT_ID_SYSTEM, &data, sizeof(data));
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
        usb_send_report(HID_REPORT_ID_CONSUMER, &data, sizeof(data));
    }
}

void remote_wakeup(void)
{
    usb_remote_wakeup();

    suspend_wakeup_init();
    // cleanup the host keyboard state
    send_keyboard_report();
    mousekey_send();
}
