/**
 * @file amk_action_util.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "host.h"
#include "timer.h"
#include "util.h"
#include "amk_action_util.h"
#include "amk_printf.h"

#ifndef AMK_ACTION_DEBUG
#define AMK_ACTION_DEBUG 0
#endif

#if AMK_ACTION_DEBUG
#define action_debug  amk_printf
#else
#define action_debug(...)
#endif

#if !defined(NKRO_AUTO_ENABLE)
#error "auto nkro not defined"
#endif

static inline void add_key_byte(uint8_t code);
static inline void del_key_byte(uint8_t code);
static inline void add_key_bit(uint8_t code);
static inline void del_key_bit(uint8_t code);

static uint8_t real_mods = 0;
static uint8_t weak_mods = 0;

amk_report_t amk_report = {.nkro_mode = false};

static int8_t oneshot_mods = 0;
#if (defined(ONESHOT_TIMEOUT) && (ONESHOT_TIMEOUT > 0))
static int16_t oneshot_time = 0;
#endif

void send_keyboard_report(void) {
    if (amk_report.nkro_mode) {
        amk_report.nkro.mods  = real_mods;
        amk_report.nkro.mods |= weak_mods;
    } else {
        amk_report.std.mods  = real_mods;
        amk_report.std.mods |= weak_mods;
    }

    if (oneshot_mods) {
#if (defined(ONESHOT_TIMEOUT) && (ONESHOT_TIMEOUT > 0))
        if (TIMER_DIFF_16(timer_read(), oneshot_time) >= ONESHOT_TIMEOUT) {
            dprintf("Oneshot: timeout\n");
            clear_oneshot_mods();
        }
#endif
        if (amk_report.nkro_mode) {
            amk_report.nkro.mods |= oneshot_mods;
        } else {
            amk_report.std.mods |= oneshot_mods;
        }

        if (has_anykey()) {
            clear_oneshot_mods();
        }
    }
    
    host_keyboard_send((report_keyboard_t*)&amk_report);
}

void add_key(uint8_t key)
{
    if (amk_report.nkro_mode) {
        add_key_bit(key);
    } else {
        add_key_byte(key);
    }
}

void del_key(uint8_t key)
{
    if (amk_report.nkro_mode) {
        del_key_bit(key);
    } else {
        del_key_byte(key);
    }
}

void clear_keys(void)
{
    for (int8_t i = 1; i < AMK_NKRO_REPORT_SIZE; i++) {
        amk_report.nkro.raw[i] = 0;
    }
}


/* modifier */
uint8_t get_mods(void) { return real_mods; }
void add_mods(uint8_t mods) { real_mods |= mods; }
void del_mods(uint8_t mods) { real_mods &= ~mods; }
void set_mods(uint8_t mods) { real_mods = mods; }
void clear_mods(void) { real_mods = 0; }

/* weak modifier */
uint8_t get_weak_mods(void) { return weak_mods; }
void add_weak_mods(uint8_t mods) { weak_mods |= mods; }
void del_weak_mods(uint8_t mods) { weak_mods &= ~mods; }
void set_weak_mods(uint8_t mods) { weak_mods = mods; }
void clear_weak_mods(void) { weak_mods = 0; }

/* Oneshot modifier */
void set_oneshot_mods(uint8_t mods)
{
    oneshot_mods = mods;
#if (defined(ONESHOT_TIMEOUT) && (ONESHOT_TIMEOUT > 0))
    oneshot_time = timer_read();
#endif
}
void clear_oneshot_mods(void)
{
    oneshot_mods = 0;
#if (defined(ONESHOT_TIMEOUT) && (ONESHOT_TIMEOUT > 0))
    oneshot_time = 0;
#endif
}

/*
 * inspect keyboard state
 */
uint8_t has_anykey(void)
{
    uint8_t cnt = 0;
    if (amk_report.nkro_mode) {
        for (uint8_t i = 1; i < AMK_NKRO_REPORT_SIZE; i++) {
            if (amk_report.nkro.raw[i])
                cnt++;
        }
    } else {
        for (uint8_t i = 1; i < KEYBOARD_REPORT_SIZE; i++) {
            if (amk_report.std.raw[i])
                cnt++;
        }
    }
    return cnt;
}

uint8_t has_anymod(void)
{
    return bitpop(real_mods);
}

uint8_t get_first_key(void)
{
    if (amk_report.nkro_mode) {
        uint8_t i = 0;
        for (; i < AMK_NKRO_BITS_SIZE && !amk_report.nkro.bits[i]; i++)
            ;
        return i<<3 | biton(amk_report.nkro.bits[i]);
    } else {
        return amk_report.std.keys[0];
    }
}

static inline void switch_to_nkro(bool nkro)
{
    if (nkro) {
        // from 6 keys to nkro
        uint8_t keys[KEYBOARD_REPORT_KEYS];
        for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
            keys[i] = amk_report.std.keys[i];
            amk_report.std.keys[i] = 0;
        }

        amk_report.nkro_mode = true;
        for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
            if (keys[i]) {
                add_key_bit(keys[i]);
            }
        }
        action_debug("NKRO: ON\n");
    } else {
        // reset the mode should be enough
        amk_report.nkro_mode = false;
        action_debug("NKRO: OFF\n");
    }
}

static inline void add_key_byte(uint8_t code)
{
    int8_t i = 0;
    int8_t empty = -1;
    for (; i < KEYBOARD_REPORT_KEYS; i++) {
        if (amk_report.std.keys[i] == code) {
            break;
        }
        if (empty == -1 && amk_report.std.keys[i] == 0) {
            empty = i;
        }
    }
    if (i == KEYBOARD_REPORT_KEYS) {
        if (empty != -1) {
            amk_report.std.keys[empty] = code;
        } else {
            switch_to_nkro(true);
            add_key_bit(code);
        }
    }
}

static inline void del_key_byte(uint8_t code)
{
    for (uint8_t i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
        if (amk_report.std.keys[i] == code) {
            amk_report.std.keys[i] = 0;
        }
    }
}

static inline void add_key_bit(uint8_t code)
{
    if ((code>>3) < AMK_NKRO_BITS_SIZE) {
        amk_report.nkro.bits[code>>3] |= 1<<(code&7);
    } else {
        action_debug("add_key_bit: can't add: %02X\n", code);
    }
}

static inline void del_key_bit(uint8_t code)
{
    if ((code>>3) < AMK_NKRO_BITS_SIZE) {
        amk_report.nkro.bits[code>>3] &= ~(1<<(code&7));
    } else {
        action_debug("del_key_bit: can't del: %02X\n", code);
    }
    if (has_anykey() == 0) {
        host_keyboard_send((report_keyboard_t*)&amk_report);
        switch_to_nkro(false);
    }
}
