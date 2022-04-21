/**
 * @file screen.c
 * 
 */

#include <string.h>
#include "screen.h"

typedef struct {
    screen_driver_t*    driver;
    screen_param_t      param;
    uint8_t*            buffer;
    uint32_t            buffer_size;
    screen_t            screen;
    uint8_t             used;
} screen_obj_t;

static screen_obj_t screen_objs[SCREEN_NUM];

static bool screen_init(screen_t *screen, screen_driver_t *driver)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->driver = driver;
    obj->buffer_size = obj->param.width*obj->param.height*2;
    obj->buffer = render_buffer_allocate(obj->buffer_size);
    if (!obj->buffer) {
        return false;
    }

    return true;
}

static void screen_uninit(screen_t *screen)
{}

static void screen_fill_rect(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t size)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->driver->fill(obj->driver, obj->param.x+x, obj->param.y+y, width, height, data, size);
}

static void screen_fill_rect_async(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t size)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->driver->fill_async(obj->driver, obj->param.x + x, obj->param.y + y, width, height, data, size);
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

static void plot(uint32_t x, uint32_t y, uint8_t *buffer)
{
	uint32_t m = y / 8;
	uint32_t n = y % 8;
    buffer[72*m+x]  |= (1<<n);
}

static void screen_draw_rect(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    for (uint32_t i = 0; i < width; i++) {
        for (uint32_t j = 0; j < height; j++) {
            plot(x+i, y+j, obj->buffer);
        }
    }
}

static void screen_clear(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    memset(obj->buffer, 0, obj->buffer_size);
}

static void screen_refresh(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;
    obj->driver->fill(obj->driver, 0, 0, obj->param.width, obj->param.height, obj->buffer, obj->buffer_size);
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
        obj->screen.draw_rect = screen_draw_rect;
        obj->screen.clear = screen_clear;
        obj->screen.refresh = screen_refresh;

        return &obj->screen;
    }

    return NULL;
}

void screen_destroy(screen_t *screen)
{
}

