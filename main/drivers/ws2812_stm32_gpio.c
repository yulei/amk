/**
 * @file ws2812_stm32_gpio.c
 *  
 * @brief ws2812 gpio driver
 * 
 */

#include <stdbool.h>
#include "stm32l0xx_ll_gpio.h"
#include "rgb_common.h"
#include "ws2812.h"
#include "amk_printf.h"
#include "wait.h"

#define LED_BUFFER_SIZE         (WS2812_LED_NUM * 3)
#define RESET_TIME              (50)

static uint8_t ws2812_leds[LED_BUFFER_SIZE];
static bool ws2812_ready = false;
static bool ws2812_dirty = false;

static pin_t ws2812_pin;

#if 0
static GPIO_TypeDef *ws2812_port;
static uint32_t ws2812_pin;

static uint32_t get_mask(uint16_t pin)
{
    switch(pin) {
        case GPIO_PIN_0:
            return LL_GPIO_PIN_0;
        case GPIO_PIN_1:
            return LL_GPIO_PIN_1;
        case GPIO_PIN_2:
            return LL_GPIO_PIN_2;
        case GPIO_PIN_3:
            return LL_GPIO_PIN_3;
        case GPIO_PIN_4:
            return LL_GPIO_PIN_4;
        case GPIO_PIN_5:
            return LL_GPIO_PIN_5;
        case GPIO_PIN_6:
            return LL_GPIO_PIN_6;
        case GPIO_PIN_7:
            return LL_GPIO_PIN_7;
        case GPIO_PIN_8:
            return LL_GPIO_PIN_8;
        case GPIO_PIN_9:
            return LL_GPIO_PIN_9;
        case GPIO_PIN_10:
            return LL_GPIO_PIN_10;
        case GPIO_PIN_11:
            return LL_GPIO_PIN_11;
        case GPIO_PIN_12:
            return LL_GPIO_PIN_12;
        case GPIO_PIN_13:
            return LL_GPIO_PIN_13;
        case GPIO_PIN_14:
            return LL_GPIO_PIN_14;
        case GPIO_PIN_15:
            return LL_GPIO_PIN_15;
        default:
            break;
    }

    amk_printf("not valid gpio pin\n");
    return LL_GPIO_PIN_ALL;
}
#endif

#define NOP_TIME (100/4)
#define CYCLES_PER_SEC (32 * 4 / (6))
#define NS_PER_SEC (10)
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

#define wait_long \
        __asm__ volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop;"); 

#define wait_short \
        __asm__ volatile("nop; nop; nop; ");

// These are the timing constraints taken mostly from the WS2812 datasheets
// These are chosen to be conservative and avoid problems rather than for maximum throughput

#define T1H 900           // Width of a 1 bit in ns
#define T1L (1250 - T1H)  // Width of a 1 bit in ns

#define T0H 350           // Width of a 0 bit in ns
#define T0L (1250 - T0H)  // Width of a 0 bit in ns

// The reset gap can be 6000 ns, but depending on the LED strip it may have to be increased
// to values like 600000 ns. If it is too small, the pixels will show nothing most of the time.
#define RES (1000 * WS2812_TRST_US)  // Width of the low gap between bits to cause a frame to latch

static void send_byte(uint8_t byte)
{
    // WS2812 protocol wants most significant bits first
    for (unsigned char bit = 0; bit < 8; bit++) {
        if (byte & (1 << (7 - bit))) {
            // 1
            LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
            wait_long;
            LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);
            wait_short;

        } else {
            // 0
            LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
            wait_short;
            LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);
            wait_long;
        }
    }
}

void ws2812_init(pin_t pin)
{
    if (ws2812_ready) return;

    //ws2812_port = GET_PORT(pin);
    //ws2812_pin = get_mask(GET_PIN(pin));

    //LL_GPIO_SetPinOutputType(ws2812_port, ws2812_pin, LL_GPIO_OUTPUT_PUSHPULL);
    //LL_GPIO_ResetOutputPin(ws2812_port, ws2812_pin);


    ws2812_pin = pin;
    gpio_set_output_pushpull(ws2812_pin);
    gpio_write_pin(ws2812_pin, 0);

    ws2812_ready = true;
    ws2812_dirty = true;

    uint32_t i = 0;
    for (i = 0; i < LED_BUFFER_SIZE; i++) {
        ws2812_leds[i] = 0xFF;
    }
}

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    uint32_t i = led->r;

    if (i < WS2812_LED_NUM) {
        // green
        ws2812_leds[i*3+0] = 0xFF;//green;
        // red
        ws2812_leds[i*3+1] = 0xFF;//red;
        // blue
        ws2812_leds[i*3+2] = 0xFF;//blue;
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
    __enable_irq();
    ws2812_dirty = false;
}

void ws2812_uninit(pin_t pin)
{
    if (!ws2812_ready) return;

    ws2812_ready = false;
    ws2812_dirty = false;
}
