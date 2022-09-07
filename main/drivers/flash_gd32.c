/**
 * @file flash_gd32.c
 *
 */

#include <string.h>

#include "flash.h"
#include "generic_hal.h"
#include "amk_printf.h"

#if defined(GD32E505)
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
    fmc_unlock();
}

void flash_lock(void)
{
    fmc_lock();
}

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
    uint32_t value = *((__IO uint32_t*)(address));

    *offset = (value >> 16) & 0xFFFF;
    uint16_t tmp = value & 0xFFFF;
    *data = ~tmp;
}

#define FLASH_FLAGS (FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR)

bool flash_write(uint32_t address, uint16_t offset, uint16_t data)
{
    bool ret = true;
    data = ~data;
    uint32_t value = (offset << 16) | data;

    flash_unlock();
    fmc_flag_clear(FLASH_FLAGS);

    fmc_state_enum state = fmc_word_program(address, value);
    if (state != FMC_READY) {
        amk_printf("Failed to programe word: addr=%x, data=%x, error=%d\n", address, value, state);
        ret = false;
    }
    fmc_flag_clear(FLASH_FLAGS);
    flash_lock();

    return ret;
}

#define FLASH_PAGE_SIZE   (8*1024)

void flash_erase_pages(void)
{
    uint32_t total = FLASH_TOTAL_SIZE / FLASH_PAGE_SIZE;

    fmc_unlock();
    fmc_flag_clear(FLASH_FLAGS);
    uint32_t cur = FLASH_BASE_ADDRESS;

    for (int i = 0; i < total; i++) {
        fmc_state_enum state = fmc_page_erase(cur);
            
        if (state != FMC_READY) {
            amk_printf("flash erase failed: address=%d, error=%d\n", cur, state);
            break;
        } 

        fmc_flag_clear(FLASH_FLAGS);
        cur += FLASH_PAGE_SIZE;
    }

    fmc_lock();
}
