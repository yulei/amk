/**
 * @file flash_at32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
*/

#include <string.h>

#include "amk_hal.h"
#include "flash.h"
#include "amk_printf.h"

#if defined(AT32F405)
#define FLASH_BASE_ADDRESS      0x8010000
#define FLASH_TOTAL_SIZE        0x10000
#define FLASH_RW_SIZE           4
#else
#error "eeconfig flash: unsupported mcu"
#endif

uint32_t flash_base_address = FLASH_BASE_ADDRESS;
uint32_t flash_total_size = FLASH_TOTAL_SIZE;
uint32_t flash_unit_size = FLASH_RW_SIZE;

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
    uint32_t value = *((__IO uint32_t*)(address));

    *offset = (value >> 16) & 0xFFFF;
    uint16_t tmp = value & 0xFFFF;
    *data = ~tmp;
}

#define FLASH_FLAGS (FLASH_OBF_FLAG | FLASH_ODF_FLAG | FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG)

bool flash_write(uint32_t address, uint16_t offset, uint16_t data)
{
    bool ret = true;
    data = ~data;
    uint32_t value = (offset << 16) | data;

    flash_unlock();
    flash_flag_clear(FLASH_FLAGS);

    flash_status_type state = flash_word_program(address, value);

    if (state != FLASH_OPERATE_DONE) {
        amk_printf("Failed to programe word: addr=%x, data=%x, error=%d\n", address, value, state);
        ret = false;
    }
    flash_flag_clear(FLASH_FLAGS);
    flash_lock();

    return ret;
}

#define FLASH_PAGE_SIZE   (2*1024)

void flash_erase_pages(void)
{
    uint32_t total = FLASH_TOTAL_SIZE / FLASH_PAGE_SIZE;

    flash_unlock();
    flash_flag_clear(FLASH_FLAGS);

    uint32_t cur = FLASH_BASE_ADDRESS;

    for (int i = 0; i < total; i++) {
        flash_status_type state = flash_sector_erase(cur);
            
        if (state != FLASH_OPERATE_DONE) {
            amk_printf("flash erase failed: address=%d, error=%d\n", cur, state);
            flash_flag_clear(FLASH_FLAGS);
            break;
        } 

        cur += FLASH_PAGE_SIZE;
    }
    flash_flag_clear(FLASH_FLAGS);

    flash_lock();
}
