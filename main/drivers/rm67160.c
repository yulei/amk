/**
 * @file rm67160.c
 */

#include "rm67160.h"
#include "spi.h"
#include "wait.h"

#define RM_DELAY      0x80

#ifdef SCREEN_DRIVER_RM67160
#define RM_X_START    0
#define RM_Y_START    0
#define RM_WIDTH      180
#define RM_HEIGHT     120
#else
#define RM_X_START    4
#define RM_Y_START    0
#define RM_WIDTH      120
#define RM_HEIGHT     240
#endif

#define RM_NOP        0x00
#define RM_SWRESET    0x01  //Software Reset
#define RM_RDDID      0x04  //Read Display ID
#define RM_RDNUMED    0x05  //Read Number of Errors on DSI
#define RM_RDDPM      0x0A  //Read Display Power Mode
#define RM_RDDMADCTR  0x0B  //Read Display MADCTR
#define RM_RDDCOLMOD  0x0C  //Read Display Pixel Format
#define RM_RDDIM      0x0D  //Read Display Image Mode
#define RM_RDDSM      0x0E  //Read Display Signal Mode
#define RM_RDDSDR     0x0F  //Read Display Self-Diagnostic Result
#define RM_SLPIN      0x10  //Sleep In
#define RM_SLPOUT     0x11  //Sleep Out
#define RM_PTLON      0x12  //Partial Display Mode On
#define RM_NORON      0x13  //Normal Display Mode On
#define RM_INVOFF     0x20  //Display Inversion Off
#define RM_INVON      0x21  //Display Inversion On
#define RM_ALLPOFF    0x22  //All Pixel Off
#define RM_ALLPON     0x23  //All Pixel On
#define RM_DISPOFF    0x28  //Display Off
#define RM_DISPON     0x29  //Display On
#define RM_CASET      0x2A  //Set Column Start Address
#define RM_RASET      0x2B  //Set Row Start Address
#define RM_RAMWR      0x2C  //Memory Write
#define RM_RAMRD      0x2E  //Memory Read
#define RM_PTLHAR     0x30  //Horizontal Partial Area
#define RM_PTLVAR     0x31  //Vertical Partial Area
#define RM_TEOFF      0x34  //Tearing Effect Line Off
#define RM_TEON       0x35  //Tearing Effect Line On
#define RM_MADCTR     0x36  //Scan Direction Control
#define RM_IDMOFF     0x38  //Idle Mode Off
#define RM_IDMON      0x39  //Idle Mode On
#define RM_COLMOD     0x3A  //Interface Pixel Format
#define RM_RAMWRC     0x3C  //Memory Continuous Write
#define RM_RAMRDC     0x3E  //Memory Continuous Read
#define RM_STESL      0x44  //Set Tear Scanline
#define RM_GSL        0x45  //Get Scanline
#define RM_DSTBON     0x4F  //Deep Standby Mode On
#define RM_WRDISBV    0x51  //Write Display Brightness
#define RM_RDDISBV    0x52  //Read Display Brightness
#define RM_WRCTRLD    0x53  //Write Display Control
#define RM_RDCTRLD    0x54  //Read Display Control
#define RM_STCLR      0x58  //Set Color Enhance, sunlight
#define RM_RDCLR      0x59  //Read Color Enhance
#define RM_STCLREHCL  0x5A  //Set Color Enhance1, low byte ambient light
#define RM_STCLRENCH  0x5B  //Set Color Enhance1, high byte ambient light
#define RM_RDDDBS     0xA1  //Read DDB Start
#define RM_RDDDBC     0xA8  //Read DDB Continous
#define RM_RDFCS      0xAA  //Read First Checksum
#define RM_RDCCS      0xAF  //Read Continue Checksum
#define RM_STDSIM     0xC2  //Set DSI Mode
#define RM_STDSPIM    0xC4  //set DSPI Mode
#define RM_CMDMS      0xFE  //CMD Mode Switch, Manufacture Command Set Control
#define RM_RDCMDS     0xFF  //Read CMD Status

