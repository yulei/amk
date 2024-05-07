/**
 * @file amk_ec_matrix.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
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

#ifndef DISCHARGE_WAIT_PRE
#define DISCHARGE_WAIT_PRE 0
#endif

#ifndef DISCHARGE_WAIT_POST
#define DISCHARGE_WAIT_POST 20
#endif

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04
#define COL_D_MASK  0x08
#define ADC_TIMEOUT 10

#ifdef MATRIX_ROW_PINS
static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
#endif

static pin_t custom_col_pins[] = MATRIX_COL_PINS;

extern ADC_HandleTypeDef hadc1;

//#define USE_ADC_DMA
#ifdef USE_ADC_DMA
DMA_HandleTypeDef           hdma_adc1;
static volatile uint32_t    adc1_dma_done;

int adc_init_kb(void)
{
    HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 1, 0);
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
    hadc1.Init.NbrOfConversion = ADC_CHANNEL_COUNT;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    sConfig.Channel = KEY_IN_CHANNEL;
    sConfig.Rank = 1;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    return 1;
}

int adc_msp_init_kb(void)
{ 
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = KEY_IN_PIN;
    HAL_GPIO_Init(KEY_IN_PORT, &GPIO_InitStruct);

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

    HAL_GPIO_DeInit(KEY_IN_PORT, KEY_IN_PIN);

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
static bool adc_read(uint32_t *data, uint32_t timeout)
{
    adc1_dma_done = 0;

    if( HAL_ADC_Start_DMA(&hadc1, data, 1) == HAL_OK) {
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

    return true;
}
#else
static bool adc_read(uint32_t *data, uint32_t timeout)
{
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, timeout);
        if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
            *data = HAL_ADC_GetValue(&hadc1);
        } else {
            HAL_ADC_Stop(&hadc1);
            custom_matrix_debug("key sense failed\n");
            return false;
        }
        HAL_ADC_Stop(&hadc1);
    }

    return true;
}
#endif

static bool sense_key(bool *key, uint32_t col, uint32_t row)
{
    wait_us(DISCHARGE_WAIT_PRE);
    gpio_set_input_floating(DISCHARGE_PIN);
    gpio_write_pin(custom_row_pins[row], 1);

    static uint32_t adc_value = 0;

    if (!adc_read(&adc_value, ADC_TIMEOUT)) {
        return false;
    }

    bool valid = false;
    *key = apc_matrix_update(row, col, adc_value, &valid);

    // clean up
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(custom_row_pins[row], 0);
    wait_us(DISCHARGE_WAIT_POST);

#if 0
    //if (key) {
    if (adc_value>1000) {
        custom_matrix_debug("key down: data=%d, row=%d, col=%d, key=%d\n", adc_value, row, col,*key);
    }
#endif
    return valid;
}

void matrix_init_custom(void)
{
#ifdef MATRIX_ROW_PINS
    for (int i = 0; i < AMK_ARRAY_SIZE(custom_row_pins); i++) {
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 0);
    }
#endif

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

    // initialize col pins
#ifdef LEFT_EN_PIN
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 1);
#endif

#ifdef RIGHT_EN_PIN
    gpio_set_output_pushpull(RIGHT_EN_PIN);
    gpio_write_pin(RIGHT_EN_PIN, 1);
#endif

    // initialize opamp and capacity discharge
#ifdef OPA_EN_PIN
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
#endif

    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);

    apc_matrix_init();
}

//#define SCAN_STAT
#ifdef SCAN_STAT
#define SCAN_COUNT   10000
static uint32_t scan_stat_count = 0;
static uint32_t scan_stat_ticks = 0;
#endif

#define CLEAR_AT_START
bool matrix_scan_custom(matrix_row_t* raw)
{
#ifdef SCAN_STAT
    if ((scan_stat_count % SCAN_COUNT) == 0) {
        amk_printf("MATRIX SCAN interval=%d, count = %d\n", timer_elapsed32(scan_stat_ticks), SCAN_COUNT);
        scan_stat_ticks = timer_read32();
    }
    scan_stat_count++;
#endif

    bool changed = false;

#ifdef CLEAR_AT_START
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(300);
#endif

    for (int col = 0; col < MATRIX_COLS; col++) {

        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);
#ifdef COL_D_PIN
        gpio_write_pin(COL_D_PIN, (custom_col_pins[col]&COL_D_MASK) ? 1 : 0);
#endif

#ifdef LEFT_EN_PIN
        if (custom_col_pins[col]&L_MASK) {
            gpio_write_pin(LEFT_EN_PIN,  0);
        }
#endif

#ifdef RIGHT_EN_PIN
        if (custom_col_pins[col]&R_MASK) {
            gpio_write_pin(RIGHT_EN_PIN, 0);
        }
#endif

        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            bool key = false;

            if (sense_key(&key, col, row)) {
                if (key) {
                    current_row_value |= (1 << col);
                } else {
                    current_row_value &= ~(1 << col);
                }
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
        }
#ifdef LEFT_EN_PIN
        gpio_write_pin(LEFT_EN_PIN,  1);
#endif
#ifdef RIGHT_EN_PIN
        gpio_write_pin(RIGHT_EN_PIN, 1);
#endif
    }

#if 1
    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
#endif

    return changed;
}
