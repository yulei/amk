/**
 * @file amk_ms_matrix.c
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
#include "amk_snaptap.h"

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

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04
#define COL_D_MASK  0x08
#define ADC_TIMEOUT 10

static uint32_t adc_value[ADC_CHANNEL_COUNT];
static volatile uint32_t adc1_dma_done;

#ifndef ADC_CHANNEL_COUNT
#error "ADC_CHANNEL_COUNT not defined"
#endif

static pin_t adc_pins[ADC_CHANNEL_COUNT]= ADC_PINS;
#ifdef MATRIX_COL_PINS
static pin_t custom_col_pins[] = MATRIX_COL_PINS;
#endif
#ifdef MATRIX_ROW_PINS
static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
#endif

#ifdef MS_SWITCH_PINS
static pin_t custom_switch_pins[] = MS_SWITCH_PINS;
#endif

#define DWT_PROFILE(x) \
static uint32_t x##_count = 0; \
static uint32_t x##_value = 0; \

#define DWT_PROFILE_START(x) \
uint32_t x##_start = DWT->CYCCNT;

#define DWT_PROFILE_STAT(x, fmt, count) \
uint32_t x##_interval = TIMER_DIFF_32(DWT->CYCCNT, x##_start); \
x##_value += x##_interval; \
x##_count++; \
if ((x##_count % count) == 0) { \
    amk_printf(fmt, x##_value, count); \
    x##_value = 0; \
}

#define MS_SCAN_STAT
//#define ADC_READ_STAT
//#define ADC_DMA_STAT
//#define APC_UPDATE_STAT
//#define ADC_MATRIX_STAT
//#define ADC_DMA_IRQ_STAT
//#define ADC_WAIT_STAT
//#define ADC_SAMPLE_STAT

#ifdef ADC_SAMPLE_STAT
static uint32_t adc_sample_start = 0;
static uint32_t adc_sample_value = 0;
static uint32_t adc_sample_count =0;
#endif

#ifdef AT32F405
void adc1_pins_init(void)
{
    gpio_init_type gpio_init_struct;
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;

    for (int i = 0; i < AMK_ARRAY_SIZE(adc_pins); i++) {
        gpio_init_struct.gpio_pins = GET_PIN(adc_pins[i]);
        gpio_init(GET_PORT(adc_pins[i]), &gpio_init_struct);
    }
}

void adc1_dma_init(void)
{
    dma_init_type dma_init_struct;
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    nvic_irq_enable(DMA1_Channel1_IRQn, 1, 0);

    dma_reset(DMA1_CHANNEL1);
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = ADC_CHANNEL_COUNT;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)adc_value;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_WORD;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_WORD;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_HIGH;
    dma_init_struct.loop_mode_enable = TRUE;
    dma_init(DMA1_CHANNEL1, &dma_init_struct);

    dmamux_enable(DMA1, TRUE);
    dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_ADC1);

    /* enable dma transfer complete interrupt */
    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
    dma_channel_enable(DMA1_CHANNEL1, TRUE);
}

void DMA1_Channel1_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA1_FDT1_FLAG) != RESET) {
        dma_flag_clear(DMA1_FDT1_FLAG);
        adc1_dma_done = 1;
    }
}

#else

extern ADC_HandleTypeDef    hadc1;
DMA_HandleTypeDef           hdma_adc1;
static uint32_t adc_channels[ADC_CHANNEL_COUNT] = ADC_CHANNELS;

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
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;//ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    for ( int i = 0; i < ADC_CHANNEL_COUNT; i++) {
        sConfig.Channel = adc_channels[i];
        sConfig.Rank = i+1;
        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
            Error_Handler();
        }
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
    for (int i = 0; i < AMK_ARRAY_SIZE(adc_pins); i++) {
        GPIO_InitStruct.Pin = GET_PIN(adc_pins[i]);
        HAL_GPIO_Init(GET_PORT(adc_pins[i]), &GPIO_InitStruct);
    }

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

    for (int i = 0; i < AMK_ARRAY_SIZE(adc_pins); i++) {
        HAL_GPIO_DeInit(GET_PORT(adc_pins[i]), GET_PIN(adc_pins[i]));
    }

    return 1;
}

#ifdef ADC_DMA_IRQ_STAT
#define ADC_DMA_IRQ_COUNT   1000
DWT_PROFILE(adc_dma_irq) 
#endif

