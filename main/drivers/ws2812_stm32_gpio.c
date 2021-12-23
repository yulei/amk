/**
 * @file ws2812_stm32_gpio.c
 *  
 * @brief ws2812 gpio driver
 * 
 */

#include <stdbool.h>
#include "rgb_common.h"
#include "ws2812.h"
#include "amk_printf.h"
#include "wait.h"


#if !defined(WS2812_TRST_US)
#    define WS2812_TRST_US 280
#endif

#define LED_BUFFER_SIZE (WS2812_LED_NUM*3)
static uint8_t ws2812_leds[LED_BUFFER_SIZE];
static bool ws2812_ready = false;
static bool ws2812_dirty = false;
static pin_t ws2812_pin;

#define NOP_FUDGE 0.4

#define NUMBER_NOPS 6
#define CYCLES_PER_SEC (SystemCoreClock / NUMBER_NOPS * NOP_FUDGE)
#define NS_PER_SEC (1000000000L)  // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

#define wait_ns(x)                                  \
    do {                                            \
        for (int i = 0; i < NS_TO_CYCLES(x); i++) { \
            __asm__ volatile("nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t");            \
        }                                           \
    } while (0)

// These are the timing constraints taken mostly from the WS2812 datasheets
// These are chosen to be conservative and avoid problems rather than for maximum throughput

#define T1H 900           // Width of a 1 bit in ns
#define T1L (1250 - T1H)  // Width of a 1 bit in ns

#define T0H 350           // Width of a 0 bit in ns
#define T0L (1250 - T0H)  // Width of a 0 bit in ns

// The reset gap can be 6000 ns, but depending on the LED strip it may have to be increased
// to values like 600000 ns. If it is too small, the pixels will show nothing most of the time.
#define RES (1000 * WS2812_TRST_US)  // Width of the low gap between bits to cause a frame to latch

void send_byte(uint8_t byte)
{
    // WS2812 protocol wants most significant bits first
    for (unsigned char bit = 0; bit < 8; bit++) {
        bool is_one = byte & (1 << (7 - bit));
        // using something like wait_ns(is_one ? T1L : T0L) here throws off timings
        if (is_one) {
            // 1
            gpio_write_pin(ws2812_pin, 1);
            wait_ns(T1H);
            gpio_write_pin(ws2812_pin, 0);
            wait_ns(T1L);
        } else {
            // 0
            gpio_write_pin(ws2812_pin, 1);
            wait_ns(T0H);
            gpio_write_pin(ws2812_pin, 0);
            wait_ns(T0L);
        }
    }
}

void ws2812_init(pin_t pin)
{
    if (ws2812_ready) return;

    ws2812_pin = pin;
    gpio_set_output_pushpull(ws2812_pin);
    gpio_write_pin(ws2812_pin, 0);

    ws2812_ready = true;
    ws2812_dirty = true;

    uint32_t i = 0;
    for (i = 0; i < LED_BUFFER_SIZE; i++) {
        ws2812_leds[i] = 0;
    }
}

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    uint32_t i = led->r;

    if (i < WS2812_LED_NUM) {
        // green
        ws2812_leds[i*3+0] = green;
        // red
        ws2812_leds[i*3+1] = red;
        // blue
        ws2812_leds[i*3+2] = blue;
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

//
    __disable_irq();
    for (uint32_t i = 0; i < WS2812_LED_NUM; i++) {
        send_byte(i*3+0);
        send_byte(i*3+1);
        send_byte(i*3+2);
    }
    wait_ns(RES);
    __enable_irq();
    ws2812_dirty = false;
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    ws2812_ready = false;
    ws2812_dirty = false;
}
