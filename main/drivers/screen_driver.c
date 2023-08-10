/**
 * @file screen_driver.c
 * 
 */

#include "render_def.h"
#include "screen_driver.h"

#include "st7735.h"
#include "st7789.h"
#include "rm67160.h"
#include "gc9107.h"
#include "gc9203.h"
#include "ssd1306.h"

typedef struct {
    screen_driver_t driver;
    screen_driver_param_t param;
    uint8_t used;
} screen_driver_obj_t;

static screen_driver_obj_t screen_driver_objs[SCREEN_DRIVER_NUM] = {0};

static bool driver_init(screen_driver_t *driver, screen_driver_param_t *param)
{
    if (param->ctrl != INVALID_PIN) {
        gpio_set_output_pushpull(param->ctrl);
        gpio_write_pin(param->ctrl, 0);
    }

    if (param->power != INVALID_PIN) {
        gpio_set_output_pushpull(param->power);
        gpio_write_pin(param->power, param->power_on);
    }

    if (param->reset != INVALID_PIN) {
        gpio_set_output_pushpull(param->reset);
        gpio_write_pin(param->reset, 0);
    }

    if (param->cs != INVALID_PIN) {
        gpio_set_output_pushpull(param->cs);
        gpio_write_pin(param->cs, 1);
    }

    if (param->dc != INVALID_PIN) {
        gpio_set_output_pushpull(param->dc);
        gpio_write_pin(param->dc, 1);
    }

    switch(param->type) {
#ifdef SCREEN_DRIVER_ST7735
    case SPI_LCD_ST7735:
        st7735_config(driver, param);
        return true;
#endif
#ifdef SCREEN_DRIVER_ST7789
    case SPI_LCD_ST7789:
        st7789_config(driver, param);
        return true;
#endif
#ifdef SCREEN_DRIVER_SSD1306
    case SPI_LCD_SSD1306:
        ssd1306_config(driver, param);
        return true;
#endif
#ifdef SCREEN_DRIVER_RM67160
    case SPI_LCD_RM67160:
        rm67160_config(driver, param);
        return true;
#endif
#ifdef SCREEN_DRIVER_GC9107
    case SPI_LCD_GC9107:
        gc9107_config(driver, param);
        return true;
#endif
#ifdef SCREEN_DRIVER_GC9203
    case SPI_LCD_GC9203:
        gc9203_config(driver, param);
        return true;
#endif
    default:
        break;
    }
    return false;
}

screen_driver_t* screen_driver_create(screen_driver_param_t *param)
{
    screen_driver_obj_t *obj = NULL;
    for (int i = 0; i < SCREEN_DRIVER_NUM; i++) {
        if( !screen_driver_objs[i].used) {
            obj = &screen_driver_objs[i];
            break;
        }
    }

    if (obj) {
        if (driver_init(&obj->driver, param)) {
            obj->used = 1;
            obj->param = *param;
            return &obj->driver;
        }
    }

    return NULL;
}

void screen_driver_destroy(screen_driver_t *driver)
{
    screen_driver_obj_t *obj = (screen_driver_obj_t *)driver->data;
    if (obj->param.power != INVALID_PIN) {
        //gpio_set_output_pushpull(param->power);
        gpio_write_pin(obj->param.power, !obj->param.power_on);
    }
    memset(obj, 0, sizeof(screen_driver_obj_t));
}
