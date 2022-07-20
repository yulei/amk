/**
 * @file eeconfig_flash.c
 *  eeprom emulation on flash
 *
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "eeconfig.h"
#include "amk_eeprom.h"
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
#define FLASH_INVALID_DATA      0xFFFFFFFF
#define FLASH_INVALID_ADDRESS   0xFFFFFFFF
#define FLASH_RW_SIZE           8

#elif defined(STM32F411xE) || defined(STM32F722xx) || defined(STM32F405xx) || defined(STM32F446xx) || defined(STM32F401xC)
#define FLASH_BASE_ADDRESS      0x8010000
#define FLASH_TOTAL_SIZE        0x10000
#define FLASH_INVALID_DATA      0xFFFFFFFF
#define FLASH_INVALID_ADDRESS   0xFFFFFFFF
#define FLASH_SECTOR_ID         FLASH_SECTOR_4
#define FLASH_SECTOR_NUM        1
#define FLASH_RW_SIZE           4
#else
#error "eeconfig flash: unsupported mcu"
#endif

//#define EEPROM_SIZE             (64+MATRIX_ROWS*MATRIX_COLS*2*4)
//#define EEPROM_KEYMAP_START     (64)
#define EEPROM_INVALID_ADDRESS  0xFFFF
#define EEPROM_EMPTY_VALUE      0x00
#define FLASH_EMPTY_VALUE       0xFF
#define IS_VALID_ADDR(x)        ((x) >= 0 && (x) < EEPROM_SIZE)

static uint8_t buffer[EEPROM_SIZE];

static void fee_erase(void);
static void fee_backup(void);
static void fee_restore(void);
static uint32_t fee_find_valid_address(void);
static bool fee_write(uintptr_t address, uint8_t data);
static uint8_t fee_read(uintptr_t address);

static void flash_unlock(void);
static void flash_lock(void);
static bool flash_write(uint32_t address, uint16_t offset, uint16_t data);
static void flash_read(uint32_t address, uint16_t *offset, uint16_t *data);
static void flash_erase_pages(void);

uint8_t eeprom_read_byte(const uint8_t *addr)
{
    uintptr_t offset = (uintptr_t)addr;
    return fee_read(offset);
}

void eeprom_write_byte(uint8_t *addr, uint8_t value)
{
    uintptr_t offset = (uintptr_t)addr;
    fee_write(offset, value);
}

void eeprom_update_byte(uint8_t *addr, uint8_t value)
{
    eeprom_write_byte(addr, value);
}

uint16_t eeprom_read_word(const uint16_t *addr)
{
    const uint8_t *p = (const uint8_t *)addr;
    return eeprom_read_byte(p) | (eeprom_read_byte(p + 1) << 8);
}

void eeprom_write_word(uint16_t *addr, uint16_t value)
{
    uint8_t *p = (uint8_t *)addr;
    eeprom_write_byte(p++,  value >> 0);
    eeprom_write_byte(p,    value >> 8);
}

void eeprom_update_word(uint16_t *addr, uint16_t value)
{
    eeprom_write_word(addr, value);
}

uint32_t eeprom_read_dword(const uint32_t *addr)
{
    const uint8_t *p = (const uint8_t *)addr;
    return eeprom_read_byte(p) 
        | (eeprom_read_byte(p + 1) << 8)
        | (eeprom_read_byte(p + 2) << 16)
        | (eeprom_read_byte(p + 3) << 24);
}

void eeprom_write_dword(uint32_t *addr, uint32_t value)
{
    uint8_t *p = (uint8_t *)addr;
    eeprom_write_byte(p++,  value >> 0);
    eeprom_write_byte(p++,  value >> 8);
    eeprom_write_byte(p++,  value >> 16);
    eeprom_write_byte(p,    value >> 24);
}

void eeprom_update_dword(uint32_t *addr, uint32_t value)
{
    eeprom_write_dword(addr, value);
}

void eeprom_read_block(void *buf, const void *addr, size_t len)
{
    const uint8_t   *p    = (const uint8_t *)addr;
    uint8_t         *dest = (uint8_t *)buf;
    while (len--) {
        *dest++ = eeprom_read_byte(p++);
    }
}

void eeprom_write_block(const void *buf, void *addr, size_t len)
{
    uint8_t         *p      = (uint8_t *)addr;
    const uint8_t   *src    = (const uint8_t *)buf;
    while (len--) {
        eeprom_write_byte(p++, *src++);
    }
}

void eeprom_update_block(const void *buf, void *addr, size_t len)
{
    eeprom_write_block(buf, addr, len);
}

// cleanup eeconfig before initialization
void eeconfig_prepare(void)
{
    fee_erase();
}

//==============================================
// flash eeprom operation
//==============================================
void fee_init(void)
{
    // initialize DataBuf
    fee_backup();
}


static void fee_erase(void)
{
    // erase all flash pages
    flash_erase_pages();

    // reset the content of the buffer
    //memset(&buffer[0], FLASH_EMPTY_VALUE, sizeof(buffer));
    memset(&buffer[0], EEPROM_EMPTY_VALUE, sizeof(buffer));
}

static void fee_backup(void)
{
    uint32_t begin = FLASH_BASE_ADDRESS;
    uint32_t end = begin + FLASH_TOTAL_SIZE;
    //memset(&buffer[0], FLASH_EMPTY_VALUE, sizeof(buffer));
    memset(&buffer[0], EEPROM_EMPTY_VALUE, sizeof(buffer));
    while (begin < end) {
        uint16_t addr = 0;
        uint16_t data = 0;
        flash_read(begin, &addr, &data);
        if (IS_VALID_ADDR(addr)) {
            buffer[addr] = data;
        } else {
            // we reached the end of valid data
            break;
        }
        begin += FLASH_RW_SIZE;
    }
}

static void fee_restore(void)
{
    uint32_t cur = FLASH_BASE_ADDRESS;
    for (uint8_t i = 0; i < EEPROM_SIZE; i++) {
        if (buffer[i] != EEPROM_EMPTY_VALUE) {
            flash_write(cur, i, buffer[i]);
            cur += FLASH_RW_SIZE;
        }
    }
}

static uint32_t fee_find_valid_address(void)
{
    uint32_t begin = FLASH_BASE_ADDRESS;
    uint32_t end = begin + FLASH_TOTAL_SIZE;
    while( begin < end) {
        uint16_t addr = 0;
        uint16_t data = 0;
        flash_read(begin, &addr, &data);
        if (addr == EEPROM_INVALID_ADDRESS) {
            return begin;
        }
        begin += FLASH_RW_SIZE;
    }
    return FLASH_INVALID_ADDRESS;
}

bool fee_write(uintptr_t address, uint8_t data)
{
    // exit if not a valid address
    if (!IS_VALID_ADDR(address)) { return false; }

// we are sure the address will not be out of bound
#pragma GCC diagnostic ignored "-Warray-bounds"
    // same value, do not need program
    if (buffer[address] == data) { return true; }

    // find the address can be written
    uint32_t flash_addr = fee_find_valid_address();
    buffer[address] = data;
#pragma GCC diagnostic pop
    if (flash_addr == FLASH_INVALID_ADDRESS) {
        // EEPROM is full, need to erase and reprogramming
        flash_erase_pages();
        fee_restore();
    } else {
        flash_write(flash_addr, address, data);
    }

    return true;
}

uint8_t fee_read(uintptr_t address)
{
    if (!IS_VALID_ADDR(address)) {
        return EEPROM_EMPTY_VALUE;
    }

    // Get Byte from caches
#pragma GCC diagnostic ignored "-Warray-bounds"
    return buffer[address];
#pragma GCC diagnostic pop
}

//==============================================
// flash hardware operation
//==============================================
#include "generic_hal.h"
void flash_unlock(void)
{
    HAL_FLASH_Unlock();
#ifdef STM32G431xx
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
#endif
}

void flash_lock(void)
{
    HAL_FLASH_Lock();
}

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
#ifdef STM32G431xx
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
#if defined(STM32G431xx)
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
#elif defined(STM32G431xx)
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
