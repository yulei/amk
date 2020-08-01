/**
 * @file ws2812.c
 */

#include "nrfx_pwm.h"
#include "nrf_gpio.h"
#include "ws2812.h"
#include "nrf_log.h"
#include "wait.h"

#ifdef WS2812_DRIVER_ENABLE

#define WS2812_VAL_0        0x8002
#define WS2812_VAL_1        0x8008

#define WS2812_COLOR_SIZE   (RGBLED_NUM*24)
#define WS2812_RESET_SIZE   1
#define WS2812_BUF_SIZE     (WS2812_COLOR_SIZE + WS2812_RESET_SIZE)

static uint16_t ws2812_data[WS2812_BUF_SIZE];
static bool ws2812_ready = false;

nrfx_pwm_t ws2812_pwm = NRFX_PWM_INSTANCE(0);
nrf_pwm_sequence_t ws2812_pwm_seq = {
    .values.p_raw = ws2812_data,
    .length = WS2812_BUF_SIZE,
    .repeats = 0,
    .end_delay = 32,
};

static void ws2812_write_color(uint8_t c, uint16_t offset)
{
    for (int i = 0; i < 8; i++) {
        ws2812_data[offset] = (c & (1<<(7-i))) ? WS2812_VAL_1 : WS2812_VAL_0;
    }
}

static void ws2812_write_led(uint8_t r, uint8_t g, uint8_t b, uint16_t index)
{
    ws2812_write_color(g, index*24);
    ws2812_write_color(r, index*24 + 8);
    ws2812_write_color(b, index*24 + 16);
}

static void pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    if (event_type == NRFX_PWM_EVT_FINISHED) {
    }
}

void ws2812_init(void)
{
    nrf_gpio_pin_clear(WS2812_RGB_PIN);
#ifdef WS2812_EN_HIGH
    nrf_gpio_pin_set(WS2812_EN_PIN);
#else
    nrf_gpio_pin_clear(WS2812_EN_PIN);
#endif
    nrf_gpio_cfg_output(WS2812_RGB_PIN);
    nrf_gpio_cfg_output(WS2812_EN_PIN);

    nrfx_pwm_config_t config;
    config.output_pins[0] = WS2812_RGB_PIN;
    config.output_pins[1] = NRFX_PWM_PIN_NOT_USED;
    config.output_pins[2] = NRFX_PWM_PIN_NOT_USED;
    config.output_pins[3] = NRFX_PWM_PIN_NOT_USED;
    config.base_clock = NRF_PWM_CLK_8MHz;
    config.top_value = 10; // clock = 8 MHz, we need a 800 kHz signal
    config.count_mode = NRF_PWM_MODE_UP;
    config.irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY;
    config.load_mode = NRF_PWM_LOAD_COMMON;
    config.step_mode = NRF_PWM_STEP_AUTO;

    nrfx_pwm_init(&ws2812_pwm, &config, pwm_handler);

    for (int i = 0; i < WS2812_COLOR_SIZE; i++) {
        ws2812_data[i] = WS2812_VAL_0;
    }
    for (int j = 0; j < WS2812_RESET_SIZE; j++) {
        ws2812_data[WS2812_COLOR_SIZE+j] = 0x8000;
    }
    ws2812_pwm_seq.length = WS2812_BUF_SIZE;
    //nrfx_pwm_simple_playback(&ws2812_pwm, &ws2812_pwm_seq, 1, NRFX_PWM_FLAG_STOP);
    //wait_ms(2);


    ws2812_ready = true;
}

void ws2812_setleds(LED_TYPE* leds, uint16_t number)
{
    if (!ws2812_ready) {
        ws2812_init();
    }

    for (uint16_t i = 0; i < number; i++) {
        ws2812_write_led(leds[i].r, leds[i].g, leds[i].b, i);
    }

    if (number <= RGBLED_NUM) {
        for (int j = 0; j < WS2812_RESET_SIZE; j++) {
            ws2812_data[number*24 + j] = 0x8000;
        }
        ws2812_pwm_seq.length = number*24 + WS2812_RESET_SIZE;
        nrfx_pwm_simple_playback(&ws2812_pwm, &ws2812_pwm_seq, 1, NRFX_PWM_FLAG_STOP);
        wait_ms(2);
        NRF_LOG_INFO("ws2812 setleds playback: number: %d", number);
    }
}

void ws2812_uninit(void)
{
    if (!ws2812_ready) return;

    nrfx_pwm_uninit(&ws2812_pwm);
#ifdef WS2812_EN_HIGH
    nrf_gpio_pin_clear(WS2812_EN_PIN);
#else
    nrf_gpio_pin_set(WS2812_EN_PIN);
#endif
    ws2812_ready = false;
}

#else

void ws2812_setleds(LED_TYPE* leds, uint16_t number)
{}
void ws2812_uninit(void)
{}

#endif
