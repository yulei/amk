/**
 * @file screen.c
 * 
 */

#include "screen.h"
typedef struct {
    screen_driver_t*    driver;
    screen_param_t      param;
    screen_t            screen;
    uint8_t             used;
} screen_obj_t;

static screen_obj_t screen_objs[SCREEN_NUM];

static bool screen_init(screen_t *screen, screen_driver_t *driver)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->driver = driver;

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

static bool screen_test(screen_t *screen)
{
    screen_obj_t *obj = (screen_obj_t*)screen->data;

    obj->screen.test(screen);

    return false;
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
        obj->screen.ready = screen_ready;
        obj->screen.test = screen_test;

        return &obj->screen;
    }

    return NULL;
}

void screen_destroy(screen_t *screen)
{
}
