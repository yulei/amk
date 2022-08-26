/**
 * @file freertos_main.c
 */

#include "generic_hal.h"
#include "board.h"
#include "usb_interface.h"
#include "amk_driver.h"
#include "amk_utils.h"
#include "amk_printf.h"

#include "FreeRTOS.h"

extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

// flags
#define FLAGS_MAIN_1MS      (1<<0)
#define FLAGS_MAIN_10MS     (1<<0)

// static timer
StaticTimer_t task1ms_tmdef;
xTimerHandle_t task1ms_tm;

// static usb task 
#define USB_TASK_STACK        (configMINIMAL_STACK_SIZE*2)
StackType_t  usb_task_stack[USB_TASK_STACK];
StaticTask_t usb_task_taskdef;

// static main task 
#define MAIN_TASK_STACK       (configMINIMAL_STACK_SIZE*4)
StackType_t  main_stack[MAIN_TASK_STACK];
StaticTask_t main_taskdef;

static void usb_task(void *arg);
static void main_task(void *arg);
static void timer_task_1ms(void *arg);

int main(int argc, char ** argv)
{
    // initialize board
    board_init();

    // soft timer for blinky
    task1ms_tm = xTimerCreateStatic(NULL, pdMS_TO_TICKS(1), true, NULL, timer_task_1ms, &task1ms_tmdef);
    xTimerStart(task1ms_tm, 0);

    // Create a task for tinyusb device stack
    xTaskCreateStatic(usb_task, "usbd", USB_TASK_STACK, NULL, configMAX_PRIORITIES-1, usb_task_stack, &usb_task_taskdef);

    // Create main task
    xTaskCreateStatic(main_task, "hid", MAIN_TASK_STACK, NULL, configMAX_PRIORITIES-2, main_stack, &main_taskdef);

    vTaskStartScheduler();

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
    uint32_t flags = 0;
    
    while(1) {
        xTaskNotifyWait(0, 
                        FLAGS_MAIN_1MS          // 1ms tick
                       | FLAGS_MAIN_10MS        // 10ms tick
                       , &flags 
                       , portMAX_DELAY);

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

//    amk_printf("usb_init\n");
//    usb_init();
//    usb_init_post();

    amk_printf("amk_init\n");
    amk_driver_init();
    amk_printf("board_init end\n");
}

static void task_flag_set(StaticTask_t *task, uint32_t flag)
{
    BaseType_t yield;
    uint32_t rflags = (uint32_t)osError;

    if (IS_IRQ_MODE()) {
        BaseType_t yield = pdFALSE;

        xTaskNotifyFromISR(task, flag, eSetBits, &yield);
        xTaskNotifyAndQueryFromISR(task, 0, eNoAction, &rflags, NULL);

        portYIELD_FROM_ISR(yield);
    } else {
        xTaskNotify(task, flags, eSetBits);
        xTaskNotifyAndQuery(task, 0, eNoAction, &rflags);
    }
}

void timer_task_1ms(void *arg)
{
    static uint32_t i = 0;
    task_flag_set(&main_taskdef, FLAGS_MAIN_1MS);
    if (!(i++ % 10)) {
        task_flag_set(&main_taskdef, FLAGS_MAIN_10MS);
    }
}