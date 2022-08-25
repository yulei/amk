/**
 * @file cmos_main.c
 */

#include "cmsis_os2.h"
#include "board.h"
#include "usb_interface.h"
#include "amk_driver.h"

// flags
#define FLAGS_MAIN_1MS      (1<<0)
#define FLAGS_MAIN_10MS     (1<<0)

// thread parameters
#define USB_THREAD_STACK        512
#define USB_THREAD_PRIORITY     osPriorityHigh
#define MAIN_THREAD_STACK       1024
#define MAIN_THREAD_PRIORITY    osPriorityNormal

static osThreadId_t usb_thread_id;
static uint32_t usb_thread_cb[WORDS(sizeof(osRtxThread_t))];
static uint64_t usb_thread_stack[USB_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t usb_thread_arrt = {
    .name = "usb",
    .cb_mem = usb_thread_cb,
    .cb_size = sizeof(usb_thread_cb),
    .stack_mem = usb_thread_stack,
    .stack_size = sizeof(s_main_task_stack),
    .priority = USB_THREAD_PRIORITY,
};

static osThreadId_t main_thread_id;
static uint32_t main_thread_cb[WORDS(sizeof(osRtxThread_t))];
static uint64_t main_thread_stack[MAIN_THREAD_STACK / sizeof(uint64_t)];
static const osThreadAttr_t main_thread_attr = {
    .name = "main",
    .cb_mem = main_thread_cb,
    .cb_size = sizeof(main_thread_cb),
    .stack_mem = main_task_stack,
    .stack_size = sizeof(main_task_stack),
    .priority = MAIN_THREAD_PRIORITY,
};

static uint32_t timer_1ms_cb[WORDS(sizeof(osRtxTimer_t))];
static const osTimerAttr_t timer_1ms_attr = {
    .name = "1ms",
    .cb_mem = timer_1ms_cb,
    .cb_size = sizeof(timer_1ms_cb),
};

void usb_thread(void *arg);
void main_thread(void *arg);

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    osKernelInitialize();

    usb_thread_id = osThreadNew(usb_thread, NULL, NULL);
    main_thread_id = osThreadNew(main_thread, NULL, NULL);
    osKernelStart();

    // never reach here
    for (;;);
    return 0;
}

void usb_thread(void *arg)
{
    while (1) {
        usb_task_usb();
    }
}

void main_thread(void *arg)
{
    uint32_t flags = 0;
    osTimerId_t tmr_id = osTimerNew(timer_task_1ms, osTimerPeriodic, NULL, &timer_1ms_attr);
    osTimerStart(tmr_id, 1);
    
    while(1) {
        flags = osThreadFlagsWait(
                        FLAGS_MAIN_1MS          // 1ms tick
                       | FLAGS_MAIN_10MS        // 10ms tick
                       , osFlagsWaitAny
                       , osWaitForever);

        if (flags & FLAGS_MAIN_1MS) {
            usb_task_report();
            amk_driver_task();
        }

        if (flags & FLAGS_MAIN_10MS) {
            // other task handler
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
}

void board_init(void)
{
    amk_printf("system_init\n");
    system_init();
    amk_printf("custom_board_init\n");
    custom_board_init();

    amk_printf("usb_init\n");
    usb_init();
    usb_init_post();

    amk_printf("amk_init\n");
    amk_driver_init();
    amk_printf("board_init end\n");
}

void timer_task_1mS(void *arg)
{
    static uint32_t i = 0;
    osThreadFlagsSet(main_thread_id, FLAGS_MAIN_1MS);
    if (!(i++ % 10)) {
        osThreadFlagsSet(main_thread_id, FLAGS_MAIN_10MS);
    }
}