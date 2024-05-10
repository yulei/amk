/**
 * @file eeconfig_flash.c
 *  eeprom emulation on flash
 *
 */

#include <stddef.h>
#include <string.h>
#include "flash.h"
#include "eeconfig.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#define FLASH_INVALID_DATA      0xFFFFFFFF
#define FLASH_INVALID_ADDRESS   0xFFFFFFFF

#define EEPROM_INVALID_ADDRESS  0xFFFF
#define EEPROM_EMPTY_VALUE      0x00
#define FLASH_EMPTY_VALUE       0xFF
#define IS_VALID_ADDR(x)        ((x) >= 0 && (x) < AMK_EEPROM_SIZE)

static uint8_t buffer[AMK_EEPROM_SIZE];

static void fee_erase(void);
static void fee_backup(void);
static void fee_restore(void);
static uint32_t fee_find_valid_address(void);
static bool fee_write(uintptr_t address, uint8_t data);
static uint8_t fee_read(uintptr_t address);

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

    uint16_t magic = eeprom_read_word(EECONFIG_MAGIC);
    amk_printf("EEPROM: magic is 0x%x\n", magic);
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
    uint32_t begin = flash_base_address;
    uint32_t end = begin + flash_total_size;
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
        begin += flash_unit_size;
    }
}

static void fee_restore(void)
{
    uint32_t cur = flash_base_address;
    for (uint8_t i = 0; i < AMK_EEPROM_SIZE; i++) {
        if (buffer[i] != EEPROM_EMPTY_VALUE) {
            flash_write(cur, i, buffer[i]);
            cur += flash_unit_size;
        }
    }
}

static uint32_t fee_find_valid_address(void)
{
    uint32_t begin = flash_base_address;
    uint32_t end = begin + flash_total_size;
    while( begin < end) {
        uint16_t addr = 0;
        uint16_t data = 0;
        flash_read(begin, &addr, &data);
        if (addr == EEPROM_INVALID_ADDRESS) {
            return begin;
        }
        begin += flash_unit_size;
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
