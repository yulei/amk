/**
 * @file qspi_hc32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "qspi.h"
#include "amk_hal.h"
#include "amk_printf.h"

#ifndef QSPI_DEBUG
#define QSPI_DEBUG 0
#endif

#if QSPI_DEBUG
#define qspi_debug  amk_printf
#else
#define qspi_debug(...)
#endif

/* QSPCK Port/Pin definition */
#define QSPCK_PORT                      (PortB)
#define QSPCK_PIN                       (Pin14)

/* QSNSS Port/Pin definition */
#define QSNSS_PORT                      (PortB)
#define QSNSS_PIN                       (Pin01)

/* QSIO0 Port/Pin definition */
#define QSIO0_PORT                      (PortB)
#define QSIO0_PIN                       (Pin13)

/* QSIO1 Port/Pin definition */
#define QSIO1_PORT                      (PortB)
#define QSIO1_PIN                       (Pin12)

/* QSIO2 Port/Pin definition */
#define QSIO2_PORT                      (PortB)
#define QSIO2_PIN                       (Pin10)

/* QSIO3 Port/Pin definition */
#define QSIO3_PORT                      (PortB)
#define QSIO3_PIN                       (Pin02)

/* QSPI memory bus address definition */
#define QSPI_BUS_ADDRESS                (0x98000000u)

/* FLASH parameters definition */
#define FLASH_PAGE_SIZE                 (0x100u)
#define FLASH_SECTOR_SIZE               (0x1000u)
#define FLASH_MAX_ADDR                  (0x1000000u)
#define FLASH_DUMMY_BYTE_VALUE          (0xffu)
#define FLASH_BUSY_BIT_MASK             (0x01u)

/* FLASH instruction definition */
#define FLASH_INSTR_WRITE_ENABLE        (0x06u)
#define FLASH_INSTR_PAGE_PROGRAM        (0x02u)
#define FLASH_INSTR_ERASE_4KB_SECTOR    (0x20u)
#define FLASH_INSTR_ERASE_CHIP          (0xC7u)
#define FLASH_INSTR_READ_SR1            (0x05u)
#define FLASH_INSTR_READ_SR2            (0x35u)
#define FLASH_INSTR_READ_SR3            (0x15u)


/**
 *******************************************************************************
 ** \brief QSPI flash write enable function
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void QspiFlash_WriteEnable(void)
{
    QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(FLASH_INSTR_WRITE_ENABLE);
    QSPI_ExitDirectCommMode();
}

/**
 *******************************************************************************
 ** \brief QSPI flash wait for write operation end function
 **
 ** \param [in] None
 **
 ** \retval Ok                              Flash internal operation finish
 ** \retval ErrorTimeout                    Flash internal operation timeout
 **
 ******************************************************************************/
static en_result_t QspiFlash_WaitForWriteEnd(void)
{
    en_result_t enRet = Ok;
    uint8_t u8Status = 0u;
    uint32_t u32Timeout;
    stc_clk_freq_t stcClkFreq;

    CLK_GetClockFreq(&stcClkFreq);
    u32Timeout = stcClkFreq.sysclkFreq / 1000u;
    QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(FLASH_INSTR_READ_SR1);
    do
    {
        u8Status = QSPI_ReadDirectCommValue();
        u32Timeout--;
    } while ((u32Timeout != 0u) &&
             ((u8Status & FLASH_BUSY_BIT_MASK) == FLASH_BUSY_BIT_MASK));

    if (FLASH_BUSY_BIT_MASK == u8Status)
    {
        enRet = ErrorTimeout;
    }
    QSPI_ExitDirectCommMode();

    return enRet;
}

bool qspi_init(uint32_t map_addr)
{
    (void)map_addr;

    stc_qspi_init_t stcQspiInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcQspiInit);

    /* Configuration peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_QSPI, Enable);

    /* Configuration QSPI pin */
    PORT_SetFunc(QSPCK_PORT, QSPCK_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSNSS_PORT, QSNSS_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO0_PORT, QSIO0_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO1_PORT, QSIO1_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO2_PORT, QSIO2_PIN, Func_Qspi, Disable);
    PORT_SetFunc(QSIO3_PORT, QSIO3_PIN, Func_Qspi, Disable);

#if 0
    stcQspiInit.enClkDiv = QspiHclkDiv2;
    stcQspiInit.enSpiMode = QspiSpiMode3;
    stcQspiInit.enBusCommMode = QspiBusModeRomAccess;
    stcQspiInit.enPrefetchMode = QspiPrefetchStopComplete;
    stcQspiInit.enPrefetchFuncEn = Disable;
    stcQspiInit.enQssnValidExtendTime = QspiQssnValidExtendSck32;
    stcQspiInit.enQssnIntervalTime = QspiQssnIntervalQsck8;
    stcQspiInit.enQsckDutyCorr = QspiQsckDutyCorrHalfHclk;
    stcQspiInit.enVirtualPeriod = QspiVirtualPeriodQsck6;
    stcQspiInit.enWpPinLevel = QspiWpPinOutputHigh;
    stcQspiInit.enQssnSetupDelayTime = QspiQssnSetupDelay1Dot5Qsck;
    stcQspiInit.enQssnHoldDelayTime = QspiQssnHoldDelay1Dot5Qsck;
    stcQspiInit.enFourByteAddrReadEn = Disable;
    stcQspiInit.enAddrWidth = QspiAddressByteThree;
    stcQspiInit.stcCommProtocol.enReadMode = QspiReadModeFourWiresIO;
    stcQspiInit.u8RomAccessInstr = QSPI_3BINSTR_FOUR_WIRES_IO_READ;
