/**
 * @file w25qxx.c
 */

#include "w25qxx.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef W25QXX_NUM
#define W25QXX_NUM 1
#endif

#define WQCMD_DUMMY 0xA5

//Write Enable
#define WQCMD_WRITE_ENABLE 0x06
//Volatile SR Write Enable
#define WQCMD_SR_WRITE_ENABLE 0x50
//Write Disable
#define WQCMD_WRITE_DISABLE 0x04
//Release Power-down / ID
#define WQCMD_RELEASE_OR_ID 0xAB
//Manufacturer/Device ID
#define WQCMD_MANUFACTURER_OR_ID 0x90
//JEDEC ID
#define WQCMD_JEDEC_ID 0x9F
//Read Unique ID
#define WQCMD_UNIQUE_ID 0x4B
//Read Data
#define WQCMD_READ_DATA 0x03
//Fast Read
#define WQCMD_FAST_READ 0x0B
//Page Program
#define WQCMD_PAGE_PROGRAM 0x02
//Sector Erase (4KB)
#define WQCMD_SECTOR_ERASE_4K 0x20
//Block Erase (32KB)
#define WQCMD_BLOCK_ERASE_32K 0x52
//Block Erase (64KB)
#define WQCMD_BLOCK_ERASE_64K 0xD8
//Chip Erase
#define WQCMD_CHIP_ERASE 0xC7 // or 0x60
//Read Status Register-1
#define WQCMD_READ_STATUS_1 0x05
//Write Status Register-1(4)
#define WQCMD_WRITE_STATUS_1 0x01
//Read Status Register-2
#define WQCMD_READ_STATUS_2 0x35
//Write Status Register-2
#define WQCMD_WRITE_STATUS_2 0x31
//Read Status Register-3
#define WQCMD_READ_STATUS_3 0x15
//Write Status Register-3
#define WQCMD_WRITE_STATUS_3 0x11
//Read SFDP Register
#define WQCMD_READ_SFDP 0x5A
//Erase Security Register(5)
#define WQCMD_ERASE_SECURITY 0x44
//Program Security Register(5)
#define WQCMD_PROGRAM_SECURITY 0x42
//Read Security Register(5)
#define WQCMD_READ_SECURITY 0x48
//Global Block Lock
#define WQCMD_GLOBAL_BLOCK_LOCK 0x7E
//Global Block Unlock
#define WQCMD_GLOBAL_BLOCK_UNLOCK 0x98
//Read Block Lock
#define WQCMD_READ_BLOCK_LOCK 0x3D
//Individual Block Lock
#define WQCMD_INDIVIDUAL_BLOCK_LOCK 0x36
//Individual Block Unlock
#define WQCMD_INDIVIDUAL_BLOCK_UNLOCK 0x39
//Erase / Program Suspend
#define WQCMD_ERASE_PROGRAM_SUSPEND 0x75
//Erase / Program Resume
#define WQCMD_ERASE_PROGRAM_RESUME 0x7A
//Power-down
#define WQCMD_POWER_DOWN 0xB9
//Enable Reset
#define WQCMD_ENABLE_RESET 0x66
//Reset Device
#define WQCMD_RESET_DEVICE 0x99
//Fast Read Dual Output
#define WQCMD_FAST_READ_DUAL 0x3B
//Fast Read Dual I/O
#define WQCMD_FAST_READ_DUALIO 0xBB
//Mftr./Device ID Dual I/O
#define WQCMD_MFTR_DUALIO 0x92
//Quad Input Page Program
#define WQCMD_QUAL_INPUT_PAGE_PROGRAM 0x32
//Fast Read Quad Output
#define WQCMD_FAST_READ_QUAD 0x6B
//Mftr./Device ID Quad I/O
#define WQCMD_MFTR_QUADIO 0x94
//Fast Read Quad I/O
#define WQCMD_FAST_READ_QUADIO 0xEB
//Set Burst with Wrap
#define WQCMD_SET_BURST_WITH_WRAP 0x77

static w25qxx_t w25qxxs[W25QXX_NUM];

static uint32_t w25qxx_read_jedec(w25qxx_t *w25qxx);
static amk_error_t w25qxx_erase_sector(w25qxx_t *w25qxx, uint32_t address);
static uint8_t w25qxx_spi_write(w25qxx_t *w25qxx, uint8_t data);
static amk_error_t w25qxx_enable_write(w25qxx_t *w25qxx);
static amk_error_t w25qxx_wait(w25qxx_t *w25qxx);
static amk_error_t w25qxx_write_address(w25qxx_t *w25qxx, uint8_t cmd, uint32_t address);
static bool w25qxx_sector_empty(w25qxx_t *w25qxx, uint32_t address);

w25qxx_t *w25qxx_current(void)
{
    return &w25qxxs[0];
}

w25qxx_t *w25qxx_init(w25qxx_config_t *config)
{
    w25qxx_t *device = &w25qxxs[0];
    device->config.spi = config->spi;
    device->config.cs = config->cs;
    gpio_set_output_pushpull(config->cs);
    gpio_write_pin(config->cs, 0);
    wait_ms(100);
    uint32_t id = w25qxx_read_jedec(device);

    switch(id&0xFFFF) {
    case 0x4018:
        // W25Q128
        amk_printf("W25Q128 device identified: %x\n", id);
        device->page_size = 256;
        device->page_count = 65536;
        device->sector_size = 4096;
        device->sector_count = 4096;
        device->type = W25Q128; 
        break;;
    default:
        amk_printf("Unknown W25QXX chip: %x\n", id);
        device = NULL;
        break;
    }

    gpio_write_pin(config->cs, 1);
    return device;
}

