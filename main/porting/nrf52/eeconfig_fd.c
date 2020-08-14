/**
 * eeconfig_fd.c
 */

#include "eeconfig_fd.h"
#include "ble_config.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"

//TMK
#include "eeconfig.h"

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 64
#endif

#define EEPROM_EMPTY_BYTE       0xFF
#define EEPROM_EMPTY_HALFWORD   0xFFFF
#define EEPROM_EMPTY_WORD       0xFFFFFFFF

#define EEPROM_DATA(x)          ((x)&0xFFFF)                // eeprom data
#define EEPROM_ADDR(x)          (((x)>>16)&0xFFFF)          // eeprom address

#define EEPROM_COMPOSE(addr, data)  (((addr)<<16)|(data))

#define IS_VALID_ADDRESS(x) ((x) < EEPROM_SIZE)

extern void idle_state_handle(void);

static uint8_t eeprom_buf[EEPROM_SIZE];
static void eeprom_backup(void);
static ret_code_t eeprom_restore(void);
static uint32_t eeprom_find_valid_address(void);
static void eeprom_clear(void);
static void wait_for_flash_ready(nrf_fstorage_t const *p_fstorage);
static void eeprom_evt_handler(nrf_fstorage_evt_t *p_evt);

// PM used fds internally, so we select the flash page which was not used by the fds.
// while using NRF52832, it has 128 pages, each page has 1024 word
#define EEPROM_START_ADDR   ((128-FDS_VIRTUAL_PAGES-1)*1024*4)
#define EEPROM_END_ADDR     ((128-FDS_VIRTUAL_PAGES)*1024*4)

NRF_FSTORAGE_DEF(nrf_fstorage_t eeprom_storage) = {
    .evt_handler    = eeprom_evt_handler,
    .start_addr     = EEPROM_START_ADDR,
    .end_addr       = EEPROM_END_ADDR,
};

static void eeprom_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS) {
        NRF_LOG_INFO("FStorage operation failed --> event:%d, result:%d", p_evt->id, p_evt->result);
        return;
    }

    switch (p_evt->id) {
    case NRF_FSTORAGE_EVT_WRITE_RESULT: {
            NRF_LOG_INFO("FStorage: wrote %d bytes at address 0x%x.", p_evt->len, p_evt->addr);
        } break;

    case NRF_FSTORAGE_EVT_ERASE_RESULT: {
            NRF_LOG_INFO("FStorage: erased %d page from address 0x%x.", p_evt->len, p_evt->addr);
        } break;

    default:
        break;
    }
}

void fd_eeprom_init(void)
{
    ret_code_t rc = nrf_fstorage_init(&eeprom_storage, &nrf_fstorage_sd, NULL);
    APP_ERROR_CHECK(rc);

    eeprom_backup();
}

void fd_eeprom_write_byte(uint16_t addr, uint8_t data)
{
    // exit if not a valid address
    if (!IS_VALID_ADDRESS(addr)) {
        NRF_LOG_WARNING("FStorage, invalid eeprom address for writing: %d", addr);
        return;
    }

    // same value, do not need program
    if (eeprom_buf[addr] == data) {
        return;
    }

    eeprom_buf[addr] = data;
    uint32_t valid_addr = eeprom_find_valid_address();
    if (valid_addr == EEPROM_EMPTY_WORD) {
        eeprom_clear();
        eeprom_restore();
    } else {
        uint32_t valid_data = EEPROM_COMPOSE(addr, data);
        ret_code_t rc = nrf_fstorage_write(&eeprom_storage, valid_addr, &valid_data, sizeof(valid_data), NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&eeprom_storage);
    }
}

uint8_t fd_eeprom_read_byte(uint16_t addr)
{
    if (!IS_VALID_ADDRESS(addr)) {
        NRF_LOG_WARNING("FStorage, invalid eeprom address for reading: %d", addr);
        return EEPROM_EMPTY_BYTE;
    }

    return eeprom_buf[addr];
}

// helper function
void eeprom_backup(void)
{
    uint32_t begin = EEPROM_START_ADDR;
    uint32_t end = EEPROM_END_ADDR;
    memset(&eeprom_buf[0], EEPROM_EMPTY_BYTE, sizeof(eeprom_buf));
    while( begin < end) {
        uint32_t data = 0;
        nrf_fstorage_read(&eeprom_storage, begin, &data, sizeof(data));
        uint16_t addr = EEPROM_ADDR(data);
        if (IS_VALID_ADDRESS(addr)) {
            eeprom_buf[addr] = EEPROM_DATA(data);
        } else if( addr == EEPROM_EMPTY_HALFWORD) {
            // we reached the end of valid data
            break;
        }
        begin += 4;
    }
}

// restore buffered data to the empty page
ret_code_t eeprom_restore(void)
{
    ret_code_t rc = nrf_fstorage_write(&eeprom_storage, EEPROM_START_ADDR, eeprom_buf, EEPROM_SIZE, NULL);
    wait_for_flash_ready(&eeprom_storage);
    return rc;
}

