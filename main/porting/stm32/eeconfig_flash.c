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

#if defined(STM32F103xB)
#define FLASH_BASE_ADDRESS      0x801FC00
#define FLASH_TOTAL_SIZE        0x400
#define FLASH_INVALID_DATA      0xFFFFFFFF
#define FLASH_INVALID_ADDRESS   0xFFFFFFFF
#define FLASH_PAGE_NUM          1
#elif defined(STM32F411xE) || defined(STM32F722xx) || defined(STM32F405xx)
#define FLASH_BASE_ADDRESS      0x8060000
#define FLASH_TOTAL_SIZE        0x20000
#define FLASH_INVALID_DATA      0xFFFFFFFF
#define FLASH_INVALID_ADDRESS   0xFFFFFFFF
#define FLASH_SECTOR_ID         FLASH_SECTOR_7
#define FLASH_SECTOR_NUM        1
#else
#error "eeconfig: unsupported mcu"
#endif

#define EEPROM_SIZE             (64+MATRIX_ROWS*MATRIX_COLS*2*4)
#define EEPROM_KEYMAP_START     (64)
#define EEPROM_INVALID_ADDRESS  0xFFFF
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

//=======================================================
// TMK functions
//=======================================================
__attribute__((weak))
void hook_eeconfig_init(void) {}

void eeconfig_init(void)
{
    fee_erase();
    eeprom_write_word(EECONFIG_MAGIC,           EECONFIG_MAGIC_NUMBER);
    eeprom_write_byte(EECONFIG_DEBUG,           0);
    eeprom_write_byte(EECONFIG_DEFAULT_LAYER,   0);
    eeprom_write_byte(EECONFIG_KEYMAP,          0);
    eeprom_write_byte(EECONFIG_MOUSEKEY_ACCEL,  0);

#ifdef RGB_EFFECTS_ENABLE
    extern void effects_update_default(void);
    effects_update_default();
#endif
#ifdef RGB_MATRIX_ENABLE
    extern void rgb_matrix_update_default(void);
    rgb_matrix_update_default();
#endif
    hook_eeconfig_init();
}

void eeconfig_enable(void)
{
    eeprom_write_word(EECONFIG_MAGIC, EECONFIG_MAGIC_NUMBER);
}

void eeconfig_disable(void)
{
    eeprom_write_word(EECONFIG_MAGIC, 0xFFFF);
}

bool eeconfig_is_enabled(void)
{
    return (eeprom_read_word(EECONFIG_MAGIC) == EECONFIG_MAGIC_NUMBER);
}

uint8_t eeconfig_read_debug(void)      { return eeprom_read_byte(EECONFIG_DEBUG); }
void eeconfig_write_debug(uint8_t val) { eeprom_write_byte(EECONFIG_DEBUG, val); }

uint8_t eeconfig_read_default_layer(void)      { return eeprom_read_byte(EECONFIG_DEFAULT_LAYER); }
void eeconfig_write_default_layer(uint8_t val) { eeprom_write_byte(EECONFIG_DEFAULT_LAYER, val); }

uint8_t eeconfig_read_keymap(void)      { return eeprom_read_byte(EECONFIG_KEYMAP); }
void eeconfig_write_keymap(uint8_t val) { eeprom_write_byte(EECONFIG_KEYMAP, val); }

//==============================================
// flash eeprom operation
//==============================================
void fee_init(void)
{
    // unlock flash
    flash_unlock();

    // initialize DataBuf
    fee_backup();
}


static void fee_erase(void)
{
    // erase all flash pages
    flash_erase_pages();

    // reset the content of the buffer
    memset(&buffer[0], FLASH_EMPTY_VALUE, sizeof(buffer));
}

static void fee_backup(void)
{
    uint32_t begin = FLASH_BASE_ADDRESS;
    uint32_t end = begin + FLASH_TOTAL_SIZE;
    memset(&buffer[0], FLASH_EMPTY_VALUE, sizeof(buffer));
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
        if (buffer[i] != FLASH_EMPTY_VALUE) {
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
        return FLASH_EMPTY_VALUE;
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
}

void flash_read(uint32_t address, uint16_t* offset, uint16_t* data)
{
    uint32_t value = *((__IO uint32_t*)(address));
    *offset = (value >> 16) & 0xFFFF;
    *data = (value >> 0) & 0xFFFF;
}

bool flash_write(uint32_t address, uint16_t offset, uint16_t data)
{
    uint32_t value = (offset << 16) | data;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value);
    return false;
}

void flash_erase_pages(void)
{
#if defined(STM32F103xB)
    FLASH_EraseInitTypeDef erase;
    memset(&erase, 0, sizeof(erase));
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks = FLASH_BANK_1;
    erase.PageAddress = FLASH_BASE_ADDRESS;
    erase.NbPages = FLASH_PAGE_NUM;
    uint32_t error = 0;
    HAL_FLASHEx_Erase(&erase, &error);
#elif defined(STM32F411xE) || defined(STM32F405xx)
    FLASH_EraseInitTypeDef erase;
    memset(&erase, 0, sizeof(erase));
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks = FLASH_BANK_1;
    erase.Sector = FLASH_SECTOR_ID;
    erase.NbSectors = FLASH_SECTOR_NUM;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    uint32_t error = 0;
    HAL_FLASHEx_Erase(&erase, &error);
#elif defined(STM32F722xx)
    FLASH_EraseInitTypeDef erase;
    memset(&erase, 0, sizeof(erase));
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_ID;
    erase.NbSectors = FLASH_SECTOR_NUM;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    uint32_t error = 0;
    HAL_FLASHEx_Erase(&erase, &error);
#else
#error "Flash Erase: unsupported mcu"
#endif
}

//==================================================
// flash store for keymaps
//==================================================

void flash_store_write(uint8_t key, const void* data, size_t size)
{
    uint32_t start = EEPROM_KEYMAP_START+key*MATRIX_ROWS*MATRIX_COLS*2;
    const uint8_t* p = (const uint8_t*)data;
    for(int i = 0; i < size; i++) {
        fee_write(start+i, p[i]);
    }
}

size_t flash_store_read(uint8_t key, void* data, size_t size)
{
    uint32_t start = EEPROM_KEYMAP_START+key*MATRIX_ROWS*MATRIX_COLS*2;
    uint8_t* p = (uint8_t*)data;
    for(int i = 0; i < size; i++) {
        p[i] = fee_read(start+i);
    }

    return size;
}