void DMA2_Stream4_IRQHandler(void)
{
#ifdef ADC_SAMPLE_STAT
    uint32_t interval = TIMER_DIFF_32(DWT->CYCCNT, adc_sample_start);
    adc_sample_value += interval;
    adc_sample_count ++;
    if ((adc_sample_count % 1000) == 0) { \
        amk_printf("ADC sample time: intervale=%d, count=%d\n", adc_sample_value, 1000);
        adc_sample_value = 0; \
    }
#endif
#ifdef ADC_DMA_IRQ_STAT
DWT_PROFILE_START(adc_dma_irq);
#endif

    HAL_DMA_IRQHandler(&hdma_adc1);

#ifdef ADC_DMA_IRQ_STAT 
DWT_PROFILE_STAT(adc_dma_irq, "ADC DMA IRQ profile: interverl=%d, count=%d\n", ADC_DMA_IRQ_COUNT);
#endif 
    //adc1_dma_done = 1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    adc1_dma_done = 1;
}
#endif

#ifdef ADC_DMA_STAT
#define ADC_DMA_COUNT   1000
DWT_PROFILE(adc_dma) 
#endif

#ifdef ADC_WAIT_STAT
#define ADC_WAIT_COUNT   1000
DWT_PROFILE(adc_wait) 
#endif
static bool adc_read(uint32_t *data, uint32_t timeout)
{
    adc1_dma_done = 0;

#ifdef AT32F405
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    if (1) {
#else
#ifdef ADC_SAMPLE_STAT
    adc_sample_start = DWT->CYCCNT;
#endif

#ifdef ADC_DMA_STAT
DWT_PROFILE_START(adc_dma);
#endif
    if( HAL_ADC_Start_DMA(&hadc1, data, ADC_CHANNEL_COUNT) == HAL_OK) {
#endif
#ifdef ADC_DMA_STAT 
    DWT_PROFILE_STAT(adc_dma, "ADC DMA profile: interverl=%d, count=%d\n", ADC_DMA_COUNT);
#endif    

#ifdef ADC_WAIT_STAT
DWT_PROFILE_START(adc_wait);
#endif
        uint32_t start = timer_read32();
        while(!adc1_dma_done) {
            if (timer_elapsed32(start) > timeout) {
                custom_matrix_debug("adc_read timeout.\n");
                return false;
            }
        }
#ifdef ADC_WAIT_STAT 
    DWT_PROFILE_STAT(adc_wait, "ADC WAIT profile: interverl=%d, count=%d\n", ADC_WAIT_COUNT);
#endif    
    } else {
        custom_matrix_debug("adc_read Start_DMA failed.\n");
        return false;
    }

    return true;
}


#ifdef ADC_READ_STAT
#define ADC_READ_COUNT   1000
DWT_PROFILE(adc_read) 
#endif

#ifdef APC_UPDATE_STAT
#define APC_UPDATE_COUNT   1000
DWT_PROFILE(apc_update) 
#endif

static bool sense_key(bool *keys, uint32_t index)
{
#ifdef ADC_READ_STAT
DWT_PROFILE_START(adc_read);
#endif
    if ( !adc_read(adc_value, ADC_TIMEOUT)) {
        return false;
    }
#ifdef ADC_READ_STAT 
    DWT_PROFILE_STAT(adc_read, "ADC READ profile: interverl=%d, count=%d\n", ADC_READ_COUNT);
#endif    

#ifdef APC_UPDATE_STAT
DWT_PROFILE_START(apc_update);
#endif
    bool valid = false;
#ifdef MS_SWITCH_PINS
    for (int i = 0; i < MATRIX_COLS; i++) {
        keys[i] = apc_matrix_update(index, i, adc_value[i], &valid);
    }
#else
    for (int i = 0; i < MATRIX_ROWS; i++) {
        keys[i] = apc_matrix_update(i, index, adc_value[i], &valid);
    }
#endif

#ifdef APC_UPDATE_STAT 
    DWT_PROFILE_STAT(apc_update, "APC UPDATE profile: interverl=%d, count=%d\n", APC_UPDATE_COUNT);
#endif    
#if 0
    for (int i = 0; i < MATRIX_ROWS; i++) {
        if (keys[i])
        {
            custom_matrix_debug("key down: data=%d, row=%d, col=%d\n", adc_value[i], i, col);
        }
    }
#endif
    return valid;
}

//#define MS_MATRIX_SLOW_START
#ifdef MS_MATRIX_SLOW_START
#define MS_POWERUP_PERIOD   1000
#define MS_SELFCAL_PERIOD   3000
static uint32_t ms_last_ticks = 0;
enum {
    MS_POWERUP,
    MS_SELFCAL,
    MS_RUN,
};
static int ms_matrix_state = MS_POWERUP;

static void ms_calibrate(void)
{
    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);
#ifdef COL_D_PIN
        gpio_write_pin(COL_D_PIN, (custom_col_pins[col]&COL_D_MASK) ? 1 : 0);
#endif
        if ( !adc_read(adc_value, ADC_TIMEOUT)) {
            return;
        }
        for (int i = 0; i < MATRIX_ROWS; i++) {
            apc_matrix_calibrate(i, col, adc_value[i]);
        }
    }
}

