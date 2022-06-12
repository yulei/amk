/**
 * @file diskio_flash.c
 *  flash driver support
 */

#include "ff.h"
#include "diskio.h"

#ifndef QSPI_ENABLE
#include "w25qxx.h"
#else
#include "qspi.h"
#endif

#define DEV_FLASH           0
#define DISK_SECTOR_SIZE    4096

#ifndef QSPI_ENABLE
static w25qxx_t* flash_driver = NULL;
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
#ifndef QSPI_ENABLE
    flash_driver = w25qxx_current();
#endif
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
#ifndef QSPI_ENABLE
    w25qxx_read_bytes(flash_driver, sector*DISK_SECTOR_SIZE, buff, count*DISK_SECTOR_SIZE);
#else
    for (UINT i = 0; i < count; i++) {
        qspi_read_sector((sector+i)*DISK_SECTOR_SIZE, buff+i*DISK_SECTOR_SIZE, DISK_SECTOR_SIZE);
    }
#endif
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
#ifndef QSPI_ENABLE
    for(UINT i = 0; i < count; i++) {
        w25qxx_write_sector(flash_driver, (sector+i)*4096, buff+i*4096, 4096);
    }
#else
    for(UINT i = 0; i < count; i++) {
        qspi_write_sector((sector+i)*DISK_SECTOR_SIZE, buff+i*DISK_SECTOR_SIZE, DISK_SECTOR_SIZE);
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

