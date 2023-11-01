/**
 * @file flash_stm32.c
 *
 */

#include <string.h>

#include "amk_hal.h"
#include "flash.h"
#include "amk_printf.h"

#if defined(HC32F460xE)
// 64x8K sector, sector63 has reseved area
// use sector 56-62, 7x8K bytes
#define FLASH_BASE_ADDRESS      0x0070000
#define FLASH_TOTAL_SIZE        0xE000
#define FLASH_RW_SIZE           4
#define FLASH_SECTOR_SIZE       0x2000
#define FLASH_SECTOR_NUM        7
#else
#error "eeconfig flash: unsupported mcu"
#endif

uint32_t flash_base_address = FLASH_BASE_ADDRESS;
uint32_t flash_total_size = FLASH_TOTAL_SIZE;
uint32_t flash_unit_size = FLASH_RW_SIZE;

void flash_unlock(void)
{
    EFM_Unlock();
    EFM_FlashCmd(Enable);
    while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY)) {
        ;
    }
}

void flash_lock(void)
{
    EFM_Lock();
}

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
    uint32_t value = *((__IO uint32_t*)(address));
    *offset = (value >> 16) & 0xFFFF;
    uint16_t tmp = value & 0xFFFF;
    *data = ~tmp;
}

bool flash_write(uint32_t address, uint16_t offset, uint16_t data)
{
    bool ret = true;
    data = ~data;
    uint32_t value = (offset << 16) | data;

    flash_unlock();
    en_result_t result = EFM_SingleProgram(address, value);
    if (result != Ok) {
        amk_printf("Failed to programe word: addr=%x, data=%x, error=%d\n", address, value, result);
        ret = false;
    }
    flash_lock();
    return ret;
}

void flash_erase_pages(void)
{
    flash_unlock();
    uint32_t addr = FLASH_BASE_ADDRESS; 
    for (int i = 0; i < FLASH_SECTOR_NUM; i++) {
        en_result_t res = EFM_SectorErase(addr+i*FLASH_SECTOR_SIZE);
        if (res != Ok) {
            amk_printf("Flash erase page: address=0x%x, error=%d\n", addr+i*FLASH_SECTOR_SIZE, res);
        }
    }
    flash_lock();
}
