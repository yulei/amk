/**
 * @file mscusb.c
 *  msc backend implementatation
 *  Note: current only support w25qxx devices
 */

#include "mscusb.h"

#if defined(QSPI_ENABLE)
#include "qspi.h"
#elif defined(SD_NAND_ENABLE)
#include "sd_nand.h"
#else
#include "w25qxx.h"
#endif

#ifndef DISK_COUNT
#define DISK_COUNT          1
#endif

#ifndef DISK_TOTAL_SIZE
#define DISK_TOTAL_SIZE     (16*1024*1024)        //W25Q128, 16M Bytes
#endif

#ifdef SD_NAND_ENABLE
#define DISK_BLOCK_SIZE     sd_nand_get_block_size()
#else
#define DISK_BLOCK_SIZE     (4096)
#endif

#ifdef SD_NAND_ENABLE
#define DISK_BLOCK_NUM      sd_nand_get_block_count()
#else
#define DISK_BLOCK_NUM      (DISK_TOTAL_SIZE/DISK_BLOCK_SIZE)
#endif

#if defined(W25QXX_ENABLE)
static w25qxx_config_t w25qxx_config;
static w25qxx_t *w25qxx;
#endif

__attribute__((weak))
void msc_init_kb(void) {}

#ifdef WDT_ENABLE
void wdt_refresh(void);
#endif

void msc_init(void)
{
#if defined(QSPI_ENABLE)
    qspi_init(0);
#elif defined(SD_NAND_ENABLE)
    sd_nand_init();
#else
    w25qxx_config.cs = FLASH_CS;
    w25qxx_config.spi = spi_init(W25QXX_SPI_ID);
    w25qxx = w25qxx_init(&w25qxx_config);
#endif

    msc_init_kb();
}

void msc_erase(void)
{
#if defined(QSPI_ENABLE)
    qspi_erase_chip();
#elif defined(SD_NAND_ENABLE)
    sd_nand_erase_chip();
#else
    w25qxx_erase_chip(w25qxx);
#endif
}

__attribute__((weak))
void msc_task_kb(void) {}

void msc_task(void)
{
    msc_task_kb();
}


#ifdef TINYUSB_ENABLE
#include "tusb.h"

// Invoked to determine max LUN
uint8_t tud_msc_get_maxlun_cb(void)
{
    return DISK_COUNT;
}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void) lun; // use same ID for both LUNs

    const char vid[] = "Matrix";
    const char pid[] = "Screen";
    const char rev[] = "1.0";

    memcpy(vendor_id  , vid, strlen(vid));
    memcpy(product_id , pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    (void) lun;

    return true; // RAM disk is always ready
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
    (void) lun;

    *block_count = DISK_BLOCK_NUM;
    *block_size  = DISK_BLOCK_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    (void) lun;
    (void) power_condition;

    if (load_eject) {
        if (start) {
        // load disk storage
        }else {
        // unload disk storage
        }
    }

    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
#ifdef WDT_ENABLE
    wdt_refresh();
#endif
#if defined(QSPI_ENABLE)
    qspi_read_sector(lba*DISK_BLOCK_SIZE, buffer, bufsize);
#elif defined(SD_NAND_ENABLE)
    sd_nand_read_blocks(lba*DISK_BLOCK_SIZE, buffer, bufsize);
#else
    w25qxx_read_sector(w25qxx, lba*DISK_BLOCK_SIZE, buffer, bufsize);
#endif
    return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
#ifdef WDT_ENABLE
    wdt_refresh();
#endif
#if defined(QSPI_ENABLE)
    qspi_write_sector(lba*DISK_BLOCK_SIZE, buffer, bufsize);
#elif defined(SD_NAND_ENABLE)
    sd_nand_write_blocks(lba*DISK_BLOCK_SIZE, buffer, bufsize);
#else
    w25qxx_write_sector(w25qxx, lba*DISK_BLOCK_SIZE, buffer, bufsize);
#endif
    return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
    // read10 & write10 has their own callback and MUST not be handled here

    void const* response = NULL;
    int32_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0])
    {
        default:
        // Set Sense = Invalid Command Operation
        tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

        // negative means error -> tinyusb could stall and/or response with failed status
        resplen = -1;
        break;
    }

    // return resplen must not larger than bufsize
    if ( resplen > bufsize ) resplen = bufsize;

    if ( response && (resplen > 0) )
    {
        if(in_xfer)
        {
        memcpy(buffer, response, resplen);
        }else
        {
        // SCSI output
        }
    }

    return resplen;
}

#else

#include "usbd_composite.h"

#define INQUIRY_DATA_LEN 0x24U

const int8_t inquiry_data[] = {/* 36 */
  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'M', 'A', 'T', 'R', 'I', 'X', ' ', ' ', /* Manufacturer : 8 bytes */
  'S', 'c', 'r', 'e', 'e', 'n', ' ', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '1', '.', '0' ,'1'                      /* Version      : 4 Bytes */
};

static int8_t storage_init(uint8_t lun)
{
    return (USBD_OK);
}

static int8_t storage_get_capacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    *block_num  = DISK_BLOCK_NUM;
    *block_size = DISK_BLOCK_SIZE;
    return (USBD_OK);
}

static int8_t storage_is_ready(uint8_t lun)
{
    return (USBD_OK);
}

static int8_t storage_is_write_protected(uint8_t lun)
{
    return (USBD_OK);
}
#ifdef WDT_ENABLE
extern void wdt_refresh(void);
#endif
static int8_t storage_read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
#ifdef WDT_ENABLE
    wdt_refresh();
#endif
    w25qxx_read_sector(w25qxx, blk_addr*DISK_BLOCK_SIZE, buf, DISK_BLOCK_SIZE*blk_len);
    return (USBD_OK);
}

static int8_t storage_write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
#ifdef WDT_ENABLE
    wdt_refresh();
#endif
    w25qxx_write_sector(w25qxx, blk_addr*DISK_BLOCK_SIZE, buf, DISK_BLOCK_SIZE*blk_len);
    return (USBD_OK);
}

static int8_t storage_get_max_lun(void)
{
    return (DISK_COUNT-1);
}

USBD_StorageTypeDef storage_ops =
{
    storage_init,
    storage_get_capacity,
    storage_is_ready,
    storage_is_write_protected,
    storage_read,
    storage_write,
    storage_get_max_lun,
    (int8_t *)inquiry_data
};

#endif