#ifdef SCREEN_DRIVER_RM67160
const uint8_t
    init_cmds1[] = {
        126, 
        //136,
        0xFE, 1, 0x04,  //page4
        0x00, 1, 0xDC,
        0x01, 1, 0x00,
        0x02, 1, 0x02,
        0x03, 1, 0x00,
        0x04, 1, 0x00,
        0x05, 1, 0x03,
        0x06, 1, 0x16,
        0x07, 1, 0x13,
        0x08, 1, 0x08,
        0x09, 1, 0xDC,
        0x0A, 1, 0x00,
        0x0B, 1, 0x02,
        0x0C, 1, 0x00,
        0x0D, 1, 0x00,
        0x0E, 1, 0x02,
        0x0F, 1, 0x16,
        0x10, 1, 0x18,
        0x11, 1, 0x08, 
        0x12, 1, 0xC2,
        0x13, 1, 0x00,
        0x14, 1, 0x34,
        0x15, 1, 0x05,
        0x16, 1, 0x00,
        0x17, 1, 0x03,
        0x18, 1, 0x15,
        0x19, 1, 0x41,
        0x1A, 1, 0x00,
        0x1B, 1, 0xDC,
        0x1C, 1, 0x00,
        0x1D, 1, 0x04,
        0x1E, 1, 0x00,
        0x1F, 1, 0x00,
        0x20, 1, 0x03,
        0x21, 1, 0x16,
        0x22, 1, 0x18, 
        0x23, 1, 0x08,
        0x24, 1, 0xDC,
        0x25, 1, 0x00,
        0x26, 1, 0x04,
        0x27, 1, 0x00,
        0x28, 1, 0x00,
        0x29, 1, 0x01,
        0x2A, 1, 0x16,
        0x2B, 1, 0x18,
        0x2D, 1, 0x08,
        0x4C, 1, 0x99,
        0x4D, 1, 0x00,
        0x4E, 1, 0x00,
        0x4F, 1, 0x00,
        0x50, 1, 0x01,
        0x51, 1, 0x0A,
        0x52, 1, 0x00,
        0x5A, 1, 0xE4,
        0x5E, 1, 0x77,
        0x5F, 1, 0x77,
        0x60, 1, 0x34,
        0x61, 1, 0x02,
        0x62, 1, 0x81,

        0xFE, 1, 0x07,  //page7
        0x07, 1, 0x4F,
    
        0xFE, 1, 0x01,  //page1
        0x04, 1, 0x80,
        0x05, 1, 0x65,
        0x06, 1, 0x1E,
        0x0E, 1, 0x8B,
        0x0F, 1, 0x8B,
        0x10, 1, 0x11,
        0x11, 1, 0xA2,
        0x13, 1, 0x00,
        0x14, 1, 0x81,
        0x15, 1, 0x82,
        0x18, 1, 0x45,
        0x19, 1, 0x34,
        0x1A, 1, 0x10, 
        0x1C, 1, 0x57,
        0x1D, 1, 0x02,
        0x21, 1, 0xF8,
        0x22, 1, 0x90,
        0x23, 1, 0x00,
        0x25, 1, 0x0A,
        0x26, 1, 0x46,
        0x2A, 1, 0x47,
        0x2B, 1, 0xFF,
        0x2D, 1, 0xFF,
        0x2F, 1, 0xAE,
        0x37, 1, 0x0C,
        0x3A, 1, 0x00,
        0x3B, 1, 0x20,
        0x3D, 1, 0x0B,
        0x3F, 1, 0x38,
        0x40, 1, 0x0B,
        0x41, 1, 0x0B,
        0x42, 1, 0x11,
        0x43, 1, 0x44,
        0x44, 1, 0x22,
        0x45, 1, 0x55,
        0x46, 1, 0x33,
        0x47, 1, 0x66,
        0x4C, 1, 0x11,
        0x4D, 1, 0x44,
        0x4E, 1, 0x22,
        0x4F, 1, 0x55,
        0x50, 1, 0x33,
        0x51, 1, 0x66,
        0x56, 1, 0x11,
        0x58, 1, 0x44,
        0x59, 1, 0x22,
        0x5A, 1, 0x55,
        0x5B, 1, 0x33,
        0x5C, 1, 0x66,
        0x61, 1, 0x11,
        0x62, 1, 0x44,
        0x63, 1, 0x22,
        0x64, 1, 0x55,
        0x65, 1, 0x33,
        0x66, 1, 0x66,
        0x6D, 1, 0x90,
        0x6E, 1, 0x40,
        0x70, 1, 0xA5,
        0x72, 1, 0x04,
        0x73, 1, 0x15,
        
        0xFE, 1, 0x0A,  //page10
        0x29, 1, 0x90,
        
        0xFE, 1, 0x05,  //page5
        0x05, 1, 0x15,  //ELVSS -2.4V depends on power ic
#if 0        
        0xFE,       1, 0x00,                        //page0
        RM_WRDISBV, 1, 0xFF,                        // write brightness
        RM_TEON,    1, 0x00,                        // tear on
        RM_COLMOD,  1, 0x55,                        // pixel format, rgb565
        RM_MADCTR,  1, 0x00,                        // scan mode forward
        RM_CASET,   4, 0x00, 0x00, 0x00, 0xB3,      // set column, 0-179
        RM_RASET,   4, 0x00, 0x00, 0x00, 0x77,      // set row, 0-119
        RM_WRCTRLD, 1, 0x28,                        // Brightness control on, Display dimming control on
        RM_SLPOUT,  1|RM_DELAY, 0x00, 120,          // sleep out, then wait 120ms
        RM_DISPON,  1|RM_DELAY, 0x00, 20            // display on, then wait 20ms
#endif
    };
