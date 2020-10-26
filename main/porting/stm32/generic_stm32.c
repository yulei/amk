/**
 * generic_stm32.c
 */

#include "generic_hal.h"
#include "board.h"
#include "rtt.h"
#include "usb_descriptors.h"
#include "amk_keymap.h"

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
void remote_wakeup(void);
static bool usb_ready(void);

void board_init(void)
{
    HAL_Init();
    DWT_Delay_Init();

    custom_board_init();
    tusb_init();
    amk_init();
}

__attribute__((weak)) void custom_board_task(void) {}

void board_task(void)
{
    tud_task();

    if (tud_suspended()) {
        if (suspend_wakeup_condition()) {
            // wake up remote
            remote_wakeup();
        }
    } else if (tud_hid_ready()) {
        keyboard_task();
    }

#ifdef SCREEN_ENABLE
    screen_task();
#endif
    custom_board_task();
}

extern void fee_init(void);
static void amk_init(void)
{
    fee_init();
    keyboard_init();
    host_set_driver(&amk_driver);
    #ifdef SCREEN_ENABLE
    screen_init();
    #endif
    amk_keymap_init();
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

    memcpy(&report_buf[0], report, sizeof(report_keyboard_t));
    tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, report_buf, (uint8_t)sizeof(report_keyboard_t));
}

void send_mouse(report_mouse_t *report)
{
    if (!usb_ready()) return;

    memcpy(&report_buf[0], report, sizeof(report_mouse_t));
    tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, report_buf, (uint8_t)sizeof(report_mouse_t));
}

void send_system(uint16_t data)
{
    if (!usb_ready()) return;

    memcpy(&report_buf[0], &data, sizeof(data));
    tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, report_buf, (uint8_t)sizeof(data));
}

void send_consumer(uint16_t data)
{
    if (!usb_ready()) return;

    memcpy(&report_buf[0], &data, sizeof(data));
    tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, report_buf, (uint8_t)sizeof(data));
}

void remote_wakeup(void)
{
    tud_remote_wakeup();

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
    return false;
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

void USB_HP_IRQHandler(void)
{
    tud_int_handler(0);
}

void USB_LP_IRQHandler(void)
{
    tud_int_handler(0);
}

void USBWakeUp_IRQHandler(void)
{
    tud_int_handler(0);
}

//=============================
// tusb callback
//=============================
// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // TODO set LED based on CAPLOCK, NUMLOCK etc...
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}