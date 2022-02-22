
/**
 * @file st7735.h
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "spi_lcd.h"

void st7735_config(spi_lcd_param_t *param, spi_lcd_driver_t* lcd);

void st7735_init(spi_lcd_driver_t *driver);
void st7735_fill_rect(spi_lcd_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void st7735_fill_rect_async(spi_lcd_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool st7735_fill_ready(spi_lcd_driver_t *driver);
void st7735_release(spi_lcd_driver_t *driver);
void st7735_fill(spi_lcd_driver_t *driver, const void* data);
void st7735_uninit(spi_lcd_driver_t *driver);

