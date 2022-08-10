/**
 * @file hc32f460.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generic_hal.h"
#include "usb_common.h"
#include "amk_printf.h"

#ifdef USE_SPI1

#if 0
#define SPI_UNIT                        (M4_SPI1)
#define SPI_UNIT_CLOCK                  (PWC_FCG1_PERIPH_SPI1)

#define SPI_NSS_PORT                    (PortA)
#define SPI_NSS_PIN                     (Pin07)
#define SPI_NSS_HIGH()                  (PORT_SetBits(SPI_NSS_PORT, SPI_NSS_PIN))
#define SPI_NSS_LOW()                   (PORT_ResetBits(SPI_NSS_PORT, SPI_NSS_PIN))

#define SPI_SCK_PORT                    (PortA)
#define SPI_SCK_PIN                     (Pin06)
#define SPI_SCK_FUNC                    (Func_Spi1_Sck)

#define SPI_MOSI_PORT                   (PortA)
#define SPI_MOSI_PIN                    (Pin05)
#define SPI_MOSI_FUNC                   (Func_Spi1_Mosi)
#else
#define SPI_UNIT                        (M4_SPI3)
#define SPI_UNIT_CLOCK                  (PWC_FCG1_PERIPH_SPI3)

#define SPI_NSS_PORT                    (PortB)
#define SPI_NSS_PIN                     (Pin05)
#define SPI_NSS_HIGH()                  (PORT_SetBits(SPI_NSS_PORT, SPI_NSS_PIN))
#define SPI_NSS_LOW()                   (PORT_ResetBits(SPI_NSS_PORT, SPI_NSS_PIN))

#define SPI_SCK_PORT                    (PortB)
#define SPI_SCK_PIN                     (Pin04)
#define SPI_SCK_FUNC                    (Func_Spi3_Sck)

#define SPI_MOSI_PORT                   (PortB)
#define SPI_MOSI_PIN                    (Pin03)
#define SPI_MOSI_FUNC                   (Func_Spi3_Mosi)

#define SPI_DMA_UNIT                    (M4_DMA1)
#define SPI_DMA_CLOCK_UNIT              (PWC_FCG0_PERIPH_DMA1)
#define SPI_DMA_TX_CHANNEL              (DmaCh1)
//#define SPI_DMA_RX_CHANNEL              (DmaCh0)
#define SPI_DMA_TX_TRIG_SOURCE          (EVT_SPI3_SPTI)
//#define SPI_DMA_RX_TRIG_SOURCE          (EVT_SPI3_SPRI)
#endif

void screen_spi_dma_init(void)
{
    stc_dma_config_t stcDmaCfg;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcDmaCfg);

    /* Configuration peripheral clock */
    PWC_Fcg0PeriphClockCmd(SPI_DMA_CLOCK_UNIT, Enable);
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);

    /* Configure TX DMA */
    stcDmaCfg.u16BlockSize = 1u;
    stcDmaCfg.u16TransferCnt = 0;//u16BufferLen;
    stcDmaCfg.u32SrcAddr = 0;//(uint32_t)(&u8TxBuffer[0]);
    stcDmaCfg.u32DesAddr = (uint32_t)(&SPI_UNIT->DR);
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressIncrease;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;
    stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    DMA_InitChannel(SPI_DMA_UNIT, SPI_DMA_TX_CHANNEL, &stcDmaCfg);

    /* Configure RX DMA */
    //stcDmaCfg.u16BlockSize = 1u;
    //stcDmaCfg.u16TransferCnt = u16BufferLen;
    //stcDmaCfg.u32SrcAddr = (uint32_t)(&SPI_UNIT->DR);
    //stcDmaCfg.u32DesAddr = (uint32_t)(&u8RxBuffer[0]);
    //stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;
    //stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    //stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;
    //stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    //DMA_InitChannel(SPI_DMA_UNIT, SPI_DMA_RX_CHANNEL, &stcDmaCfg);

    DMA_SetTriggerSrc(SPI_DMA_UNIT, SPI_DMA_TX_CHANNEL, SPI_DMA_TX_TRIG_SOURCE);
    //DMA_SetTriggerSrc(SPI_DMA_UNIT, SPI_DMA_RX_CHANNEL, SPI_DMA_RX_TRIG_SOURCE);

    /* Enable DMA. */
    DMA_Cmd(SPI_DMA_UNIT, Enable);
}

M4_SPI_TypeDef* hspi1 = SPI_UNIT;

void screen_spi_init(void)
{
    stc_spi_init_t stcSpiInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcSpiInit);

    /* Configuration peripheral clock */
    PWC_Fcg1PeriphClockCmd(SPI_UNIT_CLOCK, Enable);

    /* Configuration SPI pin */
    PORT_SetFunc(SPI_SCK_PORT, SPI_SCK_PIN, SPI_SCK_FUNC, Disable);
    PORT_SetFunc(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_FUNC, Disable);

    /* Configuration SPI structure */
    stcSpiInit.enClkDiv = SpiClkDiv2;  //pclk1 = 50, 50/8 = 6.25
    stcSpiInit.enFrameNumber = SpiFrameNumber1;
    stcSpiInit.enDataLength = SpiDataLengthBit8;
    stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
    stcSpiInit.enSckPolarity = SpiSckIdleLevelHigh;//SpiSckIdleLevelLow;
    stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;//SpiSckOddSampleEvenChange;
    stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
    stcSpiInit.enWorkMode = SpiWorkMode3Line;
    stcSpiInit.enTransMode = SpiTransOnlySend;
    stcSpiInit.enCommAutoSuspendEn = Disable;
    stcSpiInit.enModeFaultErrorDetectEn = Disable;
    stcSpiInit.enParitySelfDetectEn = Disable;
    stcSpiInit.enParityEn = Disable;
    stcSpiInit.enParity = SpiParityEven;
    stcSpiInit.enMasterSlaveMode = SpiModeMaster;
    stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
    stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
    stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
    stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
    stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
    stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;

    SPI_Init(SPI_UNIT, &stcSpiInit);
    //SPI_Cmd(SPI_UNIT, Enable);
    screen_spi_dma_init();
}
#endif

// put on retention memory
uint32_t BKP_DR0 __attribute__((section(".ret_ram_data")));     // for reboot to bootloader 
uint32_t BKP_DR1 __attribute__((section(".ret_ram_data")));     // for special reboot 

void custom_board_init(void)
{
#ifdef USE_SPI1
    screen_spi_init();
#endif
#ifdef DYNAMIC_CONFIGURATION
    uint32_t magic = BKP_DR1;
    //if (magic == 0) {
    if (magic > 0) {
        usb_setting |= USB_MSC_BIT;
    } else {
        usb_setting = 0;
    }
    amk_printf("usb_setting: %d\n", usb_setting);
    BKP_DR1 = 0;
#endif
}