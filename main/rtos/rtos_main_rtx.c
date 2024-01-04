/**
 * @file rtos_main_rtx.c
 */

#include "cmsis_os2.h"
#include "rtx_os.h"
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
#define USB_THREAD_STACK        512
#define USB_THREAD_PRIORITY     osPriorityHigh
#define MAIN_THREAD_STACK       5888//1024
#define MAIN_THREAD_PRIORITY    osPriorityNormal
#define AUX_THREAD_STACK        768
#define AUX_THREAD_PRIORITY     osPriorityNormal

static osThreadId_t usb_thread_id;
static uint32_t usb_thread_cb[WORDS(sizeof(osRtxThread_t))];
static uint64_t usb_thread_stack[USB_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t usb_thread_attr = {
    .name = "usb",
    .cb_mem = usb_thread_cb,
    .cb_size = sizeof(usb_thread_cb),
    .stack_mem = usb_thread_stack,
    .stack_size = sizeof(usb_thread_stack),
    .priority = USB_THREAD_PRIORITY,
};

static osThreadId_t main_thread_id;
static uint32_t main_thread_cb[WORDS(sizeof(osRtxThread_t))];
static uint64_t main_thread_stack[MAIN_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t main_thread_attr = {
    .name = "main",
    .cb_mem = main_thread_cb,
    .cb_size = sizeof(main_thread_cb),
    .stack_mem = main_thread_stack,
    .stack_size = sizeof(main_thread_stack),
    .priority = MAIN_THREAD_PRIORITY,
};

static osThreadId_t aux_thread_id;
static uint32_t aux_thread_cb[WORDS(sizeof(osRtxThread_t))];
static uint64_t aux_thread_stack[AUX_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t aux_thread_attr = {
    .name = "aux",
    .cb_mem = aux_thread_cb,
    .cb_size = sizeof(aux_thread_cb),
    .stack_mem = aux_thread_stack,
    .stack_size = sizeof(aux_thread_stack),
    .priority = AUX_THREAD_PRIORITY,
};

static void usb_thread(void *arg);
static void main_thread(void *arg);
static void aux_thread(void *arg);

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    osKernelInitialize();

    usb_thread_id = osThreadNew(usb_thread, NULL, &usb_thread_attr);
    main_thread_id = osThreadNew(main_thread, NULL, &main_thread_attr);
    aux_thread_id = osThreadNew(aux_thread, NULL, &aux_thread_attr);

    osKernelStart();

    // never reach here
    for (;;) {}

    return 0;
}

void usb_thread(void *arg)
{
    usb_init();
    while (1) {
        usb_task_usb();
    }
}

void main_thread(void *arg)
{
    amk_printf("amk_init\n");
    amk_driver_init();
    amk_printf("board_init end\n");

    while(1) {
        usb_task_report();
        amk_driver_task();
        osThreadFlagsSet(aux_thread_id, FLAGS_MAIN_AUX);
        osThreadYield();
    }
}

void aux_thread(void *arg)
{
    while (1) {
        osThreadFlagsWait(FLAGS_MAIN_AUX, osFlagsWaitAny, osWaitForever);

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
