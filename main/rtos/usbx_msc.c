/**
 * @file usbx_msc.c
 */

#include "tx_api.h"
#include "ux_api.h"
#include "ux_device_class_storage.h"
#include "usbx_desc.h"
#include "w25qxx.h"

#ifndef DISK_COUNT
#define DISK_COUNT          1
#endif

#ifndef DISK_TOTAL_SIZE
#define DISK_TOTAL_SIZE     (16*1024*1024)        //W25Q128, 16M Bytes
#endif

#define DISK_BLOCK_SIZE     (4096)
#define DISK_BLOCK_NUM      (DISK_TOTAL_SIZE/DISK_BLOCK_SIZE)

#ifndef W25QXX_SPI_ID
#define W25QXX_SPI_ID SPI_INSTANCE_ID_1
#endif
static w25qxx_config_t w25qxx_config;
static w25qxx_t *w25qxx;


#define MSC_READ_FLAG       0x01
#define MSC_WRITE_FLAG      0x02
TX_EVENT_FLAGS_GROUP        EventFlag;

#if 0
static UCHAR storage_vender_id[] = {
    'M', 'a', 't', 'r', 'i', 'x', ' ', 'L', 'a', 'b', '\0',
};

static UCHAR storage_product_id[] = {
    'V', 'i', 't', 'a', '\0',
};

static UCHAR storage_product_rev[] = {
    '1', '.', '0', '0', '\0',
};

static UCHAR storage_product_serial[] = {
    'a', 'm', 'k', ':', '4', 'd', '5', '8', '\0',
};
#endif

UX_SLAVE_CLASS_STORAGE_PARAMETER    storage_parameter;

static void storage_init(void);
static UINT storage_status(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status);
static UINT storage_read(VOID *storage, ULONG lun, UCHAR *data_pointer,
                   ULONG number_blocks, ULONG lba, ULONG *media_status);

static UINT storage_write(VOID *storage, ULONG lun, UCHAR *data_pointer,
                    ULONG number_blocks, ULONG lba, ULONG *media_status);

void usbx_msc_init(void)
{   
    //storage_parameter.ux_slave_class_storage_instance_activate = UX_NULL;
    //storage_parameter.ux_slave_class_storage_instance_deactivate = UX_NULL;
    //storage_parameter.ux_slave_class_storage_parameter_vendor_id = storage_vender_id;
    //storage_parameter.ux_slave_class_storage_parameter_product_id = storage_product_id;
    //storage_parameter.ux_slave_class_storage_parameter_product_rev = storage_product_rev;
    //storage_parameter.ux_slave_class_storage_parameter_product_serial = storage_product_serial;

    storage_parameter.ux_slave_class_storage_parameter_number_lun = DISK_COUNT;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_last_lba = (ULONG)(DISK_BLOCK_NUM - 1);
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_block_length = DISK_BLOCK_SIZE;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_type = UX_SLAVE_CLASS_STORAGE_MEDIA_FAT_DISK;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_removable_flag = 0x80;
    //storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_removable_flag = UX_FALSE;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_read = storage_read;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_write = storage_write;
    storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_status = storage_status;

    /* Initialize the device storage class. The class is connected with interface 0 on configuration 1. */
    UINT status =  ux_device_stack_class_register(_ux_system_slave_class_storage_name,
                                            ux_device_class_storage_entry,
                                            1, ITF_NUM_MSC, (VOID *)&storage_parameter);
    if (status != UX_SUCCESS) {
        while(1);
    }

    storage_init();
}

static void storage_init(void)
{
    // init flash driver
    w25qxx_config.cs = FLASH_CS;
    w25qxx_config.spi = spi_init(W25QXX_SPI_ID);
    w25qxx = w25qxx_init(&w25qxx_config);
}

static UINT storage_status(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status)
{
    return UX_SUCCESS;
}

static UINT storage_read(VOID *storage, ULONG lun, UCHAR *data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{
    w25qxx_read_sector(w25qxx, lba*DISK_BLOCK_SIZE, data_pointer, DISK_BLOCK_SIZE*number_blocks);

    return UX_SUCCESS;
}

static UINT storage_write(VOID *storage, ULONG lun, UCHAR *data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{
    w25qxx_write_sector(w25qxx, lba*DISK_BLOCK_SIZE, data_pointer, DISK_BLOCK_SIZE*number_blocks);

    return UX_SUCCESS;
}
