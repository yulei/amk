/**
 * screen.c
 */

#include "screen.h"
#include "gpio_pin.h"
#include "ssd1357.h"
#include "lvgl.h"

static ssd1357_t ssd1357_drivers[SCREEN_NUM] = {
    {SCREEN_0_RESET, SCREEN_0_CS, SCREEN_0_DC},
#ifdef SCREEN_1_PRESENT
    {SCREEN_1_RESET, SCREEN_1_CS, SCREEN_1_DC},
#endif
};

static lv_disp_drv_t screen_driver;
static void disp_flush(struct _disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

void screen_init(void)
{
    lv_init();

    for (int i = 0; i < SCREEN_NUM; i++) {
        ssd1357_init(&ssd1357_drivers[i]);
    }

    // create two buffer on screen size
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    static lv_color_t buf_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);


    lv_disp_drv_init(&screen_driver);

    screen_driver.hor_res = LV_HOR_RES_MAX;
    screen_driver.ver_res = LV_VER_RES_MAX;

    screen_driver.flush_cb = disp_flush;
    screen_driver.buffer = &disp_buf;

    /*Finally register the driver*/
    lv_disp_drv_register(&screen_driver);
}

void screen_task(void)
{
    lv_task_handler();
}

void screen_uninit(void)
{}

void screen_ticks(uint32_t ticks)
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
    lv_disp_flush_ready(disp_drv);
}

void screen_test(void)
{
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label1, "Hello world!");
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}