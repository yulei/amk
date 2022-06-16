/**
 * @file gc9107.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "gc9107.h"
#include "spi.h"
#include "wait.h"


#ifndef GC9107_SPI_ID
#define GC9107_SPI_ID  SPI_INSTANCE_1
#endif

#ifndef SCREEN_ROTATION
#define SCREEN_ROTATION 0
#endif

#define GC9107_PIXEL_12BITS     0x01
#define GC9107_PIXEL_16BITS     0x05
#define GC9107_PIXEL_18BITS     0x06

#define GC9107_MADCTL           0x36
#define GC9107_MAD_MY           0x80
#define GC9107_MAD_MX           0x40
#define GC9107_MAD_MV           0x20
#define GC9107_MAD_ML           0x10
#define GC9107_MAD_BGR          0x08
#define GC9107_MAD_MH           0x04
#define GC9107_MAD_RGB          0x00

// command list
#define GC9107_SLEEPOUT         0x11
#define GC9107_INVOFF           0x20
#define GC9107_INVON            0x21
#define GC9107_DISPLAYON        0x29
#define GC9107_COLMOD           0x3A
#define GC9107_SET_GAMMA1       0xF0
#define GC9107_SET_GAMMA2       0xF1
#define GC9107_CASET            0x2A
#define GC9107_RASET            0x2B
#define GC9107_RAMWR            0x2C

#define GC9107_WIDTH  128
#define GC9107_HEIGHT 128

#if (SCREEN_ROTATION == 0)
// normal directory
#define GC9107_XSTART 2
#define GC9107_YSTART 1
#define GC9107_ROTATION (GC9107_MADCTL_BGR)
#endif

#if (SCREEN_ROTATION == 1)
// rotate left
#define GC9107_XSTART 1
#define GC9107_YSTART 2
#define GC9107_ROTATION (GC9107_MADCTL_MX | GC9107_MADCTL_MV | GC9107_MADCTL_BGR)
#endif

#if (SCREEN_ROTATION == 2)
// invert 
#define GC9107_XSTART 2
#define GC9107_YSTART 1
#define GC9107_ROTATION (GC9107_MADCTL_MX | GC9107_MADCTL_MY | GC9107_MADCTL_BGR)
#endif

#if (SCREEN_ROTATION == 3)
// rotate right
#define GC9107_XSTART 1
#define GC9107_YSTART 2
#define GC9107_ROTATION (GC9107_MADCTL_MY | GC9107_MADCTL_MV | GC9107_MADCTL_BGR)
#endif

#define DELAY               0x80

#define GC9107_INT_REG_ENABLE1  0xFE
#define GC9107_INT_REG_ENABLE2  0xFE

#if 1
static const uint8_t init_cmds[] = {
    23,
    GC9107_INT_REG_ENABLE1, 0,
    GC9107_INT_REG_ENABLE2, 0,
    0xB0,1,0xC0,
    0xB2,1,0x27,
    0xB3,1,0x03,
    0xB7,1,0x01,
    0xB6,1,0x19,
    0xAC,1,0xDB,
    0xAB,1,0x0F,
    GC9107_COLMOD,1,GC9107_PIXEL_16BITS,
    0xB4,1,0x04,
    0xA8,1,0x0C,
    0xB8,1,0x08,
    0xEA,1,0x0e,
    0xE8,1,0x2a,
    0xE9,1,0x46,
    0xC6,1,0x25,
    0xC7,1,0x10,
    GC9107_SET_GAMMA1, 14, 0x09,0x32,0x29,0x46,0xc9,0x37,0x33,0x60,0x00,0x14,0x0a,0x16,0x10,0x1F,
    GC9107_SET_GAMMA2, 14, 0x15,0x28,0x5d,0x3f,0xc8,0x16,0x3f,0x60,0x0a,0x06,0x0d,0x1f,0x1c,0x10,
    GC9107_INVON, DELAY, 120,
    GC9107_SLEEPOUT, DELAY, 120,
    GC9107_DISPLAYON, DELAY, 120,
};
#else
static const uint8_t init_cmds[] = {
    23,
    GC9107_INT_REG_ENABLE1, 0,
    GC9107_INT_REG_ENABLE2, 0,
    0xB0,1,0xC0,
    0xB2,1,0x2F,
    0xB3,1,0x03,
    0xB6,1,0x19,
    0xB7,1,0x01,
    0xAC,1,0xCB,
    0xAB,1,0x0E,
    0xB4,1,0x04,
    0xA8,1,0x19,
    0x3A,1,0x05,
    0xB8,1,0x08,
    0xE8,1,0x24,
    0xE9,1,0x48,
    0xEA,1,0x22,
    0xC6,1,0x30,
    0xC7,1,0x18,
    GC9107_SET_GAMMA1,14,0x1F,0x28,0x04,0x3E,0x2A,0x2E,0x20,0x00,0x0C,0x06,0x00,0x1C,0x1F,0x0f,
    GC9107_SET_GAMMA2,14,0x00,0x2D,0x2F,0x3C,0x6F,0x1C,0x0B,0x00,0x00,0x00,0x07,0x0D,0x11,0x0f,
    GC9107_INVON, DELAY, 120,
    GC9107_SLEEPOUT, DELAY, 120,
    GC9107_DISPLAYON, DELAY, 120,
};
#endif

typedef struct {
    screen_driver_param_t param;
    spi_handle_t          spi;
} gc9107_t;

static gc9107_t gc9107_driver;

static void gc9107_select(gc9107_t *driver)
{
    gpio_write_pin(driver->param.cs, 0);
}

static void gc9107_unselect(gc9107_t *driver)
{
    gpio_write_pin(driver->param.cs, 1);
}

static void gc9107_reset(gc9107_t *driver)
{
    gpio_write_pin(driver->param.reset, 0);
    wait_ms(100);
    gpio_write_pin(driver->param.reset, 1);
    wait_ms(100);
}

static void write_command(gc9107_t *driver, uint8_t cmd)
{
    gpio_write_pin(driver->param.dc, 0);
    spi_send(driver->spi, &cmd, sizeof(cmd));
}

static void write_data(gc9107_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->param.dc, 1);
    spi_send(driver->spi, buff, buff_size);
}

static void write_data_async(gc9107_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->param.dc, 1);
    spi_send_async(driver->spi, buff, buff_size);
}

static void execute_commands(gc9107_t *driver, const uint8_t *addr)
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

static void set_address_window(gc9107_t *driver, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    write_command(driver, GC9107_CASET);
    uint8_t data[] = { 0x00, x0 + GC9107_XSTART, 0x00, x1 + GC9107_XSTART};
    write_data(driver, data, sizeof(data));

    // row address set
    write_command(driver, GC9107_RASET);
    data[1] = y0 + GC9107_YSTART;
    data[3] = y1 + GC9107_YSTART;
    write_data(driver, data, sizeof(data));

    // write to RAM
    write_command(driver, GC9107_RAMWR);
}

void gc9107_config(screen_driver_t *driver, screen_driver_param_t *param)
{
    gc9107_driver.param = *param;
    gc9107_driver.spi = spi_init(GC9107_SPI_ID);

    driver->data        = &gc9107_driver;
    driver->type        = gc9107_type;
    driver->init        = gc9107_init;
    driver->uninit      = gc9107_uninit;
    driver->fill        = gc9107_fill_rect;
    driver->fill_async  = gc9107_fill_rect_async;
    driver->ready       = gc9107_fill_ready;
    driver->release     = gc9107_release;
}

void gc9107_init(screen_driver_t *lcd)
{
    gc9107_t *driver = (gc9107_t*)lcd->data;
    gc9107_select(driver);
    gc9107_reset(driver);

    execute_commands(driver, init_cmds);

    set_address_window(driver, 0, 0, GC9107_WIDTH-1, GC9107_HEIGHT-1);
    uint16_t color = 0xFFFF;
    //uint16_t color = 0x0000;
    for (int x = 0; x < GC9107_WIDTH; x++) {
        for (int y = 0; y < GC9107_HEIGHT; y++) {
            write_data(driver, (uint8_t*)&color, sizeof(color));
        }
    }
    gc9107_unselect(driver);
}

void gc9107_fill_rect(screen_driver_t *lcd, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    gc9107_t *driver = (gc9107_t*)lcd->data;
    gc9107_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data(driver, data, size);
    gc9107_unselect(driver);
}

void gc9107_fill_rect_async(screen_driver_t *lcd, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    gc9107_t *driver = (gc9107_t*)lcd->data;
    gc9107_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data_async(driver, data, size);
    //st7735_unselect(driver);
}

bool gc9107_fill_ready(screen_driver_t *lcd)
{
    gc9107_t *driver = (gc9107_t*)lcd->data;
    return spi_ready(driver->spi);
}

void gc9107_release(screen_driver_t *lcd)
{
    gc9107_t *driver = (gc9107_t*)lcd->data;
    gc9107_unselect(driver);
}

void gc9107_fill(screen_driver_t *lcd, const void* data)
{
    gc9107_fill_rect(lcd, 0, 0, GC9107_WIDTH, GC9107_HEIGHT, data, GC9107_WIDTH*GC9107_HEIGHT*2);
}

void gc9107_uninit(screen_driver_t *lcd)
{}

uint8_t gc9107_type(screen_driver_t *lcd)
{
    return SPI_LCD_GC9107;
}
