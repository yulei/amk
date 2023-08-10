/**
 * @file gc9203.c
 * @author astro
 * 
 * @copyright Copyright (C) 2023
*/

#include "gc9203.h"
#include "spi.h"
#include "wait.h"

#ifndef SCREEN_ROTATION
#define SCREEN_ROTATION 0
#endif

// normal directory
#if (SCREEN_ROTATION == 0)
#define GC9203_WIDTH  128
#define GC9203_HEIGHT 220
#endif

#if (SCREEN_ROTATION == 1)
#define GC9203_WIDTH  128
#define GC9203_HEIGHT 220
#endif

#if (SCREEN_ROTATION == 2)
#define GC9203_WIDTH  220
#define GC9203_HEIGHT 128
#endif

#if (SCREEN_ROTATION == 3)
#define GC9203_WIDTH  220
#define GC9203_HEIGHT 128
#endif

#define DELAY               0x80

typedef struct {
    screen_driver_param_t param;
    spi_handle_t          spi;
} gc9203_t;

static gc9203_t gc9203_driver;

static void gc9203_select(gc9203_t *driver)
{
    gpio_write_pin(driver->param.cs, 0);
    //wait_ms(1);
}

static void gc9203_unselect(gc9203_t *driver)
{
    gpio_write_pin(driver->param.cs, 1);
}

static void gc9203_reset(gc9203_t *driver)
{
    gpio_write_pin(driver->param.reset, 0);
    wait_ms(100);
    gpio_write_pin(driver->param.reset, 1);
}


static void write_command(gc9203_t *driver, uint8_t cmd)
{
    gpio_write_pin(driver->param.dc, 0);
    spi_send(driver->spi, &cmd, sizeof(cmd));
}


static void write_data(gc9203_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->param.dc, 1);
    spi_send(driver->spi, buff, buff_size);
}

static void write_data_async(gc9203_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->param.dc, 1);
    spi_send_async(driver->spi, buff, buff_size);
}

static void write_16(gc9203_t *driver, bool ctrl, uint16_t data)
{
    gpio_write_pin(driver->param.dc, ctrl ? 0 : 1);
    uint8_t buffer[2];
    buffer[0] = data >> 8;
    buffer[1] = data & 0xFF;
    spi_send(driver->spi, buffer, 2);
}

static void gc9203_write_cmd(uint8_t cmd)
{
    write_command(&gc9203_driver, cmd);
}


static void gc9203_write_data(uint8_t data)
{
    write_data(&gc9203_driver, &data, sizeof(data));
}

