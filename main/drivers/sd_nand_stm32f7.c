/**
 * @file sd_nand_stm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "timer.h"
#include "amk_printf.h"

#ifndef SD_NAND_DEBUG
#define SD_NAND_DEBUG 1
#endif

#if SD_NAND_DEBUG
#define sd_nand_debug  amk_printf
#else
#define sd_nand_debug(...)
#endif

#ifdef RTOS_ENABLE
#define SD_NAND_ASYNC    1
#endif


#define NAND_TIMEOUT    500

extern void Error_Handler(void);

SD_HandleTypeDef hsd1;
DMA_HandleTypeDef hdma_sdmmc1_rx;
DMA_HandleTypeDef hdma_sdmmc1_tx; 

static HAL_SD_CardInfoTypeDef card_info;

static void refresh_sd_info(void)
{
    HAL_SD_GetCardInfo(&hsd1, &card_info);

    sd_nand_debug("Card Type:%d, Card Version: %d, Card Class:%d\n",
        card_info.CardType, card_info.CardVersion, card_info.Class);
    sd_nand_debug("Relative Addr:%d, Block Number: %d, Block Size:%d, Logic Block Number:%d, Logic Block Size:%d\n",
        card_info.RelCardAdd, card_info.BlockNbr, card_info.BlockSize, card_info.LogBlockNbr, card_info.LogBlockSize);
}

#if SD_NAND_ASYNC
#include "tx_api.h"
static TX_SEMAPHORE sd_nand_read;
static TX_SEMAPHORE sd_nand_write;
static TX_SEMAPHORE* p_read = NULL;
static TX_SEMAPHORE* p_write = NULL;
static void sd_nand_async_init(void)
{
    if (TX_SUCCESS == tx_semaphore_create(&sd_nand_read, "sd_nand_read", 0)) {
        p_read = &sd_nand_read;
    }

    if (TX_SUCCESS == tx_semaphore_create(&sd_nand_write, "sd_nand_write", 0)) {
        p_write = &sd_nand_write;
    }
}
#endif

uint32_t sd_nand_get_block_count(void)
{
    return card_info.LogBlockNbr;
}

uint32_t sd_nand_get_block_size(void)
{
    return card_info.LogBlockSize;
}

bool sd_nand_init(void)
{
    hsd1.Instance = SDMMC1;
    hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
    hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
    hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd1.Init.ClockDiv = 3;

    HAL_SD_DeInit(&hsd1);
    if (HAL_SD_Init(&hsd1) != HAL_OK) {
        sd_nand_debug("SD NAND Init failed=0x%x\n", HAL_SD_GetError(&hsd1));
        Error_Handler();
    }

    if (HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK) {
        sd_nand_debug("SD NAND bus config failed=0x%x\n", HAL_SD_GetError(&hsd1));
        Error_Handler();
    }

    sd_nand_debug("SD NAND inited\n");
    refresh_sd_info();

    //sd_nand_erase_chip();
#if SD_NAND_ASYNC
    if (p_write == NULL || p_read == NULL) {
        sd_nand_async_init();
    }
#endif
    return true;
}

static bool check_card_state(SD_HandleTypeDef *hsd)
{
#define CHECK_STATE_TIMEOUT     1000
    uint32_t start = timer_read32();
    while( timer_elapsed32(start) < CHECK_STATE_TIMEOUT) {
        if (HAL_SD_GetCardState(hsd) == HAL_SD_CARD_TRANSFER) {
            return true;
        }
    }
    return false;
}

amk_error_t sd_nand_read_blocks(uint32_t address, uint8_t *buffer, size_t count)
{
    HAL_StatusTypeDef status = HAL_OK; 
#if SD_NAND_ASYNC
    status = HAL_SD_ReadBlocks_DMA(&hsd1, buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE);
    if (status == HAL_OK) {
        tx_semaphore_get(p_read, TX_WAIT_FOREVER);
    }
#else
    status = HAL_SD_ReadBlocks(&hsd1, buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE, NAND_TIMEOUT);
#endif

    if (status != HAL_OK) {
        sd_nand_debug("SD NAND read blocks: addr=0x%x, size=%d\n", address, count);
        sd_nand_debug("SD NAND read blocks failed: status=0x%x, error=0x%x\n", status, HAL_SD_GetError(&hsd1));
        return AMK_ERROR;
    }

    if (check_card_state(&hsd1)) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t sd_nand_write_blocks(uint32_t address, const uint8_t* buffer, size_t count)
{
    HAL_StatusTypeDef status = HAL_OK; 
#if SD_NAND_ASYNC
    status = HAL_SD_WriteBlocks_DMA(&hsd1, (uint8_t*)buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE);
    if (status == HAL_OK) {
        tx_semaphore_get(p_write, TX_WAIT_FOREVER);
    }
#else
    status = HAL_SD_WriteBlocks(&hsd1, (uint8_t*)buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE, NAND_TIMEOUT);
#endif

    if(status != HAL_OK) {
        sd_nand_debug("SD NAND write blocks: addr=0x%x, size=%d\n", address, count);
        sd_nand_debug("SD NAND write blocks failed: status=%d, error=0x%x\n", status, HAL_SD_GetError(&hsd1));
        return AMK_ERROR;
    }

    if (check_card_state(&hsd1)) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t sd_nand_erase_chip(void)
{
    if (sd_nand_get_block_count()== 0) {
        refresh_sd_info();
    }

    HAL_StatusTypeDef status = HAL_SD_Erase(&hsd1, 0, sd_nand_get_block_count()-1);
    if (status != HAL_OK) {
        sd_nand_debug("SD NAND erase chip failed: status=%d, error=0x%x\n", status, HAL_SD_GetError(&hsd1));
        return AMK_ERROR;
    }
    return AMK_SUCCESS;
}

void sd_nand_uninit(void)
{
    HAL_SD_DeInit(&hsd1);
}

// HAL 
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
#if SD_NAND_ASYNC
    if (p_read) {
        tx_semaphore_put(p_read);
    }
#endif
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
#if SD_NAND_ASYNC
    if (p_write) {
        tx_semaphore_put(p_write);
    }
#endif
}

void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(hsd->Instance==SDMMC1)
    {
    /* USER CODE BEGIN SDMMC1_MspInit 0 */

    /* USER CODE END SDMMC1_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1|RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
    PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDMMC1 DMA Init */
    /* SDMMC1_RX Init */
    hdma_sdmmc1_rx.Instance = DMA2_Stream6;
    hdma_sdmmc1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdmmc1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdmmc1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdmmc1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdmmc1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdmmc1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdmmc1_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdmmc1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdmmc1_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdmmc1_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdmmc1_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdmmc1_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdmmc1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hsd,hdmarx,hdma_sdmmc1_rx);

    /* SDMMC1_TX Init */
    hdma_sdmmc1_tx.Instance = DMA2_Stream3;
    hdma_sdmmc1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdmmc1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdmmc1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdmmc1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdmmc1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdmmc1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdmmc1_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdmmc1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdmmc1_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdmmc1_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdmmc1_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdmmc1_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdmmc1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hsd,hdmatx,hdma_sdmmc1_tx);

    /* SDMMC1 interrupt Init */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
    }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
    if(hsd->Instance==SDMMC1) {
        /* Peripheral clock disable */
        __HAL_RCC_SDMMC1_CLK_DISABLE();

        /**SDMMC1 GPIO Configuration
        PC8     ------> SDMMC1_D0
        PC9     ------> SDMMC1_D1
        PC10     ------> SDMMC1_D2
        PC11     ------> SDMMC1_D3
        PC12     ------> SDMMC1_CK
        PD2     ------> SDMMC1_CMD
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                            |GPIO_PIN_12);

        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

        /* SDMMC1 DMA DeInit */
        HAL_DMA_DeInit(hsd->hdmarx);
        HAL_DMA_DeInit(hsd->hdmatx);
    }
}

void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdmmc1_tx);
}

void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdmmc1_rx);
}

void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&hsd1);
}
