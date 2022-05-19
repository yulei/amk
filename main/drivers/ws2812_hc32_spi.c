/**
 * @file ws2812_hc32_spi.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>
#include "rgb_common.h"
#include "ws2812.h"

#ifndef WS2812_SPI_ID
#error "ws2812 spi instance not defined"
#endif

#ifndef WS2812_DMA_ID
#error "ws2812 spi dma instance not defined"
#endif

#if (WS2812_SPI_ID==SPI_INSTANCE_1)
#define WS2812_SPI              (M4_SPI1)
#define WS2812_SPI_CLOCK        (PWC_FCG1_PERIPH_SPI1)
#define WS2812_SPI_MOSI_FUNC    (Func_Spi1_Mosi)
#endif

extern TIM_HandleTypeDef PWM_TIM;

#define LED_DATA_SIZE           (WS2812_LED_NUM * 24)
#define RESET_SLOTS_BEGIN       (100)
#define RESET_SLOTS_END         (100)
#define WS2812_LAST_SLOT        (1)
#define LED_BUFFER_SIZE         (RESET_SLOTS_BEGIN + LED_DATA_SIZE + WS2812_LAST_SLOT + RESET_SLOTS_END)
#define WS2812_1                (0xFFF0)
#define WS2812_0                (0xF000)
#define WS2812_RESET            (0)

static uint16_t ws2812_leds[LED_BUFFER_SIZE];
static bool ws2812_ready = false;
static bool ws2812_dirty = false;
static pin_t ws2812_pin;

static void ws2812_spi_init(void)
{
    ws2812_pin = WS2812_LED_PIN;

    stc_spi_init_t stcSpiInit;

    MEM_ZERO_STRUCT(stcSpiInit);

    PWC_Fcg1PeriphClockCmd(WS2812_SPI_CLOCK, Enable);

    PORT_SetFunc(GET_PORT(ws2812_pin), GET_PIN(ws2812_pin), WS2812_SPI_MOSI_FUNC, Disable);

    stcSpiInit.enClkDiv = SpiClkDiv2;   // pclk1 was 100/4 = 25mHz, spi = pclk1/2 = 12.5mHz
    stcSpiInit.enFrameNumber = SpiFrameNumber1;
    stcSpiInit.enDataLength = SpiDataLengthBit16;
    stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
    stcSpiInit.enSckPolarity = SpiSckIdleLevelLow;
    stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;
    stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
    stcSpiInit.enWorkMode = SpiWorkMode3Line;
    stcSpiInit.enTransMode = SpiTransOnlySend;//SpiTransFullDuplex;
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

    SPI_Init(WS2812_SPI, &stcSpiInit);
}

static void ws2812_dma_init(void)
{
    stc_dma_config_t stcDmaCfg;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcDmaCfg);

    /* Configuration peripheral clock */
    PWC_Fcg0PeriphClockCmd(SPI_DMA_CLOCK_UNIT, Enable);
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);

    /* Configure TX DMA */
    stcDmaCfg.u16BlockSize = 1u;
    stcDmaCfg.u16TransferCnt = u16BufferLen;
    stcDmaCfg.u32SrcAddr = (uint32_t)(&u8TxBuffer[0]);
    stcDmaCfg.u32DesAddr = (uint32_t)(&SPI_UNIT->DR);
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressIncrease;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;
    stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    DMA_InitChannel(SPI_DMA_UNIT, SPI_DMA_TX_CHANNEL, &stcDmaCfg);

    /* Configure RX DMA */
    stcDmaCfg.u16BlockSize = 1u;
    stcDmaCfg.u16TransferCnt = u16BufferLen;
    stcDmaCfg.u32SrcAddr = (uint32_t)(&SPI_UNIT->DR);
    stcDmaCfg.u32DesAddr = (uint32_t)(&u8RxBuffer[0]);
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;
    stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    DMA_InitChannel(SPI_DMA_UNIT, SPI_DMA_RX_CHANNEL, &stcDmaCfg);

    DMA_SetTriggerSrc(SPI_DMA_UNIT, SPI_DMA_TX_CHANNEL, SPI_DMA_TX_TRIG_SOURCE);
    DMA_SetTriggerSrc(SPI_DMA_UNIT, SPI_DMA_RX_CHANNEL, SPI_DMA_RX_TRIG_SOURCE);

    /* Enable DMA. */
    DMA_Cmd(SPI_DMA_UNIT, Enable);
}

void ws2812_init(pin_t pin)
{
    if (ws2812_ready) return;

    ws2812_pin = pin;

    ws2812_ready = true;
    ws2812_dirty = true;

    uint32_t i = 0;
    for (i = 0; i < LED_BUFFER_SIZE; i++) {
        ws2812_leds[i] = 0;
    }

    for (i = 0; i < LED_DATA_SIZE; i++) {
        ws2812_leds[RESET_SLOTS_BEGIN+i] = WS2812_0;
    }

    ws2812_leds[RESET_SLOTS_BEGIN+LED_DATA_SIZE] = WS2812_0;
}

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    index = led->r;

    if (index < WS2812_LED_NUM) {
        uint32_t i = 0;
        // green
        uint32_t cur = RESET_SLOTS_BEGIN+index*24;
        for(i = 0; i < 8; i++) {
            ws2812_leds[cur+i] = ((green<<i) & 0x80) ? WS2812_1 : WS2812_0;
        }
        cur += 8;
        // red
        for(i = 0; i < 8; i++) {
            ws2812_leds[cur+i] = ((red<<i) & 0x80) ? WS2812_1 : WS2812_0;
        }
        cur += 8;
        // blue
        for(i = 0; i < 8; i++) {
            ws2812_leds[cur+i] = ((blue<<i) & 0x80) ? WS2812_1 : WS2812_0;
        }
    }
    ws2812_dirty = true;
}

void ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < WS2812_LED_NUM; i++) {
        ws2812_set_color(i, red, green, blue);
    }
    ws2812_dirty = true;
}

void ws2812_update_buffers(pin_t pin)
{
    if (!ws2812_ready) {
        ws2812_init(pin);
    }

    if (!ws2812_dirty) {
        return;
    }

    HAL_TIM_PWM_Start_DMA(&PWM_TIM, PWM_TIM_CHANNEL, ws2812_leds, LED_BUFFER_SIZE);

    ws2812_dirty = false;
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    ws2812_ready = false;
    ws2812_dirty = false;
}
