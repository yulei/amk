/**
 * @file eeconfig_flash.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "eeconfig.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#if defined(HC32F460xE)
// 64x8K sector, sector63 has reseved area
// use sector 56-62, 7x8K bytes
#define FLASH_BASE_ADDRESS      0x8070000
#define FLASH_TOTAL_SIZE        0xE000
#define FLASH_INVALID_DATA      0xFFFFFFFF
#define FLASH_INVALID_ADDRESS   0xFFFFFFFF
#define FLASH_SECTOR_SIZE       0x2000
#define FLASH_SECTOR_NUM        7
#else
#error "eeconfig flash: unsupported mcu"
#endif

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
        begin += 4;
    }
}

static void fee_restore(void)
{
    uint32_t cur = FLASH_BASE_ADDRESS;
    for (uint8_t i = 0; i < EEPROM_SIZE; i++) {
        if (buffer[i] != EEPROM_EMPTY_VALUE) {
            flash_write(cur, i, buffer[i]);
            cur += 4;
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
        begin += 4;
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