static amk_error_t w25qxx_erase_sector(w25qxx_t *w25qxx, uint32_t address)
{
    w25qxx_wait(w25qxx);
    w25qxx_enable_write(w25qxx);

    gpio_write_pin(w25qxx->config.cs, 0);
    w25qxx_write_address(w25qxx, WQCMD_SECTOR_ERASE_4K, address);
    gpio_write_pin(w25qxx->config.cs, 1);

    w25qxx_wait(w25qxx);
    return AMK_SUCCESS;
}

static void w25qxx_write_page(w25qxx_t *w25qxx, uint32_t address, const uint8_t *data, uint32_t size)
{
    w25qxx_wait(w25qxx);
    w25qxx_enable_write(w25qxx);

    gpio_write_pin(w25qxx->config.cs, 0);
    w25qxx_write_address(w25qxx, WQCMD_PAGE_PROGRAM, address);
    spi_send(w25qxx->config.spi, data, size);
    gpio_write_pin(w25qxx->config.cs, 1);

    w25qxx_wait(w25qxx);
}

amk_error_t w25qxx_write_sector(w25qxx_t* w25qxx, uint32_t address, const uint8_t *data, uint32_t size)
{
    if (address % w25qxx->sector_size) {
        amk_printf("unsupport wrtie operation: address=%d, size=%d\n", address, size);
        return AMK_ERROR;
    }

    // check if the sector empty
    if (!w25qxx_sector_empty(w25qxx, address)) {
        // ease the sector first
        if (w25qxx_erase_sector(w25qxx, address) != AMK_SUCCESS) {
            amk_printf("failed to erase flash sector at: 0x%x\n", address);
            return AMK_SPI_ERROR;
        }
    }

    for (int i = 0; i < w25qxx->sector_size/w25qxx->page_size; i++) {
		w25qxx_write_page(w25qxx, address, data, w25qxx->page_size);
        address += w25qxx->page_size;
        data += w25qxx->page_size;
    }

    return AMK_SUCCESS;
}

amk_error_t w25qxx_read_sector(w25qxx_t* w25qxx, uint32_t address, uint8_t *data, uint32_t size)
{
    if (address % w25qxx->sector_size) {
        amk_printf("unsupport read operation: address=%d, size=%d\n", address, size);
        return AMK_ERROR;
    }

    return w25qxx_read_bytes(w25qxx, address, data, size);
}

amk_error_t w25qxx_read_bytes(w25qxx_t* w25qxx, uint32_t address, uint8_t *data, uint32_t size)
{
    gpio_write_pin(w25qxx->config.cs, 0);
    w25qxx_write_address(w25qxx, WQCMD_FAST_READ, address);
	w25qxx_spi_write(w25qxx, 0);
    spi_recv(w25qxx->config.spi, data, size);
    gpio_write_pin(w25qxx->config.cs, 1);

    return AMK_SUCCESS;
}

static uint32_t w25qxx_read_jedec(w25qxx_t *w25qxx)
{
    uint8_t tx[4] = {WQCMD_JEDEC_ID, WQCMD_DUMMY, WQCMD_DUMMY, WQCMD_DUMMY};
    uint8_t rx[4] = {0, 0, 0, 0};
    /*uint8_t src;
    src = WQCMD_JEDEC_ID;
    spi_xfer(w25qxx->config.spi, &src, &rx[0], 1);
    src = WQCMD_DUMMY;
    spi_xfer(w25qxx->config.spi, &src, &rx[1], 1);
    spi_xfer(w25qxx->config.spi, &src, &rx[2], 1);
    spi_xfer(w25qxx->config.spi, &src, &rx[3], 1);
    */
    spi_xfer(w25qxx->config.spi, &tx[0], &rx[0], 4);
    uint32_t id = (rx[1] << 16) | (rx[2] << 8) | rx[3];
    return id;
}

static uint8_t w25qxx_spi_write(w25qxx_t *w25qxx, uint8_t data)
{
	uint8_t	ret;
    spi_xfer(w25qxx->config.spi, &data, &ret, 1);
	return ret;	
}

static amk_error_t w25qxx_enable_write(w25qxx_t *w25qxx)
{
    gpio_write_pin(w25qxx->config.cs, 0);
    w25qxx_spi_write(w25qxx, WQCMD_WRITE_ENABLE);
    gpio_write_pin(w25qxx->config.cs, 1);
    return AMK_SUCCESS;
}

static amk_error_t w25qxx_wait(w25qxx_t *w25qxx)
{
    gpio_write_pin(w25qxx->config.cs, 0);
    uint8_t status = w25qxx_spi_write(w25qxx, WQCMD_READ_STATUS_1);
    do {
        status = w25qxx_spi_write(w25qxx, WQCMD_DUMMY);
		wait_ms(1);
    } while (( status & 0x01) == 0x01);
    gpio_write_pin(w25qxx->config.cs, 1);
    return AMK_SUCCESS;
}

static bool w25qxx_sector_empty(w25qxx_t *w25qxx, uint32_t address)
{
    return false;
}

static amk_error_t w25qxx_write_address(w25qxx_t *w25qxx, uint8_t cmd, uint32_t address)
{
	w25qxx_spi_write(w25qxx, cmd);
    w25qxx_spi_write(w25qxx, (address & 0x00FF0000) >> 16);
    w25qxx_spi_write(w25qxx, (address & 0x0000FF00) >> 8);
    w25qxx_spi_write(w25qxx, (address & 0x000000FF) >> 0);

    return AMK_SUCCESS;
}