/**
 * generic_stm32f4.c
 */

#include "stm32f4xx_hal.h"
#include "board.h"
#include "rtt.h"
#include "usb_descriptors.h"
#include "usb_device.h"
#include "usbd_hid.h"

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "suspend.h"
#include "action_util.h"
#include "mousekey.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

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

// enable all gpio clock
static void gpio_rcc_clk_enable(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 96000000
  *            HCLK(Hz)                       = 96000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 384
  *            PLL_P                          = 4
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 2
  *            RTCSRC                         = LSI
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 384;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        rtt_printf("Failed to config RCC\n");
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        rtt_printf("Failed to config Clock\n");
        Error_Handler();
    }
}

void board_init(void)
{
    HAL_Init();
    DWT_Delay_Init();

    SystemClock_Config();
    
    gpio_rcc_clk_enable();

    MX_USB_DEVICE_Init();

    amk_init();
}

void board_task(void)
{
    switch (hUsbDeviceFS.dev_state) {
    case USBD_STATE_CONFIGURED:
        keyboard_task();
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
}

static void amk_init(void)
{
    keyboard_init();
    host_set_driver(&amk_driver);
}

void Error_Handler(void)
{
  __asm__("BKPT");
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