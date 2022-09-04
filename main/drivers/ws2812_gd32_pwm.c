/**
 * @file ws2812_gd32_pwm.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <stdbool.h>
#include "rgb_common.h"
#include "ws2812.h"

#ifndef PWM_TIM
#error "pwm time must been defined"
#endif

#ifndef PWM_TIM_CHANNEL
#error "pwm time channel been defined"
#endif

#ifndef PWM_TIM_PERIOD
#error "pwm time period must been defined"
#endif

//extern TIM_HandleTypeDef PWM_TIM;

#define LED_DATA_SIZE           (WS2812_LED_NUM * 24)
#define RESET_SLOTS_BEGIN       (100)
#define RESET_SLOTS_END         (100)
#define WS2812_LAST_SLOT        (1)
#define LED_BUFFER_SIZE         (RESET_SLOTS_BEGIN + LED_DATA_SIZE + WS2812_LAST_SLOT + RESET_SLOTS_END)
#ifndef WS2812_1
#define WS2812_1                ((PWM_TIM_PERIOD*2)/3)
#endif
#ifndef WS2812_0
#define WS2812_0                (PWM_TIM_PERIOD/3)
#endif
#define WS2812_RESET            (0)

static uint16_t ws2812_leds[LED_BUFFER_SIZE];
static bool ws2812_ready = false;
static bool ws2812_dirty = false;
static pin_t ws2812_pin;

//#define TIMER3_CH1CV  ((uint32_t)(0x40000838U))
#define TIMER3_CH2CV  ((uint32_t)(0x4000083CU))

void gpio_config(void);
void dma_config(void);
void timer_config(void);

void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    //gpio_pin_remap_config(GPIO_TIMER3_REMAP, ENABLE);

    /*configure PB7(TIMER3 CH1) as alternate function*/
    //gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_7);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8);
}

void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* initialize DMA channel3 */
    dma_deinit(DMA0,DMA_CH6);

    /* DMA channel5 initialize */
    dma_init_struct.periph_addr = (uint32_t)TIMER3_CH2CV;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_addr = (uint32_t)(ws2812_leds);
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.number = LED_BUFFER_SIZE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;

    dma_init(DMA0,DMA_CH6,&dma_init_struct);
    
    dma_circulation_enable(DMA0, DMA_CH6);
    dma_memory_to_memory_disable(DMA0, DMA_CH6);

    /* enable DMA channel5 */
    dma_channel_enable(DMA0,DMA_CH6);
}

void timer_config(void)
{
    //TIMER3CLK = 168MHz, Prescaler = 0
    //TIMER3 counter clock = systemcoreclock/1 =168MHz 

    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER3);

    timer_deinit(TIMER3);

    /* TIMER3 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = PWM_TIM_PERIOD;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    gd32_timer_init(TIMER3,&timer_initpara);

    /* CH1 configuration in PWM1 mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER3,TIMER_CH_2,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,0);
    timer_channel_output_mode_config(TIMER3,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3,TIMER_CH_2,TIMER_OC_SHADOW_ENABLE);

    /* TIMER3 primary output enable */
    timer_primary_output_config(TIMER3,ENABLE);
    /* channel DMA request source selection */
    timer_channel_dma_request_source_select(TIMER3,TIMER_DMAREQUEST_UPDATEEVENT);
    /* configure the TIMER DMA transfer */ 
    timer_dma_transfer_config(TIMER3,TIMER_DMACFG_DMATA_CH2CV,TIMER_DMACFG_DMATC_1TRANSFER);
    /* TIMER3 update DMA request enable */
    timer_dma_enable(TIMER3, TIMER_DMA_UPD);

    /* auto-reload preload enable */
    //timer_auto_reload_shadow_enable(TIMER3);
    timer_auto_reload_shadow_disable(TIMER3);

    /* TIMER3 counter enable */
    timer_enable(TIMER3);
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
        ws2812_leds[RESET_SLOTS_BEGIN+i] = WS2812_1;
    }

    ws2812_leds[RESET_SLOTS_BEGIN+LED_DATA_SIZE] = WS2812_0;

    gpio_config();
    dma_config();
    timer_config();
}

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    index = led->r;

#if 1
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
#endif
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

    //HAL_TIM_PWM_Start_DMA(&PWM_TIM, PWM_TIM_CHANNEL, ws2812_leds, LED_BUFFER_SIZE);
    //timer_config();
    ws2812_dirty = false;
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    ws2812_ready = false;
    ws2812_dirty = false;
}
