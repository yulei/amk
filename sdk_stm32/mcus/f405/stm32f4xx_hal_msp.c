/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
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
#include "amk_hal.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

#ifdef USE_I2C1
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c1_tx;
#endif

#ifdef USE_I2C2
extern DMA_HandleTypeDef hdma_i2c2_rx;
extern DMA_HandleTypeDef hdma_i2c2_tx;
#endif

#ifdef USE_SPI1
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
#endif

#ifdef USE_SPI3
extern DMA_HandleTypeDef hdma_spi3_rx;
extern DMA_HandleTypeDef hdma_spi3_tx;
#endif

#ifdef PWM_TIM
extern DMA_HandleTypeDef hdma_tim1_ch3;
#endif

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

#if defined(USE_I2C1) || defined(USE_I2C2)
/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  #ifdef USE_I2C1
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

#if 0
    /* I2C1 DMA Init */
    /* I2C1_RX Init */
    hdma_i2c1_rx.Instance = DMA1_Stream0;
    hdma_i2c1_rx.Init.Channel = DMA_CHANNEL_1;
    hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c,hdmarx,hdma_i2c1_rx);

    /* I2C1_TX Init */
    hdma_i2c1_tx.Instance = DMA1_Stream6;
    hdma_i2c1_tx.Init.Channel = DMA_CHANNEL_1;
    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c,hdmatx,hdma_i2c1_tx);
#endif
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
  #endif
  #ifdef USE_I2C2
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();

    /* I2C2 DMA Init */
    /* I2C2_RX Init */
    hdma_i2c2_rx.Instance = DMA1_Stream3;
    hdma_i2c2_rx.Init.Channel = DMA_CHANNEL_7;
    hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c,hdmarx,hdma_i2c2_rx);

    /* I2C2_TX Init */
    hdma_i2c2_tx.Instance = DMA1_Stream7;
    hdma_i2c2_tx.Init.Channel = DMA_CHANNEL_7;
    hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_i2c2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_i2c2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hi2c,hdmatx,hdma_i2c2_tx);
  }
  #endif
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  #ifdef USE_I2C1
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

    /* I2C1 DMA DeInit */
    HAL_DMA_DeInit(hi2c->hdmarx);
    HAL_DMA_DeInit(hi2c->hdmatx);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
  #endif
  #ifdef USE_I2C2
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

    /* I2C2 DMA DeInit */
    HAL_DMA_DeInit(hi2c->hdmarx);
    HAL_DMA_DeInit(hi2c->hdmatx);
  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
  #endif
}

#endif

#ifdef PWM_TIM
/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /* TIM1 DMA Init */
    /* TIM1_CH3 Init */
    hdma_tim1_ch3.Instance = DMA2_Stream6;
    hdma_tim1_ch3.Init.Channel = DMA_CHANNEL_6;
    hdma_tim1_ch3.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim1_ch3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim1_ch3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim1_ch3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim1_ch3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim1_ch3.Init.Mode = DMA_NORMAL;
    hdma_tim1_ch3.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim1_ch3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim1_ch3) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(htim_pwm,hdma[TIM_DMA_ID_CC3],hdma_tim1_ch3);

  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspPostInit 0 */

  /* USER CODE END TIM1_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PA10     ------> TIM1_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM1_MspPostInit 1 */

  /* USER CODE END TIM1_MspPostInit 1 */
  }

}
/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* TIM1 DMA DeInit */
    HAL_DMA_DeInit(htim_pwm->hdma[TIM_DMA_ID_CC3]);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }

}
#endif

/**
* @brief RTC MSP Initialization
* This function configures the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  if(hrtc->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

/**
* @brief RTC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }

}

#if defined(USE_SPI1) || defined(USE_SPI3)
/**
* @brief SPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  #ifdef USE_SPI1
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 DMA Init */
    /* SPI1_RX Init */
    hdma_spi1_rx.Instance = DMA2_Stream0;
    hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
    hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_rx.Init.Mode = DMA_NORMAL;
    hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmarx,hdma_spi1_rx);

    /* SPI1_TX Init */
    hdma_spi1_tx.Instance = DMA2_Stream3;
    hdma_spi1_tx.Init.Channel = DMA_CHANNEL_3;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmatx,hdma_spi1_tx);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
  #endif
  #ifdef USE_SPI3
  if(hspi->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspInit 0 */

  /* USER CODE END SPI3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**SPI3 GPIO Configuration
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* SPI3 DMA Init */
    /* SPI3_RX Init */
    hdma_spi3_rx.Instance = DMA1_Stream2;
    hdma_spi3_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi3_rx.Init.Mode = DMA_NORMAL;
    hdma_spi3_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmarx,hdma_spi3_rx);

    /* SPI3_TX Init */
    hdma_spi3_tx.Instance = DMA1_Stream5;
    hdma_spi3_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi3_tx.Init.Mode = DMA_NORMAL;
    hdma_spi3_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmatx,hdma_spi3_tx);

  /* USER CODE BEGIN SPI3_MspInit 1 */

  /* USER CODE END SPI3_MspInit 1 */
  }
  #endif
}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  #ifdef USE_SPI1
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    /* SPI1 DMA DeInit */
    HAL_DMA_DeInit(hspi->hdmarx);
    HAL_DMA_DeInit(hspi->hdmatx);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  #endif
  #ifdef USE_SPI3
  if(hspi->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspDeInit 0 */

  /* USER CODE END SPI3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI3_CLK_DISABLE();

    /**SPI3 GPIO Configuration
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);

    /* SPI3 DMA DeInit */
    HAL_DMA_DeInit(hspi->hdmarx);
    HAL_DMA_DeInit(hspi->hdmatx);
  /* USER CODE BEGIN SPI3_MspDeInit 1 */

  /* USER CODE END SPI3_MspDeInit 1 */
  }
  #endif
}
#endif
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
