/**
 * @file spi_lcd.h
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "amk_gpio.h"

typedef struct {
#ifdef POWER_CHIP_PIN
    pin_t ctrl;
#endif
//#ifdef SCREEN_POWER_PIN
//    pin_t power;
//#endif
    pin_t reset;
    pin_t cs;
    pin_t dc;
} spi_lcd_param_t;

typedef enum {
    SPI_LCD_ST7735,
    SPI_LCD_ST7789,
    SPI_LCD_RM67160,
    SPI_LCD_SSD1357,
    SPI_LCD_MAX,
} spi_lcd_type_t;

typedef struct spi_lcd_driver spi_lcd_driver_t;

struct spi_lcd_driver {
    void (*init)(spi_lcd_driver_t *driver);
    void (*uninit)(spi_lcd_driver_t *driver);
    void (*fill)(spi_lcd_driver_t *driver, const void *data);
    void (*fill_rect)(spi_lcd_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
    void (*fill_rect_async)(spi_lcd_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
    bool (*fill_ready)(spi_lcd_driver_t *driver);
    void (*release)(spi_lcd_driver_t *driver);
    void *data;
};

spi_lcd_driver_t* sp_lcd_create(spi_lcd_type_t type, spi_lcd_param_t param);
void spi_lcd_destroy(spi_lcd_driver_t *spi_lcd_driver);
