/**
 * @file screen_driver.c
 * 
 */

#include "screen_driver.h"
#include "st7735.h"
#include "st7789.h"
#include "rm67160.h"

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
#ifdef SCREEN_DRIVER_ST7735
    case SPI_LCD_ST7735:
        st7735_config(&obj->param, &obj->driver);
        return true;
#endif
#ifdef SCREEN_DRIVER_ST7789
    case SPI_LCD_ST7789:
        st7789_config(&obj->param, &obj->driver);
        return true;
#endif
#ifdef SCREEN_DRIVER_RM67160
    case SPI_LCD_RM67160:
        rm67160_config(&obj->param, &obj->driver);
        return true;
#endif
    default:
        break;
    }
    return false;
}

spi_lcd_driver_t* spi_lcd_create(spi_lcd_type_t type, spi_lcd_param_t param)
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

void spi_lcd_destroy(spi_lcd_driver_t *spi_lcd_driver)
{
}
