/**
 * @file screen.c
 */

#include "screen.h"
#include "spi_lcd.h"

#ifdef SCREEN_DRIVER_ST7735
    #include "st7735.h"
    #define SPI_LCD_DRIVER_TYPE SPI_LCD_ST7735 
#else
    #ifdef SCREEN_DRIVER_RM67160
        #include "rm67160.h"
        #define SPI_LCD_DRIVER_TYPE SPI_LCD_RM67160
    #else
        #include "st7789.h"
        #define SPI_LCD_DRIVER_TYPE SPI_LCD_ST7789
    #endif
#endif

static spi_lcd_param_t screen_param = {
#ifdef POWER_CHIP_PIN
    POWER_CHIP_PIN,
#endif
    SCREEN_0_RESET,
    SCREEN_0_CS,
    SCREEN_0_DC,
};

static spi_lcd_driver_t *lcd_screen;

void screen_init(void)
{
#ifdef POWER_CHIP_PIN
    //gpio_set_output_pushpull(screen_drivers[i].ctrl);
#endif
    gpio_set_output_pushpull(screen_param.reset);
    gpio_set_output_pushpull(screen_param.cs);
    gpio_set_output_pushpull(screen_param.dc);
#ifdef POWER_CHIP_PIN
    //gpio_write_pin(screen_drivers[i].ctrl, 0);
#endif
    gpio_write_pin(screen_param.reset, 0);
    gpio_write_pin(screen_param.cs, 1);
    gpio_write_pin(screen_param.dc, 1);

    lcd_screen = sp_lcd_create(SPI_LCD_DRIVER_TYPE, screen_param);

    if (lcd_screen != NULL) {
        lcd_screen->init(lcd_screen);
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
    lcd_screen->fill(lcd_screen, data);
}

void screen_test(void)
{
}

void screen_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t data_size)
{
    lcd_screen->fill_rect(lcd_screen, x, y, width, height, data, data_size);
}

void screen_fill_rect_async(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t data_size)
{
    lcd_screen->fill_rect_async(lcd_screen, x, y, width, height, data, data_size);
}

bool screen_fill_ready(void)
{
    if (lcd_screen->fill_ready(lcd_screen)) {
        lcd_screen->release(lcd_screen);
        return true;
    }

    return false;
}
