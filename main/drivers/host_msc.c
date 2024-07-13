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

extern USBH_HandleTypeDef* MX_USB_HOST_Handle(void);
extern bool MX_USB_HOST_MSC_Ready(void);

bool host_msc_init(void)
{
    return true;
}

uint32_t host_msc_get_block_count(void)
{
    return (16*1024*1024)/4096;
}

uint32_t host_msc_get_block_size(void)
{
    return 4096;
}

amk_error_t host_msc_read_blocks(uint32_t block_no, uint8_t *buffer, size_t buffer_size)
{
    if (!MX_USB_HOST_MSC_Ready()) {
        return AMK_ERROR;
    }

    USBH_HandleTypeDef* phost = MX_USB_HOST_Handle();
    USBH_StatusTypeDef status = USBH_MSC_Read(phost, 0, block_no, buffer, buffer_size/host_msc_get_block_size());
    if (status != USBH_OK) {
        amk_printf("USB Host failed to read at:%ld, size:%ld, error:%d\n", block_no, buffer_size, status);
        return AMK_ERROR;
    }

    return AMK_SUCCESS;
}

amk_error_t host_msc_write_blocks(uint32_t block_no, const uint8_t* buffer, size_t buffer_size)
{
    if (!MX_USB_HOST_MSC_Ready()) {
        return AMK_ERROR;
    }

    USBH_HandleTypeDef* phost = MX_USB_HOST_Handle();
    USBH_StatusTypeDef status = USBH_MSC_Write(phost, 0, block_no, (uint8_t *)buffer, buffer_size/host_msc_get_block_size());
    if (status != USBH_OK) {
        amk_printf("USB Host failed to write at:%ld, size:%ld, error:%d\n", block_no, buffer_size, status);
        return AMK_ERROR;
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