/**
 * @file qspi_stm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>
#include "generic_hal.h"
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
extern DMA_HandleTypeDef hdma_quadspi;
extern void Error_Handler(void);

#define FLASH_ADDR_SIZE                 (23)

/* FLASH parameters definition */
#define FLASH_PAGE_SIZE                 (0x100u)
#define FLASH_SECTOR_SIZE               (0x1000u)
#define FLASH_MAX_ADDR                  (0x1000000u)
#define FLASH_DUMMY_BYTE_VALUE          (0xffu)
#define FLASH_BUSY_BIT_MASK             (0x01u)

/* FLASH instruction definition */
#define FLASH_INSTR_WRITE_ENABLE        (0x06u)
#define FLASH_INSTR_PAGE_PROGRAM        (0x02u)
#define FLASH_INSTR_PAGE_PROGRAM_QUAD   (0x32U)
#define FLASH_INSTR_ERASE_4KB_SECTOR    (0x20u)
#define FLASH_INSTR_ERASE_CHIP          (0xC7u)
#define FLASH_INSTR_READ_SR1            (0x05u)
#define FLASH_INSTR_READ_SR2            (0x35u)
#define FLASH_INSTR_READ_SR3            (0x15u)
#define FLASH_INSTR_QUAD_FAST_READ      (0xEB)


#define FLASH_INSTR_RESET_ENABLE        (0x66U)
#define FLASH_INSTR_RESET               (0x99U)

#define FLASH_DUMMY_CYCLES_READ_QUAD    (10)
#define FLASH_BULK_ERASE_MAX_TIME       (250000)
#define FLASH_SECTOR_ERASE_MAX_TIME     (3000)
#define FLASH_SUBSECTOR_ERASE_MAX_TIME  (800)

static amk_error_t QSPI_EraseSector(QSPI_HandleTypeDef *hqspi, uint32_t address);
static amk_error_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static amk_error_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static amk_error_t QSPI_BusyWait(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);

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

    /* QSPI memory reset */
    if (QSPI_ResetMemory(&hqspi) != AMK_SUCCESS) {
        return false;
    }
    
    return true;
}

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length)
{
    if (length != FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: read_sector: invalid size:%d\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    QSPI_CommandTypeDef s_command;

    /* Initialize the read command */
    s_command.InstructionMode       = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction           = FLASH_INSTR_QUAD_FAST_READ;
    s_command.AddressMode           = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize           = QSPI_ADDRESS_24_BITS;
    s_command.Address               = address;
    s_command.AlternateByteMode     = QSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytes        = QSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize    = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode              = QSPI_DATA_4_LINES;
    s_command.DummyCycles           = FLASH_DUMMY_CYCLES_READ_QUAD;
    s_command.NbData                = length;
    s_command.DdrMode               = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle      = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode              = QSPI_SIOO_INST_EVERY_CMD;
    
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return AMK_QSPI_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return AMK_QSPI_ERROR;
    }

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
        if (QSPI_EraseSector(&hqspi, address) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
    }

    // then program
    uint32_t addr = address;
    const uint8_t *cur = buffer;
    for (int i = 0; i < length/FLASH_PAGE_SIZE; i++) {
        QSPI_CommandTypeDef s_command;

        /* Initialize the program command */
        s_command.InstructionMode       = QSPI_INSTRUCTION_1_LINE;
        s_command.Instruction           = FLASH_INSTR_PAGE_PROGRAM_QUAD;
        s_command.AddressMode           = QSPI_ADDRESS_1_LINE;//QSPI_ADDRESS_4_LINES;
        s_command.AddressSize           = QSPI_ADDRESS_24_BITS;
        s_command.AlternateByteMode     = QSPI_ALTERNATE_BYTES_NONE;
        s_command.DataMode              = QSPI_DATA_4_LINES;
        s_command.DummyCycles           = 0;
        s_command.DdrMode               = QSPI_DDR_MODE_DISABLE;
        s_command.DdrHoldHalfCycle      = QSPI_DDR_HHC_ANALOG_DELAY;
        s_command.SIOOMode              = QSPI_SIOO_INST_EVERY_CMD;
        s_command.AlternateByteMode     = QSPI_ALTERNATE_BYTES_NONE;
        s_command.AlternateBytes        = QSPI_ALTERNATE_BYTES_NONE;
        s_command.AlternateBytesSize    = QSPI_ALTERNATE_BYTES_NONE;

        s_command.Address = addr;
        s_command.NbData  = FLASH_PAGE_SIZE;
        
        /* Enable write operations */
        if (QSPI_WriteEnable(&hqspi) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
        
        /* Configure the command */
        if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return AMK_QSPI_ERROR;
        }
        
        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&hqspi, (uint8_t*)cur, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return AMK_QSPI_ERROR;
        }
        
        /* Configure automatic polling mode to wait for end of program */  
        if (QSPI_BusyWait(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
        addr += FLASH_PAGE_SIZE;
        cur += FLASH_PAGE_SIZE;
    }
    
    return AMK_SUCCESS;
}

amk_error_t qspi_erase_chip(void)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_ERASE_CHIP;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (QSPI_WriteEnable(&hqspi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (QSPI_BusyWait(&hqspi, FLASH_BULK_ERASE_MAX_TIME) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

void qspi_uninit(void)
{
    /* Call the DeInit function to reset the driver */
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
    }
}

amk_error_t QSPI_EraseSector(QSPI_HandleTypeDef *hqspi, uint32_t address)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_ERASE_4KB_SECTOR;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = address;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (QSPI_WriteEnable(hqspi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (QSPI_BusyWait(hqspi, FLASH_SECTOR_ERASE_MAX_TIME) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
    return AMK_SUCCESS;

    QSPI_CommandTypeDef s_command;

    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_RESET_ENABLE;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    /* Send the reset memory command */
    s_command.Instruction = FLASH_INSTR_RESET;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (QSPI_BusyWait(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef     s_command;

    /* Enable write operations */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_WRITE_ENABLE;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return AMK_QSPI_ERROR;
    }
    
    return QSPI_BusyWait(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

amk_error_t QSPI_BusyWait(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef     s_command;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FLASH_INSTR_READ_SR1;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    QSPI_AutoPollingTypeDef s_config;
    s_config.Match           = FLASH_BUSY_BIT_MASK;
    s_config.Mask            = FLASH_BUSY_BIT_MASK;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}