// find an empty place for programming
uint32_t eeprom_find_valid_address(void)
{
    uint32_t begin = EEPROM_START_ADDR;
    uint32_t end = EEPROM_END_ADDR;
    while( begin < end) {
        uint32_t data = 0;
        nrf_fstorage_read(&eeprom_storage, begin, &data, sizeof(data));
        if (data == EEPROM_EMPTY_WORD) {
            return begin;
        }
        begin += 4;
    }
    return EEPROM_EMPTY_WORD;
}

void eeprom_clear(void)
{
    ret_code_t rc = nrf_fstorage_erase(&eeprom_storage, EEPROM_START_ADDR, EEPROM_END_ADDR - EEPROM_START_ADDR, NULL);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&eeprom_storage);
}

void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage)) {
        idle_state_handle();
    }
}
/*****************************************************************************
 *  Wrap library in AVR style functions.
 *******************************************************************************/
uint8_t eeprom_read_byte(const uint8_t *Address) {
    const uint16_t p = (const uint32_t)Address;
    return fd_eeprom_read_byte(p);
}

void eeprom_write_byte(uint8_t *Address, uint8_t Value) {
    uint16_t p = (uint32_t)Address;
    fd_eeprom_write_byte(p, Value);
}

void eeprom_update_byte(uint8_t *Address, uint8_t Value) {
    uint16_t p = (uint32_t)Address;
    fd_eeprom_write_byte(p, Value);
}

uint16_t eeprom_read_word(const uint16_t *Address) {
    const uint8_t *p = (const uint8_t *)Address;
    return eeprom_read_byte(p) | (eeprom_read_byte(p + 1) << 8);
}

void eeprom_write_word(uint16_t *Address, uint16_t Value) {
    uint8_t *p = (uint8_t *)Address;
    eeprom_write_byte(p, (uint8_t)Value);
    eeprom_write_byte(p + 1, (uint8_t)(Value >> 8));
}

void eeprom_update_word(uint16_t *Address, uint16_t Value) {
    uint8_t *p = (uint8_t *)Address;
    eeprom_write_byte(p, (uint8_t)Value);
    eeprom_write_byte(p + 1, (uint8_t)(Value >> 8));
}

uint32_t eeprom_read_dword(const uint32_t *Address) {
    const uint8_t *p = (const uint8_t *)Address;
    return eeprom_read_byte(p) | (eeprom_read_byte(p + 1) << 8) | (eeprom_read_byte(p + 2) << 16) | (eeprom_read_byte(p + 3) << 24);
}

void eeprom_write_dword(uint32_t *Address, uint32_t Value) {
    uint8_t *p = (uint8_t *)Address;
    eeprom_write_byte(p, (uint8_t)Value);
    eeprom_write_byte(p + 1, (uint8_t)(Value >> 8));
    eeprom_write_byte(p + 2, (uint8_t)(Value >> 16));
    eeprom_write_byte(p + 3, (uint8_t)(Value >> 24));
}

void eeprom_update_dword(uint32_t *Address, uint32_t Value) {
    uint8_t *p = (uint8_t *)Address;
    uint32_t existingValue = eeprom_read_byte(p) | (eeprom_read_byte(p + 1) << 8) | (eeprom_read_byte(p + 2) << 16) | (eeprom_read_byte(p + 3) << 24);
    if (Value != existingValue) {
        eeprom_write_byte(p, (uint8_t)Value);
        eeprom_write_byte(p + 1, (uint8_t)(Value >> 8));
        eeprom_write_byte(p + 2, (uint8_t)(Value >> 16));
        eeprom_write_byte(p + 3, (uint8_t)(Value >> 24));
    }
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    const uint8_t *p    = (const uint8_t *)addr;
    uint8_t *      dest = (uint8_t *)buf;
    while (len--) {
        *dest++ = eeprom_read_byte(p++);
    }
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    uint8_t *      p   = (uint8_t *)addr;
    const uint8_t *src = (const uint8_t *)buf;
    while (len--) {
        eeprom_write_byte(p++, *src++);
    }
}

void eeprom_update_block(const void *buf, void *addr, size_t len) {
    uint8_t *      p   = (uint8_t *)addr;
    const uint8_t *src = (const uint8_t *)buf;
    while (len--) {
        eeprom_write_byte(p++, *src++);
    }
}

/*****************/
/* TMK functions */
/*****************/

void eeconfig_init(void)
{
    //fd_eeprom_init();

    eeprom_write_word(EECONFIG_MAGIC, EECONFIG_MAGIC_NUMBER);
    eeprom_write_byte(EECONFIG_DEBUG,          0);
    eeprom_write_byte(EECONFIG_DEFAULT_LAYER,  0);
    eeprom_write_byte(EECONFIG_KEYMAP,         0);
    eeprom_write_byte(EECONFIG_MOUSEKEY_ACCEL, 0);
#ifdef BACKLIGHT_ENABLE
    eeprom_write_byte(EECONFIG_BACKLIGHT,      0);
#endif
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

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight(void)      { return eeprom_read_byte(EECONFIG_BACKLIGHT); }
void eeconfig_write_backlight(uint8_t val) { eeprom_write_byte(EECONFIG_BACKLIGHT, val); }
#endif
