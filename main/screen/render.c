/**
 * @file render.c
 * 
 */

#include "render.h"
#include "usb_common.h"
#include "screen_driver.h"
#include "screen.h"
#include "display.h"
#include "wait.h"

screen_driver_t* screen_drivers[SCREEN_DRIVER_NUM];
screen_t* screens[SCREEN_NUM];
display_t* displays[DISPLAY_NUM];

static bool render_inited = false;

static void init_screen_drivers(void)
{
    for (int i = 0; i < SCREEN_DRIVER_NUM; i++) {
        screen_drivers[i] = screen_driver_create(&screen_driver_params[i]);
        screen_drivers[i]->init(screen_drivers[i]);
    }
}

static void uninit_screen_drivers(void)
{
    for (int i = 0; i < SCREEN_DRIVER_NUM; i++) {
        screen_drivers[i]->uninit(screen_drivers[i]);
        screen_driver_destroy(screen_drivers[i]);
    }
}

static void init_screens(void)
{
    for (int i = 0; i < SCREEN_NUM; i++) {
        screens[i] = screen_create(&screen_params[i]);
        screens[i]->init(screens[i], screen_drivers[screen_params[i].driver]);
    }
}

static void uninit_screens(void)
{
    for (int i = 0; i < SCREEN_NUM; i++) {
        screens[i]->uninit(screens[i]);
        screen_destroy(screens[i]);
    }
}

static void init_displays(void)
{
    for (int i = 0; i < DISPLAY_NUM; i++) {
        displays[i] = display_create(&display_params[i]);
        displays[i]->init(displays[i], screens[display_params[i].screen]);
    }
}

static void uninit_displays(void)
{
    for (int i = 0; i < DISPLAY_NUM; i++) {
        displays[i]->uninit(displays[i]);
        display_destroy(displays[i]);
    }
}

void render_init(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    render_buffer_init();

    init_screen_drivers();
    init_screens();
    init_displays();

    render_inited = true;
}

void render_uninit(void)
{
    if (!render_inited) return;

    uninit_displays();
    uninit_screens();
    uninit_screen_drivers();

    render_buffer_init();

    render_inited = false;
}

void render_task(void)
{
    if (usb_setting & USB_MSC_BIT) return;

    if (!render_inited) return;

    for (int i = 0; i < DISPLAY_NUM; i++) {
        displays[i]->task(displays[i]);
    }
}

void render_toggle_display(uint8_t display)
{
    if (display < DISPLAY_NUM) {
        bool enabled = displays[display]->is_enabled(displays[display]);
        render_enable_display(display, !enabled);
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

void render_buffer_init(void)
{
    memset(render_buffer, 0, sizeof(render_buffer));
    render_buffer_offset = 0;
}

uint8_t* render_buffer_allocate(uint32_t size)
{
    uint32_t aligned = (size+3) / 4;
    uint8_t *p = (uint8_t*)(&render_buffer[render_buffer_offset]);
    render_buffer_offset += aligned;

    return p;
}

//------------------matrix change process
void hook_matrix_change_screen(keyevent_t event)
{
    for (int i = 0; i < DISPLAY_NUM; i++) {
        if (displays[i]->matrix_change) {
            displays[i]->matrix_change(displays[i], event);
        }
    }
}
