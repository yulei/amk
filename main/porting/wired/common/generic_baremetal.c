/**
 * generic_board.c
 */

#include "generic_hal.h"
#include "board.h"
#include "usb_descriptors.h"
#include "usb_interface.h"

#include "amk_driver.h"
#include "amk_profile.h"
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

#ifndef GENERIC_BOARD_DEBUG
#define GENERIC_BOARD_DEBUG 1
#endif

#if GENERIC_BOARD_DEBUG
#define gb_debug  amk_printf
#else
#define gb_debug(...)
#endif


extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

__attribute__((weak))
void usb_init_post(void)
{}
void board_init(void)
{
    gb_debug("system_init\n");
    system_init();

    gb_debug("custom_board_init\n");
    custom_board_init();

    gb_debug("usb_init\n");
    usb_init();
    usb_init_post();

    gb_debug("amk_init\n");
    amk_driver_init();

    amk_printf("board_init end\n");
}


void board_task(void)
{
    uint32_t region = AMK_PROFILE_BEGIN("board_task");

    amk_driver_task();
    AMK_PROFILE_SAMPLE("amk_driver", region);

#ifndef RTOS_ENABLE
    usb_task();
    AMK_PROFILE_SAMPLE("usb_task", region);
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

    custom_board_task();
    AMK_PROFILE_SAMPLE("custom_task", region);

    AMK_PROFILE_END(region);
}
