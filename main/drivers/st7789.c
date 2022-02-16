/**
 * @file st7789.c
 */

#include "st7789.h"
#include "spi.h"
#include "wait.h"

#define ST7789_IS_240X135 1

#if ST7789_IS_240X135
#define ST7789_XSTART     40
//#define ST7789_YSTART     52
#define ST7789_YSTART     53
#define ST7789_WIDTH      135
#define ST7789_HEIGHT     240
//#define ST7789_ROTATION (ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB)
#define ST7789_ROTATION (ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_ML | ST7789_MADCTL_RGB)
#endif

#define DELAY               0x80

#define ST7789_MADCTL_MY    0x80
#define ST7789_MADCTL_MX    0x40
#define ST7789_MADCTL_MV    0x20
#define ST7789_MADCTL_ML    0x10
#define ST7789_MADCTL_RGB   0x00
#define ST7789_MADCTL_BGR   0x08
#define ST7789_MADCTL_MH    0x04

// commands
#define ST7789_NOP          0x00
#define ST7789_SWRESET      0x01
#define ST7789_RDDID        0x04
#define ST7789_RDDST        0x09

#define ST7789_SLPIN        0x10
#define ST7789_SLPOUT       0x11
#define ST7789_PTLON        0x12
#define ST7789_NORON        0x13

#define ST7789_INVOFF       0x20
#define ST7789_INVON        0x21
#define ST7789_DISPOFF      0x28
#define ST7789_DISPON       0x29
#define ST7789_CASET        0x2A
#define ST7789_RASET        0x2B
#define ST7789_RAMWR        0x2C
#define ST7789_RAMRD        0x2E

#define ST7789_PTLAR        0x30
#define ST7789_COLMOD       0x3A
#define ST7789_MADCTL       0x36

#define ST7789_FRMCTR1      0xB1
#define ST7789_FRMCTR2      0xB2
#define ST7789_FRMCTR3      0xB3
#define ST7789_INVCTR       0xB4
#define ST7789_DISSET5      0xB6

#define ST7789_PWCTR1       0xC0
#define ST7789_PWCTR2       0xC1
#define ST7789_PWCTR3       0xC2
#define ST7789_PWCTR4       0xC3
#define ST7789_PWCTR5       0xC4
#define ST7789_VMCTR1       0xC5

#define ST7789_RDID1        0xDA
#define ST7789_RDID2        0xDB
#define ST7789_RDID3        0xDC
#define ST7789_RDID4        0xDD

#define ST7789_PWCTR6       0xFC

#define ST7789_GMCTRP1      0xE0
#define ST7789_GMCTRN1      0xE1

// based on Adafruit ST7789 library for Arduino
static const uint8_t init_cmds1[] = { // Init for 7789R, part 1 (red or green tab)
    9,                          //  9 commands in list:
    ST7789_SWRESET, DELAY,      //  1: Software reset, no args, w/delay
    150,                        //     ~150 ms delay
    ST7789_SLPOUT,  DELAY,      //  2: Out of sleep mode, no args, w/delay
    10,                         //      10 ms delay
    ST7789_COLMOD,  1+DELAY,    //  3: Set color mode, 1 arg + delay:
    0x55,                       //     16-bit color
    10,                         //     10 ms delay
    ST7789_MADCTL,  1,          //  4: Mem access ctrl (directions), 1 arg:
    0x08,                       //     Row/col addr, bottom-top refresh
    ST7789_CASET,   4,          //  5: Column addr set, 4 args, no delay:
    0x00,
    0,        //     XSTART = 0
    0,
    240,      //     XEND = 240
    ST7789_RASET,   4,          //  6: Row addr set, 4 args, no delay:
    0x00,
    0,        //     YSTART = 0
    320>>8,
    320&0xFF, //     YEND = 320
    ST7789_INVON,   DELAY,      //  7: hack
    10,
    ST7789_NORON,   DELAY,      //  8: Normal display on, no args, w/delay
    10,                         //     10 ms delay
    ST7789_DISPON,  DELAY,      //  9: Main screen turn on, no args, delay
    10};                        //    10 ms delay

static const uint8_t init_cmds2[] = {            // Init for 7789, rotation
    1,                        // 1 commands in list:
    ST7789_MADCTL, 1, 
    ST7789_ROTATION};

static spi_handle_t spi;

static void st7789_select(st7789_t *driver)
{
    gpio_write_pin(driver->cs, 0);
}

void st7789_unselect(st7789_t *driver)
{
    gpio_write_pin(driver->cs, 1);
}

static void st7789_reset(st7789_t *driver)
{
    gpio_write_pin(driver->reset, 0);
    wait_ms(5);
    gpio_write_pin(driver->reset, 1);
}

static void write_command(st7789_t *driver, uint8_t cmd)
{
    gpio_write_pin(driver->dc, 0);
    spi_send(spi, &cmd, sizeof(cmd));
}

static void write_data(st7789_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->dc, 1);
    spi_send(spi, buff, buff_size);
}

static void write_data_async(st7789_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->dc, 1);
    spi_send_async(spi, buff, buff_size);
}

static void execute_commands(st7789_t *driver, const uint8_t *addr)
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

static void set_address_window(st7789_t *driver, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    write_command(driver, ST7789_CASET);
    uint8_t data[] = { 0x00, x0 + ST7789_XSTART, 0x00, x1 + ST7789_XSTART };
    write_data(driver, data, sizeof(data));

    // row address set
    write_command(driver, ST7789_RASET);
    data[1] = y0 + ST7789_YSTART;
    data[3] = y1 + ST7789_YSTART;
    write_data(driver, data, sizeof(data));

    // write to RAM
    write_command(driver, ST7789_RAMWR);
}

#ifndef ST7789_SPI_ID
#define ST7789_SPI_ID  SPI_INSTANCE_1
#endif

void st7789_init(st7789_t *driver)
{
    spi = spi_init(ST7789_SPI_ID);
    st7789_select(driver);
    st7789_reset(driver);
    execute_commands(driver, init_cmds1);
    execute_commands(driver, init_cmds2);
    set_address_window(driver, 0, 0, ST7789_WIDTH-1, ST7789_HEIGHT-1);
    uint16_t color = 0;
    for (int x = 0; x < ST7789_WIDTH; x++) {
        for (int y = 0; y < ST7789_HEIGHT; y++) {
            write_data(driver, (uint8_t*)&color, sizeof(color));
        }
    }
    st7789_unselect(driver);
}

void st7789_fill_rect(st7789_t *driver, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    st7789_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data(driver, data, size);
    st7789_unselect(driver);
}

void st7789_fill_rect_async(st7789_t *driver, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    st7789_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data_async(driver, data, size);
    //st7789_unselect(driver);
}

bool st7789_fill_ready(st7789_t *driver)
{
    return spi_ready(spi);
}

void st7789_release(st7789_t *driver)
{
    st7789_unselect(driver);
}

void st7789_fill(st7789_t *driver, const void* data)
{
    st7789_fill_rect(driver, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, data, ST7789_WIDTH*ST7789_HEIGHT*2);
}

void st7789_uninit(st7789_t *driver)
{}