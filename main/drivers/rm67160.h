
/**
 * @file rm67160.h
 */

#pragma once

#include "spi_lcd.h"

void rm67160_config(spi_lcd_param_t *param, spi_lcd_driver_t* lcd);

void rm67160_init(spi_lcd_driver_t *lcd);
void rm67160_fill_rect(spi_lcd_driver_t *lcd, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void rm67160_fill_rect_async(spi_lcd_driver_t *lcd, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool rm67160_fill_ready(spi_lcd_driver_t *lcd);
void rm67160_release(spi_lcd_driver_t *lcd);
void rm67160_fill(spi_lcd_driver_t *lcd, const void* data);
void rm67160_uninit(spi_lcd_driver_t *lcd);
