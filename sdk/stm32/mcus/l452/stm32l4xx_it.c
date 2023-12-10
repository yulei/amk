/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "amk_hal.h"
#include "stm32l4xx_it.h"
#include "amk_printf.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
//extern PCD_HandleTypeDef hpcd_USB_FS;
extern DMA_HandleTypeDef hdma_lpuart_rx;
extern DMA_HandleTypeDef hdma_lpuart_tx;
extern UART_HandleTypeDef huart_rf;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/**
  * @brief This function handles DMA2 channel6 global interrupt.
  */
void DMA2_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel6_IRQn 0 */

  /* USER CODE END DMA2_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_lpuart_tx);
  /* USER CODE BEGIN DMA2_Channel6_IRQn 1 */

  /* USER CODE END DMA2_Channel6_IRQn 1 */
}

/**
  * @brief This function handles DMA2 channel7 global interrupt.
  */
void DMA2_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel7_IRQn 0 */

  /* USER CODE END DMA2_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_lpuart_rx);
  /* USER CODE BEGIN DMA2_Channel7_IRQn 1 */

  /* USER CODE END DMA2_Channel7_IRQn 1 */
}

/**
  * @brief This function handles LPUART1 global interrupt.
  */
__attribute__((weak)) void uart_recv_char(uint8_t c){}

void uart_clear_error(UART_HandleTypeDef * huart, uint32_t isr)
{
    amk_printf("UART ERROR: 0x%x\n", isr);
    /* UART frame error interrupt occurred --------------------------------------*/
    if ((isr & USART_ISR_PE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
    }
    if ((isr & USART_ISR_FE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if ((isr & USART_ISR_NE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if ((isr & USART_ISR_ORE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
    }
}

void uart_irq_process(UART_HandleTypeDef * huart)
{
  uint32_t isr = READ_REG(huart->Instance->ISR);
  uint32_t cr1 = READ_REG(huart->Instance->CR1);

  if (((isr & USART_ISR_RXNE) != 0) && ((cr1 & USART_CR1_RXNEIE) != 0)) {
    // received one char
    uint8_t d = huart->Instance->RDR & 0x000000FF;
    uart_recv_char(d);
  }

  if (isr & (USART_ISR_PE|USART_ISR_FE|USART_ISR_NE|USART_ISR_ORE)) {
    uart_clear_error(huart, isr);
  }


}

void LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN LPUART1_IRQn 0 */
  uart_irq_process(&huart_rf);
  /* USER CODE END LPUART1_IRQn 0 */
  //HAL_UART_IRQHandler(&hlpuart1);
  /* USER CODE BEGIN LPUART1_IRQn 1 */

  /* USER CODE END LPUART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
