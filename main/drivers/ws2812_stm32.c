/**
 * @file ws2812_stm32.c
 * 
 *  from https://github.com/qmk/qmk_firmware/blob/master/drivers/chibios/ws2812.c
 */

#include <stdbool.h>
#include "gpio_pin.h"
#include "ws2812.h"
#include "rgb_color.h"

static rgb_led_t ws2812_leds[RGB_LED_NUM];
static bool ws2812_ready = false;
static pin_t ws2812_pin;

#ifndef NOP_FUDGE
#define NOP_FUDGE 0.4
#endif

#ifndef WS2812_TRST_US
#define WS2812_TRST_US 280
#endif

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

void sendByte(uint8_t byte) {
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
    ws2812_pin = pin;
    gpio_set_output_pushpull(pin);
    gpio_write_pin(pin, 0);
    
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

    __disable_irq();
    for (int i = 0; i < RGB_LED_NUM; i++) {
        // WS2812 protocol dictates grb order
        sendByte(ws2812_leds[i].g);
        sendByte(ws2812_leds[i].r);
        sendByte(ws2812_leds[i].b);
    }
    wait_ns(RES);
    __enable_irq();
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    ws2812_ready = false;
}
