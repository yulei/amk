/**
 * @file spi_lcd.c
 * 
 */

#pragma once

#include "spi_lcd.h"
typedef struct {
    spi_lcd_type_t type;
    spi_lcd_param_t param;
    spi_lcd_driver_t driver;
    uint8_t used;
} spi_lcd_obj_t;

#ifndef SCREEN_NUM
#define SCREEN_NUM  1
#endif

static spi_lcd_obj_t spi_lcd_objs[SCREEN_NUM] = {0};

static bool driver_init(spi_lcd_type_t type, spi_lcd_obj_t *obj)
{
    switch(type) {
    case SPI_LCD_ST7735:
        return true;
    case SPI_LCD_ST7789:
        return true;
    case SPI_LCD_RM67160:
        return true;
    case SPI_LCD_SSD1357:
        return true;
    default:
    }
    return false;
}

spi_lcd_driver_t* sp_lcd_init(spi_lcd_type_t type, spi_lcd_param_t param)
{
    spi_lcd_obj_t *obj = NULL;
    for (int i = 0; i < SCREEN_NUM; i++) {
        if( !spi_lcd_objs[i].used) {
            obj = &spi_lcd_objs[i];
            break;
        }
    }

    if (obj) {
        obj->type = type;
        obj->param = param;
        if (driver_init(type, obj)) {
            obj->used = 1;
            return &obj->driver;
        }
    }

    return NULL;
}

void spi_lcd_uninit(spi_lcd_driver_t *spi_lcd_driver)
{
}
