/**
 * @file ws2812_nrf52.c
 */

#include "nrfx_pwm.h"
#include "nrf_gpio.h"
#include "ws2812.h"
#include "nrf_log.h"
#include "rgb_color.h"
#include "wait.h"

#define WS2812_VAL_0        0x8002
#define WS2812_VAL_1        0x8008

#define WS2812_COLOR_SIZE   (RGB_LED_NUM*24)
#define WS2812_RESET_SIZE   1
#define WS2812_BUF_SIZE     (WS2812_COLOR_SIZE + WS2812_RESET_SIZE)

static rgb_led_t ws2812_leds[RGB_LED_NUM];
static uint16_t ws2812_data[WS2812_BUF_SIZE];
static bool ws2812_ready = false;
static pin_t ws2812_pin;

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

static void ws2812_write_led(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
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

void ws2812_init(pin_t pin)
{
    ws2812_pin = pin;
    gpio_set_output_pushpull(pin);
    gpio_write_pin(pin, 0);
    
    nrfx_pwm_config_t config;
    config.output_pins[0] = pin;
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

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index < RGB_LED_NUM) {
        ws2812_leds[index].r = red;
        ws2812_leds[index].g = green;
        ws2812_leds[index].b = blue;
    }
}

void ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < RGB_LED_NUM; i++) {
        ws2812_leds[i].r = red;
        ws2812_leds[i].g = green;
        ws2812_leds[i].b = blue;
    }
}

void ws2812_update_buffers(pin_t pin)
{
    if (!ws2812_ready) {
        ws2812_init(pin);
    }

    for (int i = 0; i < RGB_LED_NUM; i++) {
        ws2812_write_led(i, ws2812_leds[i].r, ws2812_leds[i].g, ws2812_leds[i].b);
    }

    for (int j = 0; j < WS2812_RESET_SIZE; j++) {
        ws2812_data[WS2812_COLOR_SIZE + j] = 0x8000;
    }
    ws2812_pwm_seq.length = WS2812_BUF_SIZE; 
    nrfx_pwm_simple_playback(&ws2812_pwm, &ws2812_pwm_seq, 1, NRFX_PWM_FLAG_STOP);
    wait_ms(2);
        //NRF_LOG_INFO("ws2812 setleds playback: number: %d", number);
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    nrfx_pwm_uninit(&ws2812_pwm);
    ws2812_ready = false;
}
