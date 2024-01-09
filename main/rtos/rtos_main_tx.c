/**
 * @file rtos_main_threadx.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "tx_api.h"

#include "amk_hal.h"
#include "board.h"
#include "usb_interface.h"
#include "rgb_led.h"
#include "amk_driver.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifdef RF_ENABLE
#include "rf_driver.h"
#endif

#ifdef MSC_ENABLE
#include "mscusb.h"
#endif

extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

// flags
#define FLAGS_MAIN_KEYBOARD     (1<<0)
#define FLAGS_MAIN_AUX          (1<<1)
#define FLAGS_MAIN_DISPLAY      (1<<2)

// thread parameters
#define USB_STACK_SIZE              512
#define USB_THREAD_PRIO             9
#define USB_PREEMPTION_THRESHOLD    (USB_THREAD_PRIO)
static CHAR usb_stack[USB_STACK_SIZE];

#define MAIN_STACK_SIZE             5888
#define MAIN_THREAD_PRIO            11
#define MAIN_PREEMPTION_THRESHOLD   (MAIN_THREAD_PRIO)
static CHAR main_stack[MAIN_STACK_SIZE];

#define AUX_STACK_SIZE              768
#define AUX_THREAD_PRIO             10
#define AUX_PREEMPTION_THRESHOLD    (AUX_THREAD_PRIO)
static CHAR aux_stack[AUX_STACK_SIZE];

TX_THREAD usb_thr;
static void usb_thread(ULONG thread_input);
TX_THREAD main_thr;
static void main_thread(ULONG thread_input);
TX_THREAD aux_thr;
static void aux_thread(ULONG thread_input);

TX_EVENT_FLAGS_GROUP    g_event_flags;

#define BYTE_POOL_SIZE      4096
static uint32_t g_byte_pool_mem[BYTE_POOL_SIZE/4];

TX_BYTE_POOL g_tx_byte_pool_inst;
TX_BYTE_POOL* g_tx_byte_pool;

bool rtos_running = false;

void tx_application_define(void *first_unused_memory)
{
    g_tx_byte_pool = &g_tx_byte_pool_inst;
    tx_byte_pool_create(g_tx_byte_pool, NULL, g_byte_pool_mem, BYTE_POOL_SIZE);
    tx_event_flags_create(&g_event_flags, NULL);

    tx_thread_create(&usb_thr, "usb", usb_thread, 0, usb_stack, USB_STACK_SIZE, 
                    USB_THREAD_PRIO, USB_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&main_thr, "main", main_thread, 0, main_stack, MAIN_STACK_SIZE, 
                    MAIN_THREAD_PRIO, MAIN_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&aux_thr, "aux", aux_thread, 0, aux_stack, AUX_STACK_SIZE, 
                    AUX_THREAD_PRIO, AUX_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    rtos_running = true;

    // start threadx
    tx_kernel_enter();

    // never reach here
    for (;;) {}

    return 0;
}

void usb_thread(ULONG thread_input)
{
    usb_init();
    while (1) {
        usb_task_usb();
    }
}

void main_thread(ULONG thread_input)
{
    amk_printf("amk_init\n");
    amk_driver_init();
    amk_printf("board_init end\n");

    while(1) {
        usb_task_report();
        amk_driver_task();
        tx_event_flags_set(&g_event_flags, FLAGS_MAIN_AUX, TX_OR);
        tx_thread_relinquish();
    }
}

void aux_thread(ULONG thread_input)
{
    while (1) {
        uint32_t flags = 0;
        if (TX_SUCCESS !=  tx_event_flags_get(&g_event_flags, FLAGS_MAIN_AUX, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER)) {
            continue;
        }

    #ifdef MSC_ENABLE
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
    }
}


void board_init(void)
{
    amk_printf("system_init\n");
    system_init();
    amk_printf("custom_board_init\n");
    custom_board_init();
}
