/**
 * @file ssd1306.c
 * @author astro
 * @brief 
 * 
 * @copyright Copyright (c) 2022, astro,  All rights reserved.
 */

#include "ssd1306.h"
#include "spi.h"
#include "wait.h"

#ifndef SCREEN_ROTATION
#define SCREEN_ROTATION 0
#endif

// normal directory
#if (SCREEN_ROTATION == 0)
#define SSD1306_XSTART 0
#define SSD1306_YSTART 0
#define SSD1306_WIDTH  72
#define SSD1306_HEIGHT 40 
#define SSD1306_ROTATION 
#endif

#define DELAY               0x80

// SSD1306 init sequence
static const uint8_t
    init_cmds1[] = {  // Init for SSD1306
    28,               // 29 commands in list:
    0xAE, 0,          // display off
    0xD5, 0,          // set osc division
    0xF0, 0,
    0xA8, 0,          // multiplex ratio
    0x27, 0,          // duty = 1/40
    0xD3, 0,          // set display offset
    0x00, 0,
    0x40, 0,          // set display start Line
    0x8d, 0,          // set charge pump enable
    0x14, 0,
    0x20, 0,          // set page address mode
    0x02, 0,
    //0xA1, 0,          // set segment remap
    0xA0, 0,          // set segment remap
    //0xC8, 0,          // COM scan direction
    0xC0, 0,          // COM scan direction
    0xDA, 0,          // set COM pins
    0x12, 0,
    0xAD, 0,          // internal IREF setting
    0x30, 0,
    0x81, 0,          // contract control
    0xFF, 0,          // 128
    0xD9, 0,          // set pre-charge period
    0x22, 0,
    0xdb, 0,          // set vcomh
    0x20, 0,
    0xA4, 0,          // set entire display ON/OFF
    0xA6, 0,          // normal / reverse
    0x0C, 0,          // set lower column address
    0x11, 0,          // set higher column address	
  };

typedef struct {
    screen_driver_param_t param;
    spi_handle_t          spi;
} ssd1306_t;

static ssd1306_t ssd1306_driver;

static void ssd1306_select(ssd1306_t *driver)
{
    gpio_write_pin(driver->param.cs, 0);
}

static void ssd1306_unselect(ssd1306_t *driver)
{
    gpio_write_pin(driver->param.cs, 1);
}

static void ssd1306_reset(ssd1306_t *driver)
{
    gpio_write_pin(driver->param.reset, 0);
    wait_ms(200);
    gpio_write_pin(driver->param.reset, 1);
}

static void write_command(ssd1306_t *driver, uint8_t cmd)
{
    gpio_write_pin(driver->param.dc, 0);
    spi_send(driver->spi, &cmd, sizeof(cmd));
}

static void write_data(ssd1306_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->param.dc, 1);
    spi_send(driver->spi, buff, buff_size);
}

//static void write_data_async(ssd1306_t *driver, const uint8_t* buff, size_t buff_size)
//{
//    gpio_write_pin(driver->param.dc, 1);
//    spi_send_async(driver->spi, buff, buff_size);
//}

//static void set_address_window(ssd1306_t *driver, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
//{
    // column address set

    // row address set

    // write to RAM
//}

static void execute_commands(ssd1306_t *driver, const uint8_t *addr)
{
    uint8_t commands, args;
    uint16_t ms;

    commands = *addr++;
    while(commands--) {
        uint8_t cmd = *addr++;
        write_command(driver, cmd);

        args = *addr++;
        // If high bit set, delay follows args
        ms = args & DELAY;
        args &= ~DELAY;
        if(args) {
            write_data(driver, (uint8_t*)addr, args);
            addr += args;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            wait_ms(ms);
        }
    }
}

void ssd1306_config(screen_driver_t *driver, screen_driver_param_t *param)
{
    ssd1306_driver.param = *param;
    ssd1306_driver.spi = spi_init(SSD1306_SPI_ID);

    driver->data        = &ssd1306_driver;
    driver->type        = ssd1306_type;
    driver->init        = ssd1306_init;
    driver->uninit      = ssd1306_uninit;
    driver->fill        = ssd1306_fill_rect;
    driver->fill_async  = ssd1306_fill_rect_async;
    driver->ready       = ssd1306_fill_ready;
    driver->release     = ssd1306_release;
}

void ssd1306_init(screen_driver_t *lcd)
{
    ssd1306_t *driver = (ssd1306_t*)lcd->data;
    ssd1306_reset(driver);

    ssd1306_select(driver);

    execute_commands(driver, init_cmds1);

    //uint8_t color = 0xFF;
    uint8_t color = 0x00;
    for (int y = 0; y < SSD1306_HEIGHT/8; y++) {
        write_command(driver, 0xb0+y);
        write_command(driver, 0x0c);
        write_command(driver, 0x11);
        for (int x = 0; x < SSD1306_WIDTH; x++) {
            write_data(driver, &color, sizeof(color));
        }
    }
    #if 0
        write_command(driver, 0xb0+3);
        write_command(driver, 0x0c);
        write_command(driver, 0x11);
        color = 0xFF;
        for (int x = 0; x < SSD1306_WIDTH; x++) {
            write_data(driver, &color, sizeof(color));
        }
    #endif

    //write_command(driver, 0x8D); // display ON
    //write_command(driver, 0x14); // display ON
    write_command(driver, 0xAF); // display ON
    ssd1306_unselect(driver);
}

void ssd1306_fill_rect(screen_driver_t *lcd, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    ssd1306_t *driver = (ssd1306_t*)lcd->data;
    ssd1306_select(driver);
    uint8_t *cur = (uint8_t*)data;

    for (int y = 0; y < SSD1306_HEIGHT/8; y++) {
        write_command(driver, 0xb0+y);
        write_command(driver, 0x0c);
        write_command(driver, 0x11);
        //for (int x = 0; x < SSD1306_WIDTH; x++) {
        //    write_data(driver, cur++, sizeof(*cur));
        //}
        write_data(driver, cur, SSD1306_WIDTH);
        cur += SSD1306_WIDTH;
    }

    ssd1306_unselect(driver);
}

void ssd1306_fill_rect_async(screen_driver_t *lcd, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    ssd1306_fill_rect(lcd, x, y, w, h, data, size);
}

bool ssd1306_fill_ready(screen_driver_t *lcd)
{
    ssd1306_t *driver = (ssd1306_t*)lcd->data;
    return spi_ready(driver->spi);
}

void ssd1306_release(screen_driver_t *lcd)
{
    ssd1306_t *driver = (ssd1306_t*)lcd->data;
    ssd1306_unselect(driver);
}

void ssd1306_fill(screen_driver_t *lcd, const void* data)
{
    //st7735_fill_rect(lcd, 0, 0, ST7735_WIDTH, ST7735_HEIGHT, data, ST7735_WIDTH*ST7735_HEIGHT*2);
}

void ssd1306_uninit(screen_driver_t *lcd)
{}

uint8_t ssd1306_type(screen_driver_t *driver)
{
    return SPI_LCD_SSD1306;
}