const uint8_t
    init_cmds2[] = {
        10,
        0xFE,       1, 0x00,                        //page0
        RM_WRDISBV, 1, 0xFF,                        // write brightness
        RM_TEON,    1, 0x00,                        // tear on
        RM_COLMOD,  1, 0x55,                        // pixel format, rgb565
        RM_MADCTR,  1, 0x00,                        // scan mode forward
        RM_CASET,   4, 0x00, 0x00, 0x00, 0xB3,      // set column, 0-179
        RM_RASET,   4, 0x00, 0x00, 0x00, 0x77,      // set row, 0-119
        RM_WRCTRLD, 1, 0x28,                        // Brightness control on, Display dimming control on
        RM_SLPOUT,  1|RM_DELAY, 0x00, 120,          // sleep out, then wait 120ms
        RM_DISPON,  1|RM_DELAY, 0x00, 20            // display on, then wait 20ms
    };
#else
const uint8_t
    init_cmds1[] = {
        146,
        0xFE, 1, 0x01,  //page01
        0x04, 1, 0xA0, 	
        0x05, 1, 0x70, 		
        0x06, 1, 0x3C, 	
        0x25, 1, 0x06, 
        0x26, 1, 0x57, 
        0x27, 1, 0x12, 
        0x28, 1, 0x12, 	
        0x2A, 1, 0x06,
        0x2B, 1, 0x57, 
        0x2D, 1, 0x12, 
        0x2F, 1, 0x12,
        0x37, 1, 0x0C,
        0x6D, 1, 0x18, //SKIP Frame setting
        0x29, 1, 0x01,
        0x30, 1, 0x41, //write_com(0x17); write_para(0x44);
        0x3A, 1, 0x1D, //Switch timing setting	
        0x3B, 1, 0x00,
        0x3D, 1, 0x16,
        0x3F, 1, 0x2D,
        0x40, 1, 0x14,
        0x41, 1, 0x0D,
        0x42, 1, 0x63, //Switch output setting
        0x43, 1, 0x36,
        0x44, 1, 0x41,
        0x45, 1, 0x14,
        0x46, 1, 0x52,
        0x47, 1, 0x25,
        0x48, 1, 0x63,
        0x49, 1, 0x36,
        0x4A, 1, 0x41,
        0x4B, 1, 0x14,
        0x4C, 1, 0x52,
        0x4D, 1, 0x25,
        0x4E, 1, 0x63, //Source Data output setting
        0x4F, 1, 0x36,	
        0x50, 1, 0x41,
        0x51, 1, 0xFF,
        0x52, 1, 0x52,
        0x53, 1, 0x25,
        0x54, 1, 0x63,
        0x55, 1, 0x36,
        0x56, 1, 0x41,
        0x57, 1, 0x14,	
        0x58, 1, 0x52,
        0x59, 1, 0x25,	
        0x66, 1, 0x10, //idle mode power setting //write_com(0x66); write_para(0x90);
        0x67, 1, 0x40,
        0x70, 1, 0xA5,
        0x72, 1, 0x1A,
        0x73, 1, 0x15,
        0x74, 1, 0x0C,
        0x6A, 1, 0x1F, //external swire pulse
  
        0xFE, 1, 0x04, //page04 //GOA Setting
        0x00, 1, 0xDC,
        0x01, 1, 0x00,
        0x02, 1, 0x02,
        0x03, 1, 0x00,
        0x04, 1, 0x00,
        0x05, 1, 0x01,
        0x06, 1, 0x09,
        0x07, 1, 0x0A,
        0x08, 1, 0x00,
        0x09, 1, 0xDC,
        0x0A, 1, 0x00,
        0x0B, 1, 0x02,
        0x0C, 1, 0x00,
        0x0D, 1, 0x00,
        0x0E, 1, 0x00,
        0x0F, 1, 0x09,
        0x10, 1, 0x0A,
        0x11, 1, 0x00,
        0x12, 1, 0xDC,
        0x13, 1, 0x00,
        0x14, 1, 0x02,
        0x15, 1, 0x00,
        0x16, 1, 0x08,
        0x17, 1, 0x01,
        0x18, 1, 0xA3,
        0x19, 1, 0x00,
        0x1A, 1, 0x00,
        0x1B, 1, 0xDC,
        0x1C, 1, 0x00,
        0x1D, 1, 0x02,
        0x1E, 1, 0x00,
        0x1F, 1, 0x08,
        0x20, 1, 0x00,
        0x21, 1, 0xA3,
        0x22, 1, 0x00,
        0x23, 1, 0x00,
        0x4C, 1, 0x89,//skip frame 
        0x4D, 1, 0x00,
        0x4E, 1, 0x01,
        0x4F, 1, 0x08,
        0x50, 1, 0x01,
        0x51, 1, 0x85,
        0x52, 1, 0x7C,
        0x53, 1, 0x8A, 
        0x54, 1, 0x50,
        0x55, 1, 0x02,
        0x56, 1, 0x48,
        0x58, 1, 0x34,
        0x59, 1, 0x00,
        0x5E, 1, 0xBB,
        0x5F, 1, 0xBB,
        0x60, 1, 0x09,
        0x61, 1, 0xB1,
        0x62, 1, 0xBB,
        0x65, 1, 0x05,
        0x66, 1, 0x04,
        0x67, 1, 0x00,
        0x78, 1, 0xBB,
        0x79, 1, 0x8B,
        0x7A, 1, 0x32,
  
        0xFE, 1, 0x01,  //page01 //power setting
        0x0E, 1, 0x85,
        0x0F, 1, 0x85,
        0x10, 1, 0x11,
        0x11, 1, 0xA0,
        0x12, 1, 0xA0,
        0x13, 1, 0x81,
        0x14, 1, 0x81,
        0x15, 1, 0x82,
        0x16, 1, 0x82,
        0x18, 1, 0x55,
        0x19, 1, 0x33,
        0x1E, 1, 0x02,
        0x5B, 1, 0x10, //write_com(0x5E); write_para(0x17); //write_com(0x5F); write_para(0x17);
        0x62, 1, 0x15,
        0x63, 1, 0x15,
        0x6A, 1, 0x00,
        0x70, 1, 0x55,
        0x1D, 1, 0x02,
        0x89, 1, 0xF8,
        0x8A, 1, 0x80,
        0x8B, 1, 0x01,

        0xFE, 1, 0x00,  //user command
        RM_CASET,   4, 0x00, 0x04, 0x00, 0x7B,
        RM_RASET,   4, 0x00, 0x00, 0x00, 0xEF,
        RM_TEON,    1, 0x01,
        RM_MADCTR,  1, 0x00,    //æ‰«ææ–¹å‘
        RM_COLMOD,  1, 0x75,    //16.7Mè‰²é»˜è®?
        RM_WRDISBV, 1, 0xA0,    //äº®åº¦è°ƒèŠ‚ï¼Œé»˜è®?xff
        RM_STDSPIM, 1, 0x80,	//set_DSPI Mode .å†™ramä½¿èƒ½
        RM_SLPOUT,  1|RM_DELAY, 0x00, 120,          // sleep out, then wait 120ms
        RM_DISPON,  1|RM_DELAY, 0x00, 20            // display on, then wait 20ms
};
#endif

