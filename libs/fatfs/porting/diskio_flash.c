/**
 * @file diskio_flash.c
 *  flash driver support
 */

#include "ff.h"
#include "diskio.h"

#if defined(QSPI_ENABLE)
#include "qspi.h"
#elif defined(SD_NAND_ENABLE)
#include "sd_nand.h"
#else
#include "w25qxx.h"
#endif

#define DEV_FLASH           0
#ifdef SD_NAND_ENABLE
#define DISK_SECTOR_SIZE    512
#else
#define DISK_SECTOR_SIZE    4096
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(BYTE pdrv)
{
    (void)pdrv;
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE pdrv)
{
    (void)pdrv;
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
#if defined(QSPI_ENABLE)
    for (UINT i = 0; i < count; i++) {
        qspi_read_sector((sector+i)*DISK_SECTOR_SIZE, buff+i*DISK_SECTOR_SIZE, DISK_SECTOR_SIZE);
    }
#elif defined(SD_NAND_ENABLE)
    sd_nand_read_blocks(sector*DISK_SECTOR_SIZE, buff, count*DISK_SECTOR_SIZE);
#else
    w25qxx_read_bytes(w25qxx_current(), sector*DISK_SECTOR_SIZE, buff, count*DISK_SECTOR_SIZE);
#endif
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
#if defined(QSPI_ENABLE)
    for(UINT i = 0; i < count; i++) {
        qspi_write_sector((sector+i)*DISK_SECTOR_SIZE, buff+i*DISK_SECTOR_SIZE, DISK_SECTOR_SIZE);
    }
#elif defined(SD_NAND_ENABLE)
    sd_nand_write_blocks(sector*DISK_SECTOR_SIZE, buff, count*DISK_SECTOR_SIZE);
#else
    for(UINT i = 0; i < count; i++) {
        w25qxx_write_sector(w25qxx_current(), (sector+i)*DISK_SECTOR_SIZE, buff+i*DISK_SECTOR_SIZE, DISK_SECTOR_SIZE);
    }
#endif
    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    return RES_OK;
}