static void init_commands(gc9203_t *driver)
{
    #if 0
    write_16(driver, true, 0x00ff); 
    write_16(driver, false, 0x5aa5);

    write_16(driver, true, 0x00f6);
    write_16(driver, false, 0x0112);

    write_16(driver, true, 0x00ef);
    write_16(driver, false, 0x1052);  ////1052

    write_16(driver, true, 0x00a5);
    write_16(driver, false, 0x0780);

    write_16(driver, true, 0x0002);
    write_16(driver, false, 0x0100);

    write_16(driver, true, 0x0003);
	if(SCREEN_ROTATION==0) {
        write_16(driver, false, 0x1000);
    } else if(SCREEN_ROTATION==1) {
        write_16(driver, false, 0x1030);
    } else if(SCREEN_ROTATION==2) {
        write_16(driver, false, 0x1028);
    } else if(SCREEN_ROTATION==3) {
        write_16(driver, false, 0x1018);
    }
    write_16(driver, true, 0x00f6);
    write_16(driver, false, 0x0112);

    write_16(driver, true, 0x0011);
    write_16(driver, false, 0x1000);

    write_16(driver, true, 0x00EB);
    write_16(driver, false, 0x0D1F);//1630  //vreg1b[13:8]_vreg1a_[5:0]     122B   3319  0D1F

    write_16(driver, true, 0x00EC);
    write_16(driver, false, 0x0528);//160a  //vreg2b[13:8]_vreg2a_[5:0]     1606   2334  0528

    write_16(driver, true, 0x0050);
    write_16(driver, false, 0xf380);//H:V0(7-4) V13(3-0)   L:V63(3-0)

    write_16(driver, true, 0x0051);
    write_16(driver, false, 0x110c);//H:V61(5-0)   L:V62(5-0)

    write_16(driver, true, 0x0052);
    write_16(driver, false, 0x0909);//H:V57(4-0)  L:V59(4-0)

    write_16(driver, true, 0x0053);
    write_16(driver, false, 0x3706);//H:V43(6-0)  L:V50(3-0)

    write_16(driver, true, 0x0054);
    write_16(driver, false, 0x4e1f);//H:V20(6-0)   L:V27(5-3) V36(2-0)  4e27

    write_16(driver, true, 0x0055);
    write_16(driver, false, 0x1212);//H:V4(4-0)   L:V6(4-0)  

    write_16(driver, true, 0x0056);
    write_16(driver, false, 0x3736);//H:V1(5-0)   L:V2(5-0)

    write_16(driver, true, 0x0057);
    write_16(driver, false, 0xfc80);//H:V0(7-4) V13(3-0)   L:V63(3-0)

    write_16(driver, true, 0x0058);
    write_16(driver, false, 0x110c);//H:V61(5-0)   L:V62(5-0)

    write_16(driver, true, 0x0059);
    write_16(driver, false, 0x0909);//H:V57(4-0)  L:V59(4-0)

    write_16(driver, true, 0x005A);
    write_16(driver, false, 0x3706);//H:V43(6-0)  L:V50(3-0)

    write_16(driver, true, 0x005B);
    write_16(driver, false, 0x4e19);//H:V20(6-0)   L:V27(5-3) V36(2-0)  4e21

    write_16(driver, true, 0x005C);
    write_16(driver, false, 0x1212);//H:V4(4-0)   L:V6(4-0)

    write_16(driver, true, 0x005D);
    write_16(driver, false, 0x3736);//H:V1(5-0)   L:V2(5-0)

    write_16(driver, true, 0x0007);
    write_16(driver, false, 0x1013);

    write_16(driver, true, 0x0001);
    write_16(driver, false, 0x011c);

    write_16(driver, true, 0x00fe);
	wait_ms(60);

    write_16(driver, true, 0x0022);

#else
    gc9203_write_cmd(0xff);
    gc9203_write_data(0x5a);
    gc9203_write_data(0xa5);

    gc9203_write_cmd(0xf6);
    gc9203_write_data(0x01);
    gc9203_write_data(0x12);

    gc9203_write_cmd(0xef);
    gc9203_write_data(0x10);
    gc9203_write_data(0x52);

    gc9203_write_cmd(0xa5);
    gc9203_write_data(0x07);
    gc9203_write_data(0x80);

    gc9203_write_cmd(0x02);
    gc9203_write_data(0x01);
    gc9203_write_data(0x00);

    gc9203_write_cmd(0x03);
    gc9203_write_data(0x10);
    gc9203_write_data(0x10);

    gc9203_write_cmd(0x01);
    gc9203_write_data(0x03);
    gc9203_write_data(0x1c);

    gc9203_write_cmd(0xf6);
    gc9203_write_data(0x01);
    gc9203_write_data(0x12);

    gc9203_write_cmd(0x11);
    gc9203_write_data(0x10);
    gc9203_write_data(0x00);

    gc9203_write_cmd(0xEB);
    gc9203_write_data(0x0d);
    gc9203_write_data(0x1f);

    gc9203_write_cmd(0xEC);
    gc9203_write_data(0x05);
    gc9203_write_data(0x28);

    gc9203_write_cmd(0x50);
    gc9203_write_data(0xf3);
    gc9203_write_data(0x80);

    gc9203_write_cmd(0x51);
    gc9203_write_data(0x11);
    gc9203_write_data(0x0c);
    
    gc9203_write_cmd(0x52);
    gc9203_write_data(0x09);
    gc9203_write_data(0x09);

    gc9203_write_cmd(0x53);
    gc9203_write_data(0x37);
    gc9203_write_data(0x06);

    gc9203_write_cmd(0x54);
    gc9203_write_data(0x4e);
    gc9203_write_data(0x1f);

    gc9203_write_cmd(0x55);
    gc9203_write_data(0x12);
    gc9203_write_data(0x12);

    gc9203_write_cmd(0x56);
    gc9203_write_data(0x37);
    gc9203_write_data(0x36);

    gc9203_write_cmd(0x57);
    gc9203_write_data(0xfc);
    gc9203_write_data(0x80);

    gc9203_write_cmd(0x58);
    gc9203_write_data(0x11);
    gc9203_write_data(0x0c);

    gc9203_write_cmd(0x59);
    gc9203_write_data(0x09);
    gc9203_write_data(0x09);

    gc9203_write_cmd(0x5A);
    gc9203_write_data(0x37);
    gc9203_write_data(0x06);
    gc9203_write_cmd(0x5B);
    gc9203_write_data(0x4e);
    gc9203_write_data(0x19);
    gc9203_write_cmd(0x5C);
    gc9203_write_data(0x12);
    gc9203_write_data(0x12);
    gc9203_write_cmd(0x5D);
    gc9203_write_data(0x37);
    gc9203_write_data(0x36);

    gc9203_write_cmd(0x07);
    gc9203_write_data(0x10);
    gc9203_write_data(0x13);
    gc9203_write_cmd(0xfe);
#endif
}

