/**
 * lvgl_driver.c
 */

#include "lvgl_driver.h"
#include "ssd1357.h"
#include "lvgl.h"

static ssd1357_t ssd1357_drivers[2];

static void disp_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

void lvgl_driver_init(void)
{
    lv_init();

    ssd1357_init(&ssd1357_drivers[0]);
    ssd1357_init(&ssd1357_drivers[1]);

    // create two buffer on screen size
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    static lv_color_t buf_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);


    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &disp_buf;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

void lvgl_driver_task(void)
{
    lv_task_handler();
}

void lvgl_driver_uninit(void)
{}

void lvgl_driver_ticks(uint32_t ticks)
{
    lv_tick_inc(ticks);
}

void disp_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if (area->x2 <= LV_HOR_RES_MAX/2) {
        uint32_t width = area->x2 - area->x1 + 1;
        uint32_t height = area->y2 - area->y1 + 1;
        ssd1357_fill_rect(&ssd1357_drivers[0],
                          area->x1,
                          area->y1,
                          width,
                          height,
                          color_p,
                          sizeof(lv_color_t) * (width*height));
    } else {
        if (area->x1 > LV_HOR_RES_MAX/2) {
            uint32_t width = area->x2 - area->x1 + 1;
            uint32_t height = area->y2 - area->y1 + 1;
            ssd1357_fill_rect(&ssd1357_drivers[1],
                            area->x1-LV_HOR_RES_MAX/2,
                            area->y1,
                            width,
                            height,
                            color_p,
                            sizeof(lv_color_t) * (width*height));
        } else {
            uint32_t width = LV_HOR_RES_MAX/2 - area->x1 + 1;
            uint32_t height = area->y2 - area->y1 + 1;
            ssd1357_fill_rect(&ssd1357_drivers[0],
                          area->x1,
                          area->y1,
                          width,
                          height,
                          color_p,
                          sizeof(lv_color_t) * (width*height));
            uint32_t width2 = area->x2 - LV_HOR_RES_MAX/2 + 1;
            ssd1357_fill_rect(&ssd1357_drivers[1],
                            area->x1-LV_HOR_RES_MAX/2,
                            area->y1,
                            width2,
                            height,
                            color_p,
                            sizeof(lv_color_t) * (width2*height));
        }
    }
}