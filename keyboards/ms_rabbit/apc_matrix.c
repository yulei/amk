/**
 * @file apc_matrix.c
 */

#include "matrix.h"
#include "wait.h"
#include "led.h"
#include "timer.h"
#include "amk_apc.h"

#include "amk_gpio.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifndef CUSTOM_MATRIX_DEBUG
#define CUSTOM_MATRIX_DEBUG 1
#endif

#if CUSTOM_MATRIX_DEBUG
#define custom_matrix_debug  amk_printf
#else
#define custom_matrix_debug(...)
#endif

#define MS_DELAY_START
#ifdef MS_DELAY_START
#define MS_DELAY    1000
static uint32_t ms_start = 0;
#endif

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04
#define COL_D_MASK  0x08
#define ADC_TIMEOUT 10

extern ADC_HandleTypeDef    hadc1;
DMA_HandleTypeDef           hdma_adc1;
static volatile uint32_t    adc1_dma_done;
#define ADC_SCAN_COUNT      1
#define ADC_CHANNEL_COUNT   4

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS;

static bool adc_read(uint32_t *data, uint32_t timeout)
{
    adc1_dma_done = 0;
    static uint32_t adc_value[ADC_CHANNEL_COUNT];
    
    for (int i = 0; i < ADC_SCAN_COUNT; i++) {
        if( HAL_ADC_Start_DMA(&hadc1, adc_value, ADC_CHANNEL_COUNT) == HAL_OK) {
            uint32_t start = timer_read32();
            while( !adc1_dma_done) {
                if (timer_elapsed32(start) > timeout) {
                    custom_matrix_debug("adc_read timeout.\n");
                    return false;
                }
            }
        } else {
            custom_matrix_debug("adc_read Start_DMA failed.\n");
            return false;
        }

        for (int j = 0; j < ADC_CHANNEL_COUNT; j++) {
            data[j] = data[j]+adc_value[j];
        }
    }

    for (int k = 0; k < ADC_CHANNEL_COUNT; k++) {
        data[k] /= ADC_SCAN_COUNT;
    }

    return true;
}

static bool sense_key(bool *keys, uint32_t col)
{
    static uint32_t adc_value[ADC_CHANNEL_COUNT];
    for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        adc_value[i] = 0;
    }

    if ( !adc_read(adc_value, ADC_TIMEOUT)) {
        return false;
    }
    
    for (int i = 0; i < MATRIX_ROWS; i++) {
        keys[i] = apc_matrix_update(i, col, adc_value[i]);
    }

/*
    for (int i = 0; i < MATRIX_ROWS; i++) {
        if (keys[i]) {
            custom_matrix_debug("key down: data=%d, row=%d, col=%d\n", adc_value[i], i, col);
        }
    }
*/

    return true;
}

void matrix_init_custom(void)
{
    // initialize row pins
    for (int i = 0; i < AMK_ARRAY_SIZE(custom_row_pins); i++) {
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 0);
    }

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);
#ifdef COL_D_PIN
    gpio_set_output_pushpull(COL_D_PIN);
    gpio_write_pin(COL_D_PIN, 0);
#endif

    apc_matrix_init();
#ifdef MS_DELAY_START
    ms_start = timer_read32();
#endif
}

bool matrix_scan_custom(matrix_row_t* raw)
{
#ifdef MS_DELAY_START
    if (timer_elapsed32(ms_start) < MS_DELAY) {
        return false;
    }
#endif

    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);
        gpio_write_pin(COL_D_PIN, (custom_col_pins[col]&COL_D_MASK) ? 1 : 0);

        bool keys[4] = {false, false, false, false};
        if (sense_key(keys, col) ) {
            for ( int row = 0; row < MATRIX_ROWS; row++) {
                matrix_row_t last_row_value    = raw[row];
                matrix_row_t current_row_value = last_row_value;
                if (keys[row]) {
                    current_row_value |= (1 << col);
                } else {
                    current_row_value &= ~(1 << col);
                }

                if (last_row_value != current_row_value) {
                    raw[row] = current_row_value;
                    changed = true;
                }
            }
        }
    }

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }

    return changed;
}

int adc_init_kb(void)
{
    HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 4;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    return 1;
}

int adc_msp_init_kb(void)
{ 
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    hdma_adc1.Instance = DMA2_Stream4;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_NORMAL;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
    return 1;
}

int adc_msp_uninit_kb(void)
{
    __HAL_RCC_ADC1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    return 1;
}

void DMA2_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    adc1_dma_done = 1;
}
