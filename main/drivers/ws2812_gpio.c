/**
 * @file ws2812_gpio.c
 *  
 */

#include "amk_hal.h"
#include "amk_gpio.h"
#include "ws2812.h"
#include "rgb_common.h"

#ifndef WS2812_TIMING
#define WS2812_TIMING 1250
#endif

#ifndef WS2812_T1H
#define WS2812_T1H 900// Width of a 1 bit in ns
#endif

#ifndef WS2812_T1L
#define WS2812_T1L (WS2812_TIMING - WS2812_T1H) // Width of a 1 bit in ns
#endif

#ifndef WS2812_T0H
#define WS2812_T0H 350 // Width of a 0 bit in ns
#endif

#ifndef WS2812_T0L
#define WS2812_T0L (WS2812_TIMING - WS2812_T0H) // Width of a 0 bit in ns
#endif

#ifndef WS2812_TRST_US
#define WS2812_TRST_US 280
#endif

#ifndef NOP_FUDGE
#define NOP_FUDGE 0.4
#endif

// The reset gap can be 6000 ns, but depending on the LED strip it may have to be increased
// to values like 600000 ns. If it is too small, the pixels will show nothing most of the time.
#ifndef WS2812_RES
#define WS2812_RES (1000 * WS2812_TRST_US) // Width of the low gap between bits to cause a frame to latch
#endif

extern uint32_t SystemCoreClock;

#define NUMBER_NOPS 6
#define CYCLES_PER_SEC (SystemCoreClock / NUMBER_NOPS * NOP_FUDGE)
#define NS_PER_SEC (1000000000L) // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
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

static void sendByte(uint8_t byte)
{
    // WS2812 protocol wants most significant bits first
    for (unsigned char bit = 0; bit < 8; bit++) {
        bool is_one = byte & (1 << (7 - bit));
        // using something like wait_ns(is_one ? T1L : T0L) here throws off timings
        if (is_one) {
            // 1
            gpio_write_pin(WS2812_LED_PIN, 1);
            wait_ns(WS2812_T1H);
            gpio_write_pin(WS2812_LED_PIN, 0);
            wait_ns(WS2812_T1L);
        } else {
            // 0
            gpio_write_pin(WS2812_LED_PIN, 1);
            wait_ns(WS2812_T0H);
            gpio_write_pin(WS2812_LED_PIN, 0);
            wait_ns(WS2812_T0L);
        }
    }
}

static bool ws2812_ready = false;
static bool ws2812_dirty = false;

void ws2812_init(pin_t pin)
{
    gpio_set_output_pushpull(WS2812_LED_PIN);
    ws2812_ready = true;
}

static uint8_t ws2812_leds[WS2812_LED_NUM*3];

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    index = led->r;

    if (index < WS2812_LED_NUM) {
        ws2812_leds[index+0] = red;
        ws2812_leds[index+1] = green;
        ws2812_leds[index+2] = blue;
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

    for (uint8_t i = 0; i < WS2812_LED_NUM; i++) {
        sendByte(ws2812_leds[i*3+1]);
        sendByte(ws2812_leds[i*3+0]);
        sendByte(ws2812_leds[i*3+2]);
    }

    wait_ns(WS2812_RES);

    ws2812_dirty = false;
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    ws2812_ready = false;
    ws2812_dirty = false;
}
