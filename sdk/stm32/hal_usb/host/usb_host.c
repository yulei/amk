/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_hid_multi.h"
#include "usbh_msc.h"
#include "usb_led.h"
#include "usb_common.h"
#include "amk_printf.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostHS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
static void USBH_UpdateLedState(USBH_HandleTypeDef *phost);
USBH_HandleTypeDef* MX_USB_HOST_Handle(void)
{
  return &hUsbHostHS;
}
bool MX_USB_HOST_MSC_Ready(void)
{
  return Appli_state == APPLICATION_READY;
}

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
 #ifdef USE_HS_USB
 #define CUR_HOST_ID  HOST_FS
 #else
 #define CUR_HOST_ID  HOST_HS
 #endif
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostHS, USBH_UserProcess, CUR_HOST_ID) != USBH_OK)
  {
    Error_Handler();
  }
#ifdef HOST_MSC_ENABLE
  if (usb_setting & USB_MSC_BIT) {
    if (USBH_RegisterClass(&hUsbHostHS, USBH_MSC_CLASS) != USBH_OK)
    {
      Error_Handler();
    }

  } else 
#endif
  {
    if (USBH_RegisterClass(&hUsbHostHS, USBH_HID_CLASS) != USBH_OK)
    {
      Error_Handler();
    }
  }
  if (USBH_Start(&hUsbHostHS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

void USBH_UpdateLedState(USBH_HandleTypeDef *phost)
{
    if (Appli_state == APPLICATION_READY) {
        if (usb_led_event) {
            if (USBH_OK == USBH_HID_SetReport(&hUsbHostHS, 0x02, 0, &amk_led_state, 1u, 0)) {
                usb_led_event = false;
            }
        }
    }
}

/*
 * Background task
 */

__attribute__((weak)) void usbh_task_kb(USBH_HandleTypeDef *phost) {}
void MX_USB_HOST_Process(void)
{
    /* USB Host Background task */
    USBH_Process(&hUsbHostHS);
    USBH_UpdateLedState(&hUsbHostHS);

    usbh_task_kb(&hUsbHostHS);
}


__attribute__((weak)) void usbh_user_process_kb(USBH_HandleTypeDef *phost, uint8_t id) {}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  amk_printf("Host select configuration\n");
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  amk_printf("Host disconnection\n");
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  amk_printf("Host activate\n");
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  amk_printf("Host connection\n");
  break;

  default:
  break;
  }

  usbh_user_process_kb(phost, id);
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
