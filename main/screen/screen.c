/**
 * @file screen.c
 * 
 */

#include <string.h>
#include "render.h"
#include "screen.h"
#include "amk_printf.h"

typedef struct {
    screen_driver_t*    driver;
    screen_param_t      param;
    uint8_t*            buffer;
    uint32_t            buffer_size;
    screen_t            screen;
    uint8_t             used;
} screen_obj_t;

static screen_obj_t screen_objs[SCREEN_NUM];
#define MONO_BUFFER_SIZE    (72*40)
static uint8_t mono_buffer[MONO_BUFFER_SIZE];

static bool screen_init(screen_t *screen, screen_driver_t *driver)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->driver = driver;
    obj->buffer_size = obj->param.width*obj->param.height*2;
    obj->buffer = render_buffer_allocate(obj->buffer_size);

    if (!obj->buffer) {
        return false;
    }

    memset(obj->buffer, 0, obj->buffer_size);

    return true;
}

static void screen_uninit(screen_t *screen)
{
}


static void screen_plot(screen_t *screen, uint32_t x, uint32_t y, uint16_t color)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    if (x > obj->param.width || y > obj->param.height) {
        amk_printf("out of scope: x:%d, y:%d\n", x, y);
        return;
    }

    if (obj->driver->type(obj->driver) == SPI_LCD_SSD1306) {
        uint32_t m = y / 8;
        uint32_t n = y % 8;
        mono_buffer[obj->param.width*m + x] |= (1<<n);
    } else {
        uint16_t *p = (uint16_t*)obj->buffer;
        p[obj->param.width*y + x] = color;
    }
}

static void screen_clear(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    if (obj->driver->type(obj->driver) == SPI_LCD_SSD1306) {
        memset(mono_buffer, 0, MONO_BUFFER_SIZE);
    } else {
        memset(obj->buffer, 0, obj->buffer_size);
    }
}

static void screen_fill_rect(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t size)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    if (screen_driver_params[obj->param.driver].type == SPI_LCD_SSD1306) {
        screen_clear(screen);
        uint16_t *p = (uint16_t*)data;
        for (int j = y; j < y+height; j++) {
            for (int i = x; i < x+width; i++) {
                uint16_t c = *p;
                uint16_t r = (c) >> 11;
                uint16_t g = ((c) >> 5) & 0x3F;
                uint16_t b =  c & 0x1F;
                if( (r>25) || (g>50) || (b>25)) {
                    screen_plot(screen, i, j, 0xFFFF);
                }
                p++;
            }
        }

        obj->driver->fill(obj->driver, 0, 0, obj->param.width, obj->param.height, mono_buffer, MONO_BUFFER_SIZE);
    } else {
        obj->driver->fill(obj->driver, obj->param.x+x, obj->param.y+y, width, height, data, size);
    }
}

static void screen_fill_rect_async(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t size)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    if (screen_driver_params[obj->param.driver].type == SPI_LCD_SSD1306) {
        screen_fill_rect(screen, x, y, width, height, data, size);
    } else {
        obj->driver->fill_async(obj->driver, obj->param.x + x, obj->param.y + y, width, height, data, size);
    }
}

static bool screen_ready(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    if (obj->driver->ready(obj->driver)) {
        obj->driver->release(obj->driver);
        return true;
    }

    return false;
}

static uint8_t* screen_get_buffer(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    return obj->buffer;
}

static bool screen_test(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->screen.test(screen);

    return false;
}


static void screen_refresh(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    if (screen_driver_params[obj->param.driver].type == SPI_LCD_SSD1306) {
        obj->driver->fill(obj->driver, 0, 0, obj->param.width, obj->param.height, mono_buffer, MONO_BUFFER_SIZE);
    } else {
        obj->driver->fill(obj->driver, obj->param.x, obj->param.y, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
    }
}

screen_t* screen_create(screen_param_t *param)
{
    screen_obj_t *obj = NULL;
    for (int i = 0; i < SCREEN_NUM; i++) {
        if( !screen_objs[i].used) {
            obj = &screen_objs[i];
            break;
        }
    }

    if (obj) {
        obj->used = 1;
        obj->param = *param;


        obj->screen.data = obj;
        obj->screen.init = screen_init;
        obj->screen.uninit = screen_uninit;
        obj->screen.fill_rect = screen_fill_rect;
        obj->screen.fill_rect_async = screen_fill_rect_async;
        obj->screen.get_buffer = screen_get_buffer;
        obj->screen.ready = screen_ready;
        obj->screen.test = screen_test;
        obj->screen.plot = screen_plot;
        obj->screen.clear = screen_clear;
        obj->screen.refresh = screen_refresh;

        return &obj->screen;
    }

    return NULL;
}

void screen_destroy(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    memset(obj, 0, sizeof(screen_obj_t));
}