#else
    /* Configuration QSPI structure */
    stcQspiInit.enClkDiv = QspiHclkDiv2;
    stcQspiInit.enSpiMode = QspiSpiMode0;
    stcQspiInit.enBusCommMode = QspiBusModeRomAccess;
    stcQspiInit.enPrefetchMode = QspiPrefetchStopComplete;
    stcQspiInit.enPrefetchFuncEn = Disable;
    stcQspiInit.enQssnValidExtendTime = QspiQssnValidExtendNot;
    stcQspiInit.enQssnIntervalTime = QspiQssnIntervalQsck8;
    stcQspiInit.enQsckDutyCorr = QspiQsckDutyCorrNot;
    stcQspiInit.enVirtualPeriod = QspiVirtualPeriodQsck8;
    stcQspiInit.enWpPinLevel = QspiWpPinOutputLow;
    stcQspiInit.enQssnSetupDelayTime = QspiQssnSetupDelayHalfQsck;
    stcQspiInit.enQssnHoldDelayTime = QspiQssnHoldDelayHalfQsck;
    stcQspiInit.enFourByteAddrReadEn = Disable;
    stcQspiInit.enAddrWidth = QspiAddressByteThree;

    stcQspiInit.u8RomAccessInstr = QSPI_3BINSTR_FAST_READ;
    stcQspiInit.u8RomAccessInstr = QSPI_3BINSTR_FAST_READ;

    //stcQspiInit.u8RomAccessInstr = QSPI_3BINSTR_FOUR_WIRES_IO_READ;
    //stcQspiInit.stcCommProtocol.enReadMode = QspiReadModeFourWiresIO;

    //stcQspiInit.u8RomAccessInstr = QSPI_3BINSTR_STANDARD_READ;
    //stcQspiInit.stcCommProtocol.enReadMode = QspiReadModeStandard;
#endif

    stcQspiInit.stcCommProtocol.enTransInstrProtocol = QspiProtocolExtendSpi;
    stcQspiInit.stcCommProtocol.enTransAddrProtocol = QspiProtocolExtendSpi;
    stcQspiInit.stcCommProtocol.enReceProtocol = QspiProtocolExtendSpi;
    QSPI_Init(&stcQspiInit);

    return true;
}

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length)
{
    if (length != FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: read_sector: invalid size:%d\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    uint8_t *src = (uint8_t *)(QSPI_BUS_ADDRESS + address);
    qspi_debug("QSPI: read_sector: addr=%p, size=%d\n", src, length);
    memcpy(buffer, src, length);
    //for(int i = 0; i < FLASH_PAGE_SIZE; i++) {
    //    qspi_debug("%x", buffer[i]);
    //}
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
        QspiFlash_WriteEnable();
        /* Send instruction to flash */
        QSPI_EnterDirectCommMode();
        QSPI_WriteDirectCommValue(FLASH_INSTR_ERASE_4KB_SECTOR);
        QSPI_WriteDirectCommValue((uint8_t)((address & 0xFF0000ul) >> 16));
        QSPI_WriteDirectCommValue((uint8_t)((address & 0xFF00u) >> 8));
        QSPI_WriteDirectCommValue((uint8_t)( address & 0xFFu));
        QSPI_ExitDirectCommMode();
        /* Wait for flash idle */
        en_result_t ret = QspiFlash_WaitForWriteEnd();
        if (ret != Ok) {
            qspi_debug("QSPI: write_sector: erase failed:%d\n", ret);
            return AMK_QSPI_ERROR;
        }
    }

    // then program
    uint32_t addr = address;
    const uint8_t *cur = buffer;
    for (int i = 0; i < length/FLASH_PAGE_SIZE; i++) {
        QspiFlash_WriteEnable();
        /* Send data to flash */
        QSPI_EnterDirectCommMode();
        QSPI_WriteDirectCommValue(FLASH_INSTR_PAGE_PROGRAM);
        QSPI_WriteDirectCommValue((uint8_t)((addr & 0xFF0000ul) >> 16));
        QSPI_WriteDirectCommValue((uint8_t)((addr & 0xFF00u) >> 8));
        QSPI_WriteDirectCommValue((uint8_t)( addr & 0xFFu));
        uint32_t len = FLASH_PAGE_SIZE;
        while (len--) {
           QSPI_WriteDirectCommValue(*cur++);
        }
        QSPI_ExitDirectCommMode();

        /* Wait for flash idle */
        en_result_t ret = QspiFlash_WaitForWriteEnd();
        if (ret != Ok) {
            qspi_debug("QSPI: write_sector: program failed:%d\n", ret);
            return AMK_QSPI_ERROR;
        } else {
            qspi_debug("QSPI: write_sector: addr=0x%x, size=%d\n", addr, FLASH_PAGE_SIZE);
            addr += FLASH_PAGE_SIZE;
        }
    }

    return AMK_SUCCESS;
}

amk_error_t qspi_erase_chip(void)
{
    QspiFlash_WriteEnable();
    /* Send instruction to flash */
    QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(FLASH_INSTR_ERASE_CHIP);
    QSPI_ExitDirectCommMode();
    /* Wait for flash idle */
    QspiFlash_WaitForWriteEnd();
    return AMK_SUCCESS;
}

void qspi_uninit(void)
{
    QSPI_DeInit();
}