static void set_address_window(gc9203_t *driver, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	if(SCREEN_ROTATION ==0 || SCREEN_ROTATION == 1) {
        gc9203_write_cmd(0x37);
        write_16(driver, false, x0+24);
        gc9203_write_cmd(0x36);
        write_16(driver, false, x1+24);

        gc9203_write_cmd(0x39);
        write_16(driver, false, y0);
        gc9203_write_cmd(0x38);
        write_16(driver, false, y1);
	} else {
        gc9203_write_cmd(0x39);
        write_16(driver, false, x0);
        gc9203_write_cmd(0x38);
        write_16(driver, false, x1);

        gc9203_write_cmd(0x37);
        write_16(driver, false, y0+24);
        gc9203_write_cmd(0x36);
        write_16(driver, false, y1+24);
	}

    gc9203_write_cmd(0x22);
}

void gc9203_config(screen_driver_t *driver, screen_driver_param_t *param)
{
    gc9203_driver.param = *param;
    gc9203_driver.spi = spi_init(GC9203_SPI_ID);

    driver->data        = &gc9203_driver;
    driver->type        = gc9203_type;
    driver->init        = gc9203_init;
    driver->uninit      = gc9203_uninit;
    driver->fill        = gc9203_fill_rect;
    driver->fill_async  = gc9203_fill_rect_async;
    driver->ready       = gc9203_fill_ready;
    driver->release     = gc9203_release;
}

void gc9203_init(screen_driver_t *lcd)
{
    gc9203_t *driver = (gc9203_t*)lcd->data;

    gc9203_select(driver);
    gc9203_reset(driver);

    init_commands(driver);

    set_address_window(driver, 0, 0, GC9203_WIDTH-1, GC9203_HEIGHT-1);
    uint16_t color = 0xFFFF;
    //uint16_t color = 0x0000;
    for (int x = 0; x < GC9203_WIDTH; x++) {
        for (int y = 0; y < GC9203_HEIGHT; y++) {
            write_data(driver, (uint8_t*)&color, sizeof(color));
        }
    }
//    execute_commands(driver, init_cmds4);
    gc9203_unselect(driver);
}

void gc9203_fill_rect(screen_driver_t *lcd, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    gc9203_t *driver = (gc9203_t*)lcd->data;
    gc9203_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data(driver, data, size);
    gc9203_unselect(driver);
}

void gc9203_fill_rect_async(screen_driver_t *lcd, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    gc9203_t *driver = (gc9203_t*)lcd->data;
    gc9203_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data_async(driver, data, size);
    //st7735_unselect(driver);
}

bool gc9203_fill_ready(screen_driver_t *lcd)
{
    gc9203_t *driver = (gc9203_t*)lcd->data;
    return spi_ready(driver->spi);
}

void gc9203_release(screen_driver_t *lcd)
{
    gc9203_t *driver = (gc9203_t*)lcd->data;
    gc9203_unselect(driver);
}

void gc9203_fill(screen_driver_t *lcd, const void* data)
{
    gc9203_fill_rect(lcd, 0, 0, GC9203_WIDTH, GC9203_HEIGHT, data, GC9203_WIDTH*GC9203_HEIGHT*2);
}

void gc9203_uninit(screen_driver_t *lcd)
{}

uint8_t gc9203_type(screen_driver_t *lcd)
{
    return SPI_LCD_GC9203;
}