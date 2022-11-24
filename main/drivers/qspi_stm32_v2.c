/**
 * @file qspi_stm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>
#include "w25q_mem.h"
#include "amk_error.h"
#include "amk_printf.h"

#ifndef QSPI_DEBUG
#define QSPI_DEBUG 0
#endif

#if QSPI_DEBUG
#define qspi_debug  amk_printf
#else
#define qspi_debug(...)
#endif

extern QSPI_HandleTypeDef hqspi;

#define FLASH_ADDR_SIZE                 (23)

/* FLASH parameters definition */
#define FLASH_PAGE_SIZE                 (0x100u)
#define FLASH_SECTOR_SIZE               (0x1000u)
#define FLASH_MAX_ADDR                  (0x1000000u)

bool qspi_init(uint32_t map_addr)
{
    hqspi.Instance = QUADSPI;

    /* Call the DeInit function to reset the driver */
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
        return false;
    }
            
    /* QSPI initialization */
    hqspi.Init.ClockPrescaler     = 0; /* QSPI freq = 100 MHz/(0+1) = 100 Mhz */
    hqspi.Init.FifoThreshold      = 4;
    hqspi.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    hqspi.Init.FlashSize          = FLASH_ADDR_SIZE;
    hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE; /* Min 50ns for nonRead command */
    hqspi.Init.ClockMode          = QSPI_CLOCK_MODE_0;
    hqspi.Init.FlashID            = QSPI_FLASH_ID_2;
    hqspi.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

    if (HAL_QSPI_Init(&hqspi) != HAL_OK) {
        return false;
    }

    W25Q_Init();

    return true;
}

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length)
{
    if (length != FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: read_sector: invalid size:%d\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    W25Q_ReadRaw(buffer, length, address);

    return AMK_SUCCESS;
}

amk_error_t qspi_write_sector(uint32_t address, const uint8_t* buffer, size_t length)
{
    //qspi_debug("QSPI WRITE: addr=%d, size=%d\n", address, length);
    if (length != FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: write_sector: invalid size:%u\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    // erase sector first
    if (address >= FLASH_MAX_ADDR) {
        qspi_debug("QSPI: write_sector: invalid address:%u\n", address);
        return AMK_QSPI_INVALID_PARAM;
    } else {
        W25Q_EraseSector(address/4096);
    }

    // then program
    uint32_t addr = address;
    uint8_t *cur = (uint8_t*)buffer;
    for (int i = 0; i < length/FLASH_PAGE_SIZE; i++) {
        W25Q_ProgramRaw(cur, FLASH_PAGE_SIZE, addr);
        addr += FLASH_PAGE_SIZE;
        cur += FLASH_PAGE_SIZE;
    }
    
    return AMK_SUCCESS;
}

amk_error_t qspi_erase_chip(void)
{
    return AMK_SUCCESS;
}

void qspi_uninit(void)
{
    /* Call the DeInit function to reset the driver */
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
    }
}
