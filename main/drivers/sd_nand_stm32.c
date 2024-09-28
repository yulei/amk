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

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

static HAL_SD_CardInfoTypeDef card_info;

static void refresh_sd_info(void)
{
    HAL_SD_GetCardInfo(&hsd, &card_info);

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
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_ENABLE;
                                //SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;//+2;
    HAL_SD_DeInit(&hsd);

    if (HAL_SD_Init(&hsd) != HAL_OK) {
        sd_nand_debug("SD NAND Init failed=0x%x\n", HAL_SD_GetError(&hsd));

        Error_Handler();
    }
    if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK) {
        sd_nand_debug("SD NAND bus config failed=0x%x\n", HAL_SD_GetError(&hsd));
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
    status = HAL_SD_ReadBlocks_DMA(&hsd, buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE);
    if (status == HAL_OK) {
        tx_semaphore_get(p_read, TX_WAIT_FOREVER);
    }
#else
    status = HAL_SD_ReadBlocks(&hsd, buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE, NAND_TIMEOUT);
#endif

    if (status != HAL_OK) {
        sd_nand_debug("SD NAND read blocks: addr=0x%x, size=%d\n", address, count);
        sd_nand_debug("SD NAND read blocks failed: status=0x%x, error=0x%x\n", status, HAL_SD_GetError(&hsd));
        status = HAL_SD_Abort(&hsd);
        sd_nand_debug("SD NAND: Abort state=0x%x, status=0x%x\n, error=0x%x\n", HAL_SD_GetCardState(&hsd), status, HAL_SD_GetError(&hsd));
        return AMK_ERROR;
    }

    if (check_card_state(&hsd)) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t sd_nand_write_blocks(uint32_t address, const uint8_t* buffer, size_t count)
{
    HAL_StatusTypeDef status = HAL_OK; 
#if SD_NAND_ASYNC
    status = HAL_SD_WriteBlocks_DMA(&hsd, (uint8_t*)buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE);
    if (status == HAL_OK) {
        tx_semaphore_get(p_write, TX_WAIT_FOREVER);
    }
#else
    status = HAL_SD_WriteBlocks(&hsd, (uint8_t*)buffer, address/SD_NAND_BLOCK_SIZE, count/SD_NAND_BLOCK_SIZE, NAND_TIMEOUT);
#endif

    if(status != HAL_OK) {
        sd_nand_debug("SD NAND write blocks: addr=0x%x, size=%d\n", address, count);
        sd_nand_debug("SD NAND write blocks failed: status=%d, error=0x%x\n", status, HAL_SD_GetError(&hsd));
        status = HAL_SD_Abort(&hsd);
        sd_nand_debug("SD NAND: Abort state=0x%x, status=0x%x\n, error=0x%x\n", HAL_SD_GetCardState(&hsd), status, HAL_SD_GetError(&hsd));
        return AMK_ERROR;
    }

    if (check_card_state(&hsd)) {
        return AMK_SUCCESS;
    }

    return AMK_ERROR;
}

amk_error_t sd_nand_erase_chip(void)
{
    if (sd_nand_get_block_count()== 0) {
        refresh_sd_info();
    }

    HAL_StatusTypeDef status = HAL_SD_Erase(&hsd, 0, sd_nand_get_block_count()-1);
    if (status != HAL_OK) {
        sd_nand_debug("SD NAND erase chip failed: status=%d, error=0x%x\n", status, HAL_SD_GetError(&hsd));
        return AMK_ERROR;
    }
    return AMK_SUCCESS;
}

void sd_nand_uninit(void)
{
    HAL_SD_DeInit(&hsd);
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
    __HAL_RCC_SDIO_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(SDIO_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream6;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(hsd,hdmarx,hdma_sdio_rx);

    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream3;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(hsd,hdmatx,hdma_sdio_tx);

    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(hsd->hdmarx);
    HAL_DMA_DeInit(hsd->hdmatx);

    HAL_NVIC_DisableIRQ(SDIO_IRQn);
}

void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdio_tx);
}

void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdio_rx);
}

void SDIO_IRQHandler(void)
{
    HAL_SD_IRQHandler(&hsd);
}
