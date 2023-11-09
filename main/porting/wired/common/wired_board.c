/**
 * generic_board.c
 */

#include "amk_hal.h"
#include "board.h"
#include "usb_interface.h"

#include "amk_driver.h"
#include "amk_profile.h"
#include "amk_printf.h"
#include "SEGGER_SYSVIEW.h"

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

#ifndef GENERIC_BOARD_DEBUG
#define GENERIC_BOARD_DEBUG 1
#endif

#if GENERIC_BOARD_DEBUG
#define gb_debug amk_printf 
#else
#define gb_debug(...)
#endif


extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

void board_init(void)
{

    system_init();
    gb_debug("system_init\n");

    custom_board_init();
    gb_debug("custom_board_init\n");

    usb_init();
    gb_debug("usb_init\n");

    amk_driver_init();
    gb_debug("amk_init\n");

#ifndef CH32V307
    SEGGER_SYSVIEW_Conf();
    gb_debug("segger sysview init\n");
    SEGGER_SYSVIEW_NameResource(1, "amk_driver");
    SEGGER_SYSVIEW_NameResource(2, "usb_task");
    SEGGER_SYSVIEW_NameResource(3, "custom_task");
#endif
}


void board_task(void)
{
#ifndef CH32V307
    SEGGER_SYSVIEW_MarkStart(1);
#endif
    amk_driver_task();
#ifndef CH32V307
    SEGGER_SYSVIEW_MarkStop(1);
#endif


#ifndef RTOS_ENABLE
#ifndef CH32V307
    SEGGER_SYSVIEW_MarkStart(2);
#endif
    usb_task();
#ifndef CH32V307
    SEGGER_SYSVIEW_MarkStop(2);
#endif
#endif

#if defined(MSC_ENABLE) && !defined(RTOS_ENABLE)
    msc_task();
#endif

#ifdef AUDIO_ENABLE
    audio_task();
#endif

#ifdef SCREEN_ENABLE
    render_task();
#endif

#ifdef RGB_ENABLE
    rgb_led_task();
#endif

#ifdef RF_ENABLE
    rf_driver_task();
#endif

#ifndef CH32V307
    SEGGER_SYSVIEW_MarkStart(3);
#endif
    custom_board_task();
#ifndef CH32V307
    SEGGER_SYSVIEW_MarkStop(3);
#endif
}