#ifdef POWER_CHIP_PIN
#define CTRL_PULSE  21
extern void dwt_wait_us(uint32_t us);
static void ctrl_pulse(pin_t pin)
{
    gpio_write_pin(pin, 0);
    dwt_wait_us(10);
    gpio_write_pin(pin, 1);
    dwt_wait_us(10);
}

static void init_vneg(pin_t pin)
{
    gpio_set_output_pushpull(pin);
    gpio_write_pin(pin, 1);
    wait_ms(20); // wait until chip stable

    for (uint32_t i = 0; i < CTRL_PULSE; i++) {
        ctrl_pulse(pin);
    }
    wait_ms(30); // wait until vneg stable 
}

#endif
static spi_handle_t spi;

static void rm67160_select(rm67160_t *driver)
{
    gpio_write_pin(driver->cs, 0);
}

void rm67160_unselect(rm67160_t *driver)
{
    gpio_write_pin(driver->cs, 1);
}

void rm67160_reset(rm67160_t *driver)
{
    gpio_write_pin(driver->reset, 1);
    wait_ms(100);
    gpio_write_pin(driver->reset, 0);
    wait_ms(100);
    gpio_write_pin(driver->reset, 1);
    wait_ms(500);
}

static void write_command(rm67160_t *driver, uint8_t cmd)
{
    gpio_write_pin(driver->dc, 0);
    spi_send(spi, &cmd, sizeof(cmd));
}

