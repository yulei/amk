/**
 * @file eeprom_fds.c
 * @brief fds base eeprom emulation
 *
 */

#include <string.h>
#include "eeconfig.h"
#include "fds.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrf_log.h"

#define EECONFIG_SIZE 64

APP_TIMER_DEF(m_eeprom_update_timer_id);        // timer for update the eeprom
#define EEPROM_UPDATE_DELAY APP_TIMER_TICKS(10) // timeout delay

#define EE_FILEID 0x6565                        //"ee"
#define EE_EEPROM_KEY 0x00AB                    // should in rage 0x0001-0xBFFF

static volatile bool ee_fds_initialized = false;
static volatile bool eeprom_dirty = false;

static void wait_for_fds_ready(void)
{
    while(!ee_fds_initialized) {
        app_sched_execute();
    }
}

__ALIGN(4) static uint8_t eeprom_buf[(EECONFIG_SIZE + 3) & (~3)];  // pad to word size

static fds_record_t ee_record = {
    .file_id = EE_FILEID,
    .key = EE_EEPROM_KEY,
    .data.p_data = &eeprom_buf[0],
    .data.length_words = sizeof(eeprom_buf)/sizeof(uint32_t),
};

static void fds_eeprom_update(void);
static void eeprom_update_timeout_handler(void *p_context);

static void ee_evt_handler(fds_evt_t const *p_evt)
{
    switch(p_evt->id) {
    case FDS_EVT_WRITE:
    case FDS_EVT_UPDATE:
        eeprom_dirty = false;
        break;
    case FDS_EVT_DEL_RECORD:
    case FDS_EVT_DEL_FILE:
        break;
    case FDS_EVT_GC:
        if (eeprom_dirty) {
            ret_code_t err_code;
            err_code = app_timer_start(m_eeprom_update_timer_id, EEPROM_UPDATE_DELAY, NULL);
            APP_ERROR_CHECK(err_code);
        }
        break;
    case FDS_EVT_INIT:
        if (p_evt->result == NRF_SUCCESS) {
            ee_fds_initialized = true;
            NRF_LOG_INFO("FDS initialized.");
        } else {
            NRF_LOG_INFO("Failed to initialized FDS, code: %d", p_evt->result);
        } break;
    default:
        break;
    }
}

void fds_eeprom_init(void)
{
    ret_code_t err_code;
    NRF_LOG_INFO("Initializing fds...");
    ee_fds_initialized = false;
    fds_register(ee_evt_handler);
    err_code = fds_init();
    APP_ERROR_CHECK(err_code);
    wait_for_fds_ready();

    err_code = app_timer_create(&m_eeprom_update_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                eeprom_update_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

static void eeprom_update_timeout_handler(void* p_context)
{
    if (!eeprom_dirty) return;

    fds_eeprom_update();
}

static void fds_eeprom_update(void)
{
    fds_record_desc_t desc = {0};
    fds_find_token_t token = {0};
    uint16_t key = EE_EEPROM_KEY;
    ret_code_t err_code = fds_record_find_by_key(key, &desc, &token);
    if (err_code == NRF_SUCCESS) {
        err_code  = fds_record_update(&desc, &ee_record);
        if (err_code  == FDS_ERR_NO_SPACE_IN_FLASH) {
            fds_gc();
        } else {
            APP_ERROR_CHECK(err_code);
        }
    } else {
        err_code = fds_record_write(&desc, &ee_record);
        if (err_code == FDS_ERR_NO_SPACE_IN_FLASH) {
            fds_gc();
        } else {
            APP_ERROR_CHECK(err_code);
        }
    }
}

void fds_eeprom_write_byte(uint16_t addr, uint8_t data)
{
    ret_code_t err_code;
    if (eeprom_buf[addr] == data) return;

    eeprom_buf[addr] = data;
    eeprom_dirty = true;
    err_code = app_timer_start(m_eeprom_update_timer_id, EEPROM_UPDATE_DELAY, NULL);
    APP_ERROR_CHECK(err_code);
}

uint8_t fds_eeprom_read_byte(uint16_t addr) { return eeprom_buf[addr]; }

/*****************************************************************************
 *  Wrap library in AVR style functions.
 *******************************************************************************/
uint8_t eeprom_read_byte(const uint8_t *Address) {
    const uint16_t p = (const uint32_t)Address;
    return fds_eeprom_read_byte(p);
}

void eeprom_write_byte(uint8_t *Address, uint8_t Value) {
    uint16_t p = (uint32_t)Address;
    fds_eeprom_write_byte(p, Value);
}

void eeprom_update_byte(uint8_t *Address, uint8_t Value) {
    uint16_t p = (uint32_t)Address;
    fds_eeprom_write_byte(p, Value);
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