static bool ms_can_run(void)
{
    switch(ms_matrix_state) {
        case MS_POWERUP:
            if (timer_elapsed32(ms_last_ticks) > MS_POWERUP_PERIOD) {
                ms_last_ticks = timer_read32();
                ms_matrix_state = MS_SELFCAL;
            }
        break;
        case MS_SELFCAL:
            if (timer_elapsed32(ms_last_ticks) > MS_SELFCAL_PERIOD) {
                ms_last_ticks = timer_read32();
                ms_matrix_state = MS_RUN;
            } else {
                ms_calibrate();
            }
        break;
        case MS_RUN:
        default:
        return true;
    }

    return false;
}
#endif

#ifdef DIGITAL_POT_ENABLE
#include "digital_pot.h"
#endif

void matrix_init_custom(void)
{
#ifdef DIGITAL_POT_ENABLE
    digital_pot_init();
    digital_pot_set(DIGITAL_POT_INDEX);
#endif

#ifdef MS_SWITCH_PINS
    for (int i = 0; i < AMK_ARRAY_SIZE(custom_switch_pins); i++) {
        gpio_set_output_pushpull(custom_switch_pins[i]);
        gpio_write_pin(custom_switch_pins[i], 1);
    }
#endif

#ifdef MATRIX_ROW_PINS
    for (int i = 0; i < AMK_ARRAY_SIZE(custom_row_pins); i++) {
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 0);
    }
#endif

#ifdef MATRIX_COL_PINS
    #ifdef SELECT_EN_PIN
        gpio_set_output_pushpull(SELECT_EN_PIN);
        gpio_write_pin(SELECT_EN_PIN, 0);
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
#endif

    apc_matrix_init();

#ifdef AMK_SNAPTAP_ENABLE
    snaptap_init();
#endif
#ifdef MS_MATRIX_SLOW_START
    ms_last_ticks = timer_read32();
#endif
}

#ifdef MS_SCAN_STAT
#define MS_SCAN_COUNT   10000
static uint32_t scan_stat_count = 0;
static uint32_t scan_stat_ticks = 0;
#endif

#ifdef ADC_MATRIX_STAT
#define ADC_MATRIX_COUNT   1000
DWT_PROFILE(adc_matrix) 
#endif
bool matrix_scan_custom(matrix_row_t* raw)
{
#ifdef MS_SCAN_STAT
    if ((scan_stat_count % MS_SCAN_COUNT) == 0) {
        amk_printf("MS SCAN interval=%d, count = %d\n", timer_elapsed32(scan_stat_ticks), MS_SCAN_COUNT);
        scan_stat_ticks = timer_read32();
    }
    scan_stat_count++;
#endif

#ifdef MS_MATRIX_SLOW_START
    if (!ms_can_run()) return false;
#endif

#ifdef ADC_MATRIX_STAT
DWT_PROFILE_START(adc_matrix);
#endif
    bool changed = false;

#ifdef MS_SWITCH_PINS
    for (int row = 0; row < MATRIX_ROWS; row++) {
        gpio_write_pin(custom_switch_pins[row], 0);
        wait_us(1);
        bool keys[ADC_CHANNEL_COUNT] = {[0 ... ADC_CHANNEL_COUNT-1] = false};
        if (sense_key(keys, row) ) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;
            for ( int col = 0; col < MATRIX_COLS; col++) {
                if (keys[col]) {
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
        gpio_write_pin(custom_switch_pins[row], 1);
    }
#else
    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);
#ifdef COL_D_PIN
        gpio_write_pin(COL_D_PIN, (custom_col_pins[col]&COL_D_MASK) ? 1 : 0);
#endif
        wait_us(1);

        bool keys[ADC_CHANNEL_COUNT] = {[0 ... ADC_CHANNEL_COUNT-1] = false};
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
#endif

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }

#ifdef AMK_SNAPTAP_ENABLE
    snaptap_process(raw);
#endif
#ifdef ADC_MATRIX_STAT 
    DWT_PROFILE_STAT(adc_matrix, "ADC MATRIX profile: interverl=%d, count=%d\n", ADC_MATRIX_COUNT);
#endif    

    return changed;
}
