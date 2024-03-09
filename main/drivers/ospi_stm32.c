/**
 * @file ospi_stm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdbool.h>
#include "amk_hal.h"
#include "amk_error.h"
#include "amk_printf.h"

#ifndef OSPI_DEBUG
#define OSPI_DEBUG 1
#endif

#if OSPI_DEBUG
#define ospi_debug  amk_printf
#else
#define ospi_debug(...)
#endif

#if !defined(STM32U575xx)
#error "Current MCU Do NOt Support OSPI"
#endif

extern OSPI_HandleTypeDef hospi1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel2;
extern DMA_HandleTypeDef handle_GPDMA1_Channel1;

extern void Error_Handler(void);

#define OSPI_FLASH_ADDR_SIZE            (24)

/* FLASH parameters definition */
#define OSPI_FLASH_PAGE_SIZE            (0x100u)
#define OSPI_FLASH_SECTOR_SIZE          (0x1000u)
#define OSPI_FLASH_MAX_ADDR             (0x1000000u)
#define OSPI_FLASH_DUMMY_BYTE_VALUE     (0xffu)
#define OSPI_FLASH_BUSY_BIT_MASK        (0x01u)

/* FLASH instruction definition */
#define FLASH_INSTR_WRITE_ENABLE        (0x06u)
#define FLASH_INSTR_PAGE_PROGRAM        (0x02u)
#define FLASH_INSTR_PAGE_PROGRAM_QUAD   (0x32U)
#define FLASH_INSTR_ERASE_4KB_SECTOR    (0x20u)
#define FLASH_INSTR_ERASE_CHIP          (0xC7u)
#define FLASH_INSTR_READ_SR1            (0x05u)
#define FLASH_INSTR_READ_SR2            (0x35u)
#define FLASH_INSTR_READ_SR3            (0x15u)
#define FLASH_INSTR_WRITE_SR1           (0x01u)
#define FLASH_INSTR_WRITE_SR2           (0x31u)
#define FLASH_INSTR_WRITE_SR3           (0x11u)
#define FLASH_INSTR_READ_CMD            (0x03)
#define FLASH_INSTR_QUAD_READ_CMD       (0x6B)
#define FLASH_INSTR_QUAD_READ_IO_CMD    (0xEB)
#define FLASH_INSTR_RESET_ENABLE        (0x66U)
#define FLASH_INSTR_RESET               (0x99U)
#define FLASH_INSTR_JEDECID             (0x9FU)

/* TIMEOUT*/
#define DUMMY_CYCLES_READ_QUAD          (8)
#define DUMMY_CYCLES_READ_QUAD_IO       (4)
#define FLASH_BULK_ERASE_MAX_TIME       (250000)
#define FLASH_SECTOR_ERASE_MAX_TIME     (3000)
#define FLASH_SUBSECTOR_ERASE_MAX_TIME  (800)

/* STATUS */
#define W25Q128FV_FSR_BUSY              ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN              ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                ((uint8_t)0x02)    /*!< quad enable */

static amk_error_t OSPI_InitW25Qxx(OSPI_HandleTypeDef *hospi);
static amk_error_t OSPI_EraseSector(OSPI_HandleTypeDef *hospi, uint32_t address);
static amk_error_t OSPI_ResetMemory(OSPI_HandleTypeDef *hospi);
static amk_error_t OSPI_WriteEnable(OSPI_HandleTypeDef *hospi);
static amk_error_t OSPI_BusyWait(OSPI_HandleTypeDef *hospi, uint32_t Timeout);

