/**
 * @file flash_ch32.c
 *
 */

#include <string.h>

#include "amk_hal.h"
#include "flash.h"
#include "amk_printf.h"

#if defined(CH32V307)
#define FLASH_BASE_ADDRESS      0x800A000
#define FLASH_TOTAL_SIZE        0x6000
#define FLASH_RW_SIZE           4
#else
#error "eeconfig flash: unsupported mcu"
#endif

uint32_t flash_base_address = FLASH_BASE_ADDRESS;
uint32_t flash_total_size = FLASH_TOTAL_SIZE;
uint32_t flash_unit_size = FLASH_RW_SIZE;

void flash_unlock(void)
{
    FLASH_Unlock();
}

void flash_lock(void)
{
    FLASH_Lock();
}

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
    uint32_t value = *((__IO uint32_t*)(address));

    *offset = (value >> 16) & 0xFFFF;
    uint16_t tmp = value & 0xFFFF;
    *data = ~tmp;
}

#define FLASH_FLAGS (FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR)

bool flash_write(uint32_t address, uint16_t offset, uint16_t data)
{
    bool ret = true;
    data = ~data;
    uint32_t value = (offset << 16) | data;

    flash_unlock();
    FLASH_ClearFlag(FLASH_FLAGS);

    FLASH_Status state = FLASH_ProgramWord(address, value);
    if (state != FLASH_COMPLETE) {
        amk_printf("Failed to programe word: addr=%x, data=%x, error=%d\n", address, value, state);
        ret = false;
    }
    FLASH_ClearFlag(FLASH_FLAGS);
    flash_lock();

    return ret;
}

#define FLASH_PAGE_SIZE   (4*1024)

void flash_erase_pages(void)
{
    uint32_t total = FLASH_TOTAL_SIZE / FLASH_PAGE_SIZE;

    flash_unlock();
    FLASH_ClearFlag(FLASH_FLAGS);
    uint32_t cur = FLASH_BASE_ADDRESS;

    for (int i = 0; i < total; i++) {
        FLASH_Status state = FLASH_ErasePage(cur);
            
        if (state != FLASH_COMPLETE) {
            amk_printf("flash erase failed: address=%d, error=%d\n", cur, state);
            break;
        } 

        FLASH_ClearFlag(FLASH_FLAGS);
        cur += FLASH_PAGE_SIZE;
    }

    flash_unlock();
}