static void write_data(rm67160_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->dc, 1);
    spi_send(spi, buff, buff_size);
}

void write_data_async(rm67160_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->dc, 1);
    spi_send_async(spi, buff, buff_size);
}

void set_address_window(rm67160_t *driver, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    write_command(driver, RM_CASET);
    uint8_t data[] = { 0x00, RM_X_START+x0, 0x00, RM_X_START+x1};
    write_data(driver, data, sizeof(data));

    // row address set
    write_command(driver, RM_RASET);   
    data[1] = RM_Y_START+y0;
    data[3] = RM_Y_START+y1;
    write_data(driver, data, sizeof(data));

    // write to RAM
    write_command(driver, RM_RAMWR);
}

void execute_commands(rm67160_t *driver, const uint8_t *addr)
{
    uint8_t commands, args;
    uint16_t ms;

    commands = *addr++;
    while(commands--) {
        uint8_t cmd = *addr++;
        write_command(driver, cmd);

        args = *addr++;
        // If high bit set, delay follows args
        ms = args & RM_DELAY;
        args &= ~RM_DELAY;
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


#ifndef RM67160_SPI_ID
#define RM67160_SPI_ID  SPI_INSTANCE_1
#endif

void rm67160_init(rm67160_t *driver)
{
    wait_ms(1000);
#if 1
    spi = spi_init(RM67160_SPI_ID);

    rm67160_reset(driver);
    rm67160_select(driver);
    execute_commands(driver, init_cmds1);
    execute_commands(driver, init_cmds2);


#ifdef POWER_CHIP_PIN
    init_vneg(driver->ctrl);
#endif
    set_address_window(driver, 0, 0, RM_WIDTH-1, RM_HEIGHT-1);
    uint16_t color = 0xFFFF;
    for (int x = 0; x < RM_WIDTH; x++) {
        for (int y = 0; y < RM_HEIGHT; y++) {
            write_data(driver, (uint8_t*)&color, sizeof(color));
        }
    }
    rm67160_unselect(driver);


#endif
}

void rm67160_fill_rect(rm67160_t *driver, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    rm67160_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data(driver, data, size);
    rm67160_unselect(driver);
}

void rm67160_fill_rect_async(rm67160_t *driver, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    rm67160_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data_async(driver, data, size);
    //rm67160_unselect(driver);
}

bool rm67160_fill_ready(rm67160_t *driver)
{
    return spi_ready(spi);
}

void rm67160_release(rm67160_t *driver)
{
    rm67160_unselect(driver);
}

void rm67160_fill(rm67160_t *driver, const void* data)
{
    rm67160_fill_rect(driver, 0, 0, RM_WIDTH, RM_HEIGHT, data, RM_WIDTH*RM_HEIGHT*2);
}

void rm67160_uninit(rm67160_t *driver)
{}