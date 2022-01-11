/**
 * @file screen.c
 */

#include "screen.h"
#include "amk_gpio.h"

#ifndef SCREEN_DRIVER
#define SCREEN_DRIVER_ST7735
#endif

#ifdef SCREEN_DRIVER_ST7735
    #include "st7735.h"
    #define screen_driver_t st7735_t
#else
    #ifdef SCREEN_DRIVER_RM67160
        #include "rm67160.h"
        #define screen_driver_t rm67160_t
    #else
        #include "ssd1357.h"
        #define screen_driver_t ssd1357_t 
    #endif
#endif

static screen_driver_t screen_drivers[SCREEN_NUM] = {
    {
#ifdef POWER_CHIP_PIN
    POWER_CHIP_PIN,
#endif
    SCREEN_0_RESET,
    SCREEN_0_CS,
    SCREEN_0_DC},
#ifdef SCREEN_1_PRESENT
    {SCREEN_1_RESET, SCREEN_1_CS, SCREEN_1_DC},
#endif
};

void screen_init(void)
{
    for (int i = 0; i < SCREEN_NUM; i++) {
    #ifdef POWER_CHIP_PIN
        gpio_set_output_pushpull(screen_drivers[i].ctrl);
    #endif
        gpio_set_output_pushpull(screen_drivers[i].reset);
        gpio_set_output_pushpull(screen_drivers[i].cs);
        gpio_set_output_pushpull(screen_drivers[i].dc);
    #ifdef POWER_CHIP_PIN
        gpio_write_pin(screen_drivers[i].ctrl, 0);
    #endif
        gpio_write_pin(screen_drivers[i].reset, 0);
        gpio_write_pin(screen_drivers[i].cs, 1);
        gpio_write_pin(screen_drivers[i].dc, 1);
#ifdef SCREEN_DRIVER_ST7735
        st7735_init(&screen_drivers[i]);
#else
    #ifdef SCREEN_DRIVER_RM67160
        rm67160_init(&screen_drivers[i]);
    #else
        ssd1357_init(&screen_drivers[i]);
    #endif
#endif
    }
}

__attribute__((weak))
void screen_task_kb(void) {}

void screen_task(void)
{
    screen_task_kb();
}

void screen_uninit(void)
{}

void screen_ticks(uint32_t ticks)
{
}

void screen_fill(const void* data)
{
#ifdef SCREEN_DRIVER_ST7735
    st7735_fill(&screen_drivers[0], data);
#else
    #ifdef SCREEN_DRIVER_RM67160
    rm67160_fill(&screen_drivers[0], data);
    #else
    ssd1357_fill(&screen_drivers[0], data);
    #endif
#endif
}

void screen_test(void)
{
}

void screen_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t data_size)
{
#ifdef SCREEN_DRIVER_ST7735
    st7735_fill_rect(&screen_drivers[0], x, y, width, height, data, data_size);
#else
    #ifdef SCREEN_DRIVER_RM67160
    rm67160_fill_rect(&screen_drivers[0], x, y, width, height, data, data_size);
    #else
    ssd1357_fill_rect(&screen_drivers[0], x, y, width, height, data, data_size);
    #endif
#endif
}

void screen_fill_rect_async(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t data_size)
{
#ifdef SCREEN_DRIVER_ST7735
    st7735_fill_rect_async(&screen_drivers[0], x, y, width, height, data, data_size);
#else
    rm67160_fill_rect_async(&screen_drivers[0], x, y, width, height, data, data_size);
#endif
}

bool screen_fill_ready(void)
{

#ifdef SCREEN_DRIVER_ST7735
    if (st7735_fill_ready(&screen_drivers[0])) {
        st7735_release(&screen_drivers[0]);
        return true;
    }
#endif
    return false;
}
