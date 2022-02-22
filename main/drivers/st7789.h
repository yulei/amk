
/**
 * @file st7789.h
 */

#pragma once

#include "spi_lcd.h"

void st7789_config(spi_lcd_param_t *param, spi_lcd_driver_t* lcd);

void st7789_init(spi_lcd_driver_t *lcd);
void st7789_fill_rect(spi_lcd_driver_t *lcd, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void st7789_fill_rect_async(spi_lcd_driver_t *lcd, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool st7789_fill_ready(spi_lcd_driver_t *lcd);
void st7789_release(spi_lcd_driver_t *lcd);
void st7789_fill(spi_lcd_driver_t *lcd, const void* data);
void st7789_uninit(spi_lcd_driver_t *lcd);
