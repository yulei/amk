/**
 * @file ws2812_stm32_pwm.c
 *  
 *  timer pwm based driver for ws2812
 * 
 */

#include <stdbool.h>
#include "rgb_common.h"
#include "ws2812.h"

#ifndef PWM_TIM
#define PWM_TIM htim1
#endif

#ifndef PWM_TIM_CHANNEL
#define PWM_TIM_CHANNEL TIM_CHANNEL_3
#endif

#ifndef PWM_TIM_PERIOD
#error "pwm time period must been defined"
#endif

extern TIM_HandleTypeDef PWM_TIM;

#define LED_DATA_SIZE           (WS2812_LED_NUM * 24)
#define RESET_SLOTS_BEGIN       (50)
#define RESET_SLOTS_END         (50)
#define WS2812_LAST_SLOT        (1)
#define LED_BUFFER_SIZE         (RESET_SLOTS_BEGIN + LED_DATA_SIZE + WS2812_LAST_SLOT + RESET_SLOTS_END)
#define WS2812_1                ((PWM_TIM_PERIOD*2)/3)
#define WS2812_0                (PWM_TIM_PERIOD/3)
#define WS2812_RESET            (0)

static uint32_t ws2812_leds[LED_BUFFER_SIZE];
static bool ws2812_ready = false;
static bool ws2812_dirty = false;
static pin_t ws2812_pin;

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
