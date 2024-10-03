/**
 * @file host_msc.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 */

#include "host_msc.h"
#include "usbh_def.h"
#include "usb_host.h"
#include "usbh_msc.h"

#ifndef HOST_MSC_DEBUG
#define HOST_MSC_DEBUG 0
#endif

#if HOST_MSC_DEBUG
#define host_debug  amk_printf
#else
#define host_debug(...)
#endif

extern USBH_HandleTypeDef* MX_USB_HOST_Handle(void);
extern bool MX_USB_HOST_MSC_Ready(void);

#ifndef SLAVE_MSC_BLOCK_COUNT
#define SLAVE_MSC_BLOCK_COUNT   ((16*1024*1024)/4096)
#endif

#ifndef SLAVE_MSC_BLOCK_SIZE
#define SLAVE_MSC_BLOCK_SIZE    (4096)
#endif

bool host_msc_init(void)
{
    return true;
}

uint32_t host_msc_get_block_count(void)
{
    if (MX_USB_HOST_MSC_Ready()) {
        USBH_HandleTypeDef* phost = MX_USB_HOST_Handle();
        MSC_LUNTypeDef info={0};
        USBH_StatusTypeDef status = USBH_MSC_GetLUNInfo(phost, 0, &info);
        if (status == USBH_OK) {
            host_debug("USB Host read block count=%d\n", info.capacity.block_nbr);
            return info.capacity.block_nbr;
        } else {
            host_debug("USB Host failed to read lun info, error:%d\n", status);
        }
    } else {
        host_debug("USB Host failed to read lun info, USB HOST MSC not ready\n");
    }
    return SLAVE_MSC_BLOCK_COUNT;
}

uint32_t host_msc_get_block_size(void)
{
    if (MX_USB_HOST_MSC_Ready()) {
        USBH_HandleTypeDef* phost = MX_USB_HOST_Handle();
        MSC_LUNTypeDef info={0};
        USBH_StatusTypeDef status = USBH_MSC_GetLUNInfo(phost, 0, &info);
        if (status == USBH_OK) {
            host_debug("USB Host read block size=%d\n", info.capacity.block_size);
            return info.capacity.block_size;
        } else {
            host_debug("USB Host failed to read block size, error:%d\n", status);
        }
    } else {
        host_debug("USB Host failed to read block size, USB HOST MSC not ready\n");
    }
    return SLAVE_MSC_BLOCK_SIZE;
}

amk_error_t host_msc_read_blocks(uint32_t block_no, uint8_t *buffer, size_t buffer_size)
{
    if (!MX_USB_HOST_MSC_Ready()) {
        host_debug("USB Host not ready, read blocks\n");
        return AMK_ERROR;
    }

    USBH_HandleTypeDef* phost = MX_USB_HOST_Handle();
    USBH_StatusTypeDef status = USBH_MSC_Read(phost, 0, block_no, buffer, buffer_size/host_msc_get_block_size());
    if (status != USBH_OK) {
        host_debug("USB Host failed to read at:%ld, size:%ld, error:%d\n", block_no, buffer_size, status);
        return AMK_ERROR;
    } else {
        host_debug("USB Host read at:%ld, size:%ld\n", block_no, buffer_size);
    }

    return AMK_SUCCESS;
}

amk_error_t host_msc_write_blocks(uint32_t block_no, const uint8_t* buffer, size_t buffer_size)
{
    if (!MX_USB_HOST_MSC_Ready()) {
        host_debug("USB Host not ready, write blocks\n");
        return AMK_ERROR;
    }

    USBH_HandleTypeDef* phost = MX_USB_HOST_Handle();
    USBH_StatusTypeDef status = USBH_MSC_Write(phost, 0, block_no, (uint8_t *)buffer, buffer_size/host_msc_get_block_size());
    if (status != USBH_OK) {
        host_debug("USB Host failed to write at:%ld, size:%ld, error:%d\n", block_no, buffer_size, status);
        return AMK_ERROR;
    } else {
        host_debug("USB Host write at:%ld, size:%ld\n", block_no, buffer_size, status);
    }

    return AMK_SUCCESS;
}

amk_error_t host_msc_erase_chip(void)
{
    return AMK_SUCCESS;
}

void host_msc_uninit(void)
{
}
