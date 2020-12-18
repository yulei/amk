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
        gpio_write_pin(ssd1357_drivers[i].reset, 0);
        gpio_write_pin(ssd1357_drivers[i].cs, 1);
        gpio_write_pin(ssd1357_drivers[i].dc, 1);
        gpio_set_output_pushpull(ssd1357_drivers[i].reset);
        gpio_set_output_pushpull(ssd1357_drivers[i].cs);
        gpio_set_output_pushpull(ssd1357_drivers[i].dc);
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

    // register the driver
    lv_disp_drv_register(&screen_driver);

    // test
    //screen_test();
}

__attribute__((weak))
void screen_task_kb(void) {}

void screen_task(void)
{
    lv_task_handler();
    screen_task_kb();
}

void screen_uninit(void)
{}

void screen_ticks(uint32_t ticks)
{
    lv_tick_inc(ticks);
}

void disp_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint32_t width  = area->x2-area->x1+1;
    uint32_t height = area->y2-area->y1+1;
    ssd1357_fill_rect(&ssd1357_drivers[0],
                          area->x1,
                          area->y1,
                          width,
                          height,
                          color_p,
                          sizeof(lv_color_t) * (width*height));
                          

    lv_disp_flush_ready(disp_drv);
}

void screen_fill(const void* data)
{
    ssd1357_fill(&ssd1357_drivers[0], data);
}

void screen_test(void)
{
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label1, "Hello \nbabyfish!");
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}

void screen_fill_rect(void* data)
{
    ssd1357_fill_rect(&ssd1357_drivers[0],
                        0,
                        0,
                        64,
                        64,
                        data,
                        sizeof(lv_color_t) * (64*64));
}