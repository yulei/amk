/**
 * @file flash_stm32.c
 *
 */

#include <string.h>

#include "flash.h"
#include "generic_hal.h"
#include "amk_printf.h"

#if defined(STM32F103xB)
#error "eeconfig flash: stm32f103 was not supported currently"
//#define FLASH_BASE_ADDRESS      0x801FC00
//#define FLASH_TOTAL_SIZE        0x400
//#define FLASH_INVALID_DATA      0xFFFFFFFF
//#define FLASH_INVALID_ADDRESS   0xFFFFFFFF
//#define FLASH_PAGE_NUM          1
#elif defined(STM32G431xx)
#define FLASH_BASE_ADDRESS      0x800A000
#define FLASH_TOTAL_SIZE        0x6000
#define FLASH_RW_SIZE           8
#elif defined(STM32L432xx)
#define FLASH_BASE_ADDRESS      0x800A000
#define FLASH_TOTAL_SIZE        0x6000
#define FLASH_RW_SIZE           8

#elif defined(STM32F411xE) || defined(STM32F722xx) || defined(STM32F405xx) || defined(STM32F446xx) || defined(STM32F401xC)
#define FLASH_BASE_ADDRESS      0x8010000
#define FLASH_TOTAL_SIZE        0x10000
#define FLASH_SECTOR_ID         FLASH_SECTOR_4
#define FLASH_SECTOR_NUM        1
#define FLASH_RW_SIZE           4
#else
#error "eeconfig flash: unsupported mcu"
#endif

uint32_t flash_base_address = FLASH_BASE_ADDRESS;
uint32_t flash_total_size = FLASH_TOTAL_SIZE;
uint32_t flash_unit_size = FLASH_RW_SIZE;

void flash_unlock(void)
{
    HAL_FLASH_Unlock();
#if defined(STM32G431xx) || defined(STM32L432xx)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
#endif
}

void flash_lock(void)
{
    HAL_FLASH_Lock();
}

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
#if defined(STM32G431xx) || defined(STM32L432xx)
    uint64_t d = *((__IO uint64_t*)(address));
    uint32_t value = (uint32_t)(d&0x00000000FFFFFFFF);
#else
    uint32_t value = *((__IO uint32_t*)(address));
#endif
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
#if defined(STM32G431xx) || defined(STM32L432xx)
    uint64_t d = (((uint64_t)value)<<32) | value;
    HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, d);
#else
    HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value);
#endif
    if (status != HAL_OK) {
        amk_printf("Failed to programe word: addr=%x, data=%x, error=%d\n", address, value, HAL_FLASH_GetError());
        ret = false;
    }
    flash_lock();
    return ret;
}

void flash_erase_pages(void)
{
    uint32_t error = 0;
    FLASH_EraseInitTypeDef erase;
    memset(&erase, 0, sizeof(erase));
    flash_unlock();
#if defined(STM32F103xB)
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks = FLASH_BANK_1;
    erase.PageAddress = FLASH_BASE_ADDRESS;
    erase.NbPages = FLASH_PAGE_NUM;
#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F446xx) || defined(STM32F401xC)
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks = FLASH_BANK_1;
    erase.Sector = FLASH_SECTOR_ID;
    erase.NbSectors = FLASH_SECTOR_NUM;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#elif defined(STM32F722xx)
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_ID;
    erase.NbSectors = FLASH_SECTOR_NUM;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#elif defined(STM32G431xx) || defined(STM32L432xx)
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Page = (FLASH_BASE_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;;
    erase.NbPages = (FLASH_TOTAL_SIZE) / FLASH_PAGE_SIZE;
#else
    #error "Flash Erase: unsupported mcu"
#endif
    HAL_FLASHEx_Erase(&erase, &error);
    flash_lock();
    amk_printf("Flash erase page, error=%d\n", error);
}
