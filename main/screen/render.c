/**
 * @file render.c
 * 
 */

#include "render.h"
#include "screen_driver.h"
#include "screen.h"
#include "display.h"

screen_driver_t* screen_drivers[SCREEN_DRIVER_NUM];
screen_t* screens[SCREEN_NUM];
display_t* displays[DISPLAY_NUM];

static void init_screen_drivers(void)
{
    for (int i = 0; i < SCREEN_DRIVER_NUM; i++) {
        screen_drivers[i] = screen_driver_create(&screen_driver_params[i]);
        screen_drivers[i]->init(screen_drivers[i]);
    }
}

static void init_screens(void)
{
    for (int i = 0; i < SCREEN_NUM; i++) {
        screens[i] = screen_create(&screen_params[i]);
        screens[i]->init(screens[i], screen_drivers[screen_params[i].driver]);
    }
}

static void init_displays(void)
{
    for (int i = 0; i < DISPLAY_NUM; i++) {
        displays[i] = display_create(&display_params[i]);
        displays[i]->init(displays[i], screens[display_params[i].screen]);
    }
}

void render_init(void)
{
    init_screen_drivers();
    init_screens();
    init_displays();
}

void render_task(void)
{
    for (int i = 0; i < DISPLAY_NUM; i++) {
        displays[i]->task(displays[i]);
    }
}

void render_enable_display(uint8_t display, bool enable)
{
    if (display < DISPLAY_NUM) {
        displays[display]->set_enable(displays[display], enable);
    }
}


//-----------------simple static buffer allocator

static uint32_t render_buffer[RENDER_BUFFER_SIZE/4];
static uint32_t render_buffer_offset = 0;

uint8_t* render_buffer_allocate(uint32_t size)
{
    uint32_t aligned = (size+3) / 4;
    uint8_t *p = (uint8_t*)(&render_buffer[render_buffer_offset]);
    render_buffer_offset += aligned;

    return p;
}