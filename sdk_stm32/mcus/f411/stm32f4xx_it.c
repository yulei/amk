/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "amk_hal.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "amk_utils.h"
#include "amk_printf.h"
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

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

#ifdef USE_SPI2

extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;
/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
//void DMA1_Stream3_IRQHandler(void)
//{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
//  HAL_DMA_IRQHandler(&hdma_spi2_rx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
//}

/**
  * @brief This function handles DMA1 stream4 global interrupt.
  */
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream4_IRQn 0 */

  /* USER CODE END DMA1_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
  /* USER CODE BEGIN DMA1_Stream4_IRQn 1 */

  /* USER CODE END DMA1_Stream4_IRQn 1 */
}
#endif

#ifdef USE_SPI1
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

#endif

#ifdef USE_UART1

/* USER CODE BEGIN 1 */
extern UART_HandleTypeDef huart1;
__attribute__((weak)) void uart_recv_char(uint8_t c){}

void uart_error_process(UART_HandleTypeDef * huart, uint32_t sr)
{
    amk_printf("UART ERROR: 0x%x\n", sr);
    /* UART frame error interrupt occurred --------------------------------------*/
    if ((sr & USART_SR_FE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if ((sr & USART_SR_NE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if ((sr & USART_SR_ORE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
    }
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  uint32_t sr = huart1.Instance->SR;
  uint32_t cr1 = huart1.Instance->CR1;
  if (((sr & USART_SR_RXNE) != 0) && ((cr1 & USART_CR1_RXNEIE) != 0))
  {// received one char
    uint8_t d = huart1.Instance->DR & 0x000000FF;
    uart_recv_char(d);
  }

  if (sr & (uint32_t)( USART_SR_FE | USART_SR_ORE | USART_SR_NE )) {
    uart_error_process(&huart1, sr);
  }
  /* USER CODE END USART1_IRQn 0 */
  //HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
#endif

#ifdef USE_UART2

/* USER CODE BEGIN 1 */
extern UART_HandleTypeDef huart2;
__attribute__((weak)) void uart_recv_char(uint8_t c){}

void uart_error_process(UART_HandleTypeDef * huart, uint32_t sr)
{
    amk_printf("UART ERROR: 0x%x\n", sr);
    /* UART frame error interrupt occurred --------------------------------------*/
    if ((sr & USART_SR_FE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if ((sr & USART_SR_NE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if ((sr & USART_SR_ORE) != 0U) {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
    }
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  uint32_t sr = huart2.Instance->SR;
  uint32_t cr1 = huart2.Instance->CR1;
  if (((sr & USART_SR_RXNE) != 0) && ((cr1 & USART_CR1_RXNEIE) != 0))
  {// received one char
    uint8_t d = huart2.Instance->DR & 0x000000FF;
    uart_recv_char(d);
  }

  if (sr & (uint32_t)( USART_SR_FE | USART_SR_ORE | USART_SR_NE )) {
    uart_error_process(&huart2, sr);
  }
  /* USER CODE END USART1_IRQn 0 */
  //HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
#endif
#ifdef USE_PWM_TIM4
extern DMA_HandleTypeDef hdma_tim4_ch;

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
void PWM_DMA_IRQHnadler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim4_ch);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

#endif

#ifdef USE_PWM_TIM2
extern DMA_HandleTypeDef PWM_DMA_TIM_CH;

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
void PWM_DMA_IRQHnadler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&PWM_DMA_TIM_CH);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

#endif
#ifdef USE_PWM_TIM1
extern DMA_HandleTypeDef hdma_tim1_ch1;

void DMA2_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

  /* USER CODE END DMA2_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);
  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

  /* USER CODE END DMA2_Stream1_IRQn 1 */
}
#endif
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