bool qspi_init(uint32_t map_addr)
{
    hospi1.Instance = OCTOSPI1;
    HAL_OSPI_DeInit(&hospi1);

    hospi1.Init.FifoThreshold = 1;
    hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
    hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
    hospi1.Init.DeviceSize = OSPI_FLASH_ADDR_SIZE;
    hospi1.Init.ChipSelectHighTime = 1;
    hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
    hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
    hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
    hospi1.Init.ClockPrescaler = 2;
    hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
    hospi1.Init.DelayHoldQuarterCycle = //HAL_OSPI_DHQC_DISABLE;
                                    HAL_OSPI_DHQC_ENABLE;
    hospi1.Init.ChipSelectBoundary = 0;
    hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
    hospi1.Init.MaxTran = 0;
    hospi1.Init.Refresh = 0;
    if (HAL_OSPI_Init(&hospi1) != HAL_OK) {
        return false;
    }

    OSPIM_CfgTypeDef sOspiManagerCfg = {0};
    sOspiManagerCfg.ClkPort = 1;
    sOspiManagerCfg.NCSPort = 1;
    sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
    if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return false;
    }

    HAL_OSPI_DLYB_CfgTypeDef HAL_OSPI_DLYB_Cfg_Struct = {0};
    HAL_OSPI_DLYB_Cfg_Struct.Units = 0;
    HAL_OSPI_DLYB_Cfg_Struct.PhaseSel = 0;
    if (HAL_OSPI_DLYB_SetConfig(&hospi1, &HAL_OSPI_DLYB_Cfg_Struct) != HAL_OK) {
        return false;
    }
    
    if (OSPI_InitW25Qxx(&hospi1) != AMK_SUCCESS) {
        ospi_debug("QSPI: failed to init w25qxx\n");
        return false;
    }
    return true;
}

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length)
{
    if (length != OSPI_FLASH_SECTOR_SIZE) {
        ospi_debug("QSPI: read_sector: invalid size:%d\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    OSPI_RegularCmdTypeDef s_command = {0};

    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_QUAD_READ_CMD;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
    s_command.DataMode          = HAL_OSPI_DATA_4_LINES;
    s_command.DummyCycles       = DUMMY_CYCLES_READ_QUAD;

    //s_command.Instruction       = FLASH_INSTR_QUAD_READ_IO_CMD;
    //s_command.AddressMode       = HAL_OSPI_ADDRESS_4_LINES;
    //s_command.DataMode          = HAL_OSPI_DATA_4_LINES;
    //s_command.DummyCycles       = DUMMY_CYCLES_READ_QUAD_IO;

    //s_command.Instruction       = FLASH_INSTR_READ_CMD;
    //s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
    //s_command.DataMode          = HAL_OSPI_DATA_1_LINE;
    //s_command.DummyCycles       = 0;

    s_command.Address           = address;
    s_command.NbData            = length;
    
    HAL_StatusTypeDef status = HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
    if (status != HAL_OK) {
        ospi_debug("QSPI: read_sector: failed to send read cmd, error=%d\n", status);
        return AMK_QSPI_ERROR;
    } else {
        //ospi_debug("QSPI: read_sector: send read cmd SUCCESS\n");
    }

    status = HAL_OSPI_Receive(&hospi1, buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
    if (status != HAL_OK) {
        ospi_debug("QSPI: read_sector: failed to receive data, address=%d, size=%d, error=%d\n", address, length, status);
        return AMK_QSPI_ERROR;
    } else {
        //ospi_debug("QSPI: read_sector: receive data SUCCESS, address=%d, size=%d\n", address, length);
    }

    return AMK_SUCCESS;
}

amk_error_t qspi_write_sector(uint32_t address, const uint8_t* buffer, size_t length)
{
    //ospi_debug("QSPI WRITE: addr=%d, size=%d\n", address, length);
    if (length != OSPI_FLASH_SECTOR_SIZE) {
        ospi_debug("QSPI: write_sector: invalid size:%u\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    // erase sector first
    if (address >= OSPI_FLASH_MAX_ADDR) {
        ospi_debug("QSPI: write_sector: invalid address:%u\n", address);
        return AMK_QSPI_INVALID_PARAM;
    } else {
        if ( OSPI_EraseSector(&hospi1, address) != AMK_SUCCESS) {
            ospi_debug("QSPI: write_sector: faled to erase sector:%u\n", address);
            return AMK_QSPI_ERROR;
        }
    }

    // then program
    uint32_t addr = address;
    const uint8_t *cur = buffer;
    for (int i = 0; i < length/OSPI_FLASH_PAGE_SIZE; i++) {
        OSPI_RegularCmdTypeDef s_command = {0};
        s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
        s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
        s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
        s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
        s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
        s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
        s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
        s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
        s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
        s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
        s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

        s_command.Instruction       = FLASH_INSTR_PAGE_PROGRAM_QUAD;
        s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
        s_command.DataMode          = HAL_OSPI_DATA_4_LINES;
        s_command.DummyCycles       = 0;

        //s_command.Instruction       = FLASH_INSTR_PAGE_PROGRAM;
        //s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
        //s_command.DataMode          = HAL_OSPI_DATA_1_LINE;
        //s_command.DummyCycles       = 0;

        s_command.Address = addr;
        s_command.NbData  = OSPI_FLASH_PAGE_SIZE;
        
        /* Enable write operations */
        if (OSPI_WriteEnable(&hospi1) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
        
        /* Configure the command */
        if (HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return AMK_QSPI_ERROR;
        }
        
        /* Transmission of the data */
        if (HAL_OSPI_Transmit(&hospi1, (uint8_t*)cur, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return AMK_QSPI_ERROR;
        }
        
        /* Configure automatic polling mode to wait for end of program */  
        if (OSPI_BusyWait(&hospi1, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != AMK_SUCCESS) {
            return AMK_QSPI_ERROR;
        }
        addr += OSPI_FLASH_PAGE_SIZE;
        cur += OSPI_FLASH_PAGE_SIZE;
    }
    
    ospi_debug("QSPI: write_sector:%u success\n", address);
    return AMK_SUCCESS;
}

amk_error_t qspi_erase_chip(void)
{
     OSPI_RegularCmdTypeDef s_command = {0};
    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_ERASE_CHIP;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_NONE;
    s_command.DataMode          = HAL_OSPI_DATA_NONE;
    s_command.DummyCycles       = 0;

    /* Enable write operations */
    if (OSPI_WriteEnable(&hospi1) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (OSPI_BusyWait(&hospi1, FLASH_BULK_ERASE_MAX_TIME) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

void qspi_uninit(void)
{
    /* Call the DeInit function to reset the driver */
    if (HAL_OSPI_DeInit(&hospi1) != HAL_OK) {
    }
}

amk_error_t OSPI_EraseSector(OSPI_HandleTypeDef *hospi, uint32_t address)
{
     OSPI_RegularCmdTypeDef s_command = {0};
    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_ERASE_4KB_SECTOR;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
    s_command.Address           = address;
    s_command.DataMode          = HAL_OSPI_DATA_NONE;
    s_command.DummyCycles       = 0;

    /* Enable write operations */
    if (OSPI_WriteEnable(hospi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    /* Send the command */
    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of erase */  
    if (OSPI_BusyWait(hospi, FLASH_SECTOR_ERASE_MAX_TIME) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t OSPI_ResetMemory(OSPI_HandleTypeDef *hospi)
{
    OSPI_RegularCmdTypeDef s_command = {0};
    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_RESET_ENABLE;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_NONE;
    s_command.DataMode          = HAL_OSPI_DATA_NONE;
    s_command.DummyCycles       = 0;

    /* Send the command */
    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        ospi_debug("QSPI: failed to send RESET ENABLE command\n");
        return AMK_QSPI_ERROR;
    }

    /* Send the reset memory command */
    s_command.Instruction = FLASH_INSTR_RESET;
    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        ospi_debug("QSPI: failed to send RESET command\n");
        return AMK_QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (OSPI_BusyWait(hospi, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t OSPI_WriteEnable(OSPI_HandleTypeDef *hospi)
{
    OSPI_RegularCmdTypeDef s_command = {0};
    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_WRITE_ENABLE;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_NONE;
    s_command.DataMode          = HAL_OSPI_DATA_NONE;
    s_command.DummyCycles       = 0;

    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        ospi_debug("QSPI: state=%d\n", hospi->State);
        ospi_debug("QSPI: failed to send WRITE ENABLE command\n");
        return AMK_QSPI_ERROR;
    }

    s_command.Instruction       = FLASH_INSTR_READ_SR1;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
    s_command.Address           = 0;
    s_command.DataMode          = HAL_OSPI_DATA_1_LINE;
    s_command.NbData            = 1;
    s_command.DummyCycles       = 0;
    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        ospi_debug("QSPI: failed to send READ SR1 command\n");
        return AMK_QSPI_ERROR;
    }

    OSPI_AutoPollingTypeDef s_config;
    s_config.Match          = W25Q128FV_FSR_WREN;
    s_config.Mask           = W25Q128FV_FSR_WREN;
    s_config.MatchMode      = HAL_OSPI_MATCH_MODE_AND;
    s_config.Interval       = 0x10;
    s_config.AutomaticStop  = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_OSPI_AutoPolling(hospi, &s_config, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        ospi_debug("QSPI: failed to AutoPolling\n");
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t OSPI_BusyWait(OSPI_HandleTypeDef *hospi, uint32_t Timeout)
{
    OSPI_RegularCmdTypeDef s_command = {0};
    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_READ_SR1;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_1_LINE;
    s_command.Address           = 0;
    s_command.DataMode          = HAL_OSPI_DATA_1_LINE;
    s_command.NbData            = 1;
    s_command.DummyCycles       = 0;
    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        ospi_debug("QSPI: failed to send READ SR1 command\n");
        return AMK_QSPI_ERROR;
    }

    OSPI_AutoPollingTypeDef s_config;
    s_config.Match          = 0;
    s_config.Mask           = OSPI_FLASH_BUSY_BIT_MASK;
    s_config.MatchMode      = HAL_OSPI_MATCH_MODE_AND;
    s_config.Interval       = 0x10;
    s_config.AutomaticStop  = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_OSPI_AutoPolling(hospi, &s_config, Timeout) != HAL_OK) {
        ospi_debug("QSPI: failed to AutoPolling\n");
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}

static amk_error_t OSPI_InitW25Qxx(OSPI_HandleTypeDef *hospi)
{
    if (OSPI_ResetMemory(hospi) != AMK_SUCCESS) {
        ospi_debug("QSPI: failed to RESET memory\n");
        return AMK_QSPI_ERROR;
    }

    if (OSPI_WriteEnable(hospi) != AMK_SUCCESS) {
        return AMK_QSPI_ERROR;
    }

    OSPI_RegularCmdTypeDef s_command = {0};
    s_command.OperationType     = HAL_OSPI_OPTYPE_COMMON_CFG;
    s_command.FlashId           = HAL_OSPI_FLASH_ID_1;
    s_command.InstructionMode   = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize   = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.InstructionDtrMode= HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    s_command.AddressSize       = HAL_OSPI_ADDRESS_24_BITS;
    s_command.AddressDtrMode    = HAL_OSPI_ADDRESS_DTR_DISABLE;
    s_command.AlternateBytesMode= HAL_OSPI_ALTERNATE_BYTES_NONE;
    s_command.DataDtrMode       = HAL_OSPI_DATA_DTR_DISABLE;
    s_command.DQSMode           = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode          = HAL_OSPI_SIOO_INST_EVERY_CMD;

    s_command.Instruction       = FLASH_INSTR_WRITE_SR2;
    s_command.AddressMode       = HAL_OSPI_ADDRESS_NONE;
    s_command.DataMode          = HAL_OSPI_DATA_1_LINE;
    s_command.NbData            = 1;
    s_command.DummyCycles       = 0;

    if (HAL_OSPI_Command(hospi, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        return AMK_QSPI_ERROR;
    }

    uint8_t value = W25Q128FV_FSR_QE;
    if (HAL_OSPI_Transmit(hospi, &value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK){
        return AMK_QSPI_ERROR;
    }

    if (OSPI_BusyWait(hospi, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return AMK_QSPI_ERROR;
    }

    return AMK_SUCCESS;
}
