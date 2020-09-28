/**
 * generic_stm32.c
 */

#include "generic_hal.h"
#include "board.h"
#include "rtt.h"
#include "usb_descriptors.h"
#include "usb_device.h"
#include "usbd_hid.h"

#ifdef HAL_HCD_MODULE_ENABLED
#include "usb_host.h"
#endif

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "suspend.h"
#include "action_util.h"
#include "mousekey.h"

#ifdef SCREEN_ENABLE
#include "screen.h"
#endif

extern void Error_Handler(void); 
extern USBD_HandleTypeDef hUsbDeviceFS;
extern void custom_board_init(void);

/**
 * tmk related stuff
 */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

uint8_t amk_led_state = 0;
//static bool amk_remote_wakeup = false;

/* host struct */
host_driver_t amk_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

static void DWT_Delay_Init(void);
static void amk_init(void);
static void remote_wakeup(void);
static bool usb_ready(void);

void board_init(void)
{
    HAL_Init();
    DWT_Delay_Init();

    custom_board_init();

    amk_init();
}

void board_task(void)
{
    switch (hUsbDeviceFS.dev_state) {
    case USBD_STATE_CONFIGURED:
        keyboard_task();
        #ifdef SCREEN_ENABLE
        screen_task();
        #endif
        break;
    case USBD_STATE_SUSPENDED:
        // in suspend state
        if (suspend_wakeup_condition()) {
            // wake up remote
            remote_wakeup();
        }
        break;
    case USBD_STATE_DEFAULT:
    case USBD_STATE_ADDRESSED:
    default:
        // do nothing
        break;
    }
#ifdef HAL_HCD_MODULE_ENABLED
    MX_USB_HOST_Process();
#endif
}

static void amk_init(void)
{
    extern void fee_init(void);
    fee_init();
    keyboard_init();
    host_set_driver(&amk_driver);
    #ifdef SCREEN_ENABLE
    screen_init();
    #endif
}

static void DWT_Delay_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

// tmk integration
uint8_t keyboard_leds(void)
{
    return amk_led_state;
}

#define REPORT_BUF_SIZE 16
static uint8_t report_buf[REPORT_BUF_SIZE];
void send_keyboard(report_keyboard_t *report)
{
    rtt_printf("start send keyboard\n");
    if (!usb_ready()) return;

    report_buf[0] = REPORT_ID_KEYBOARD;
    memcpy(&report_buf[1], report, sizeof(report_keyboard_t));
    USBD_HID_SendReport(&hUsbDeviceFS, report_buf, sizeof(report_keyboard_t) + 1);
}

void send_mouse(report_mouse_t *report)
{
    if (!usb_ready()) return;

    report_buf[0] = REPORT_ID_MOUSE;
    memcpy(&report_buf[1], report, sizeof(report_mouse_t));
    USBD_HID_SendReport(&hUsbDeviceFS, report_buf, sizeof(report_mouse_t) + 1);
}

void send_system(uint16_t data)
{
    if (!usb_ready()) return;

    report_buf[0] = REPORT_ID_SYSTEM;
    memcpy(&report_buf[1], &data, sizeof(data));
    USBD_HID_SendReport(&hUsbDeviceFS, report_buf, sizeof(data) + 1);
}

void send_consumer(uint16_t data)
{
    if (!usb_ready()) return;

    report_buf[0] = REPORT_ID_CONSUMER;
    memcpy(&report_buf[1], &data, sizeof(data));
    USBD_HID_SendReport(&hUsbDeviceFS, report_buf, sizeof(data) + 1);
}

void remote_wakeup(void)
{
    HAL_PCD_ActivateRemoteWakeup(hUsbDeviceFS.pData);
    // remote wakeup between 1-15ms according to the usb spec
    HAL_Delay(5);
    HAL_PCD_DeActivateRemoteWakeup(hUsbDeviceFS.pData);

    suspend_wakeup_init();
    // cleanup the host keyboard state
    send_keyboard_report();
#ifdef MOUSEKEY_ENABLE
    mousekey_send();
#endif
}

void usb_resume_cb(void)
{
    suspend_wakeup_init();
}

void usb_suspend_cb(void)
{

}

bool usb_ready(void)
{
    return hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED;
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    /* USER CODE BEGIN SysTick_IRQn 1 */
    #ifdef SCREEN_ENABLE
    screen_ticks(1);
    #endif

    /* USER CODE END SysTick_IRQn 1 */
}