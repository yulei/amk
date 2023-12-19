/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_eeprom.c
 */

#include "amk_hal.h"
#include "eeprom.h"
#include "keycode_config.h"
#include "amk_eeprom.h"
#include "amk_printf.h"

#ifdef RGB_ENABLE
#include "rgb_led.h"
#endif

#ifndef EEPROM_MANAGER_DEBUG
#define EEPROM_MANAGER_DEBUG 1
#endif

#if EEPROM_MANAGER_DEBUG
#define ee_mgr_debug  amk_printf
#else
#define ee_mgr_debug(...)
#endif

#ifdef RGB_ENABLE
void eeconfig_read_rgb(void* rgb, uint8_t index)
{
    rgb_cfg_t *config = (rgb_cfg_t*)rgb;
    uint8_t *addr = ((uint8_t*)EECONFIG_RGB) + index*6;
    config->enable  = eeprom_read_byte(addr);
    config->mode    = eeprom_read_byte(addr+1);
    config->speed   = eeprom_read_byte(addr+2);
    config->hue     = eeprom_read_byte(addr+3);
    config->sat     = eeprom_read_byte(addr+4);
    config->val     = eeprom_read_byte(addr+5);
    ee_mgr_debug("EE MGR: read rgb, enable: %d\n", config->enable);

}

void eeconfig_write_rgb(const void* rgb, uint8_t index)
{
    rgb_cfg_t *config = (rgb_cfg_t*)rgb;
    uint8_t *addr = ((uint8_t*)EECONFIG_RGB) + index*6;
    eeprom_write_byte(addr,   config->enable);
    eeprom_write_byte(addr+1, config->mode);
    eeprom_write_byte(addr+2, config->speed);
    eeprom_write_byte(addr+3, config->hue);
    eeprom_write_byte(addr+4, config->sat);
    eeprom_write_byte(addr+5, config->val);
    ee_mgr_debug("EE MGR: write rgb, enable: %d\n", config->enable);
}

void eeconfig_update_rgb(const void* rgb, uint8_t index)
{
    eeconfig_write_rgb(rgb, index);
}
#endif

__attribute__((weak)) void eeconfig_init_custom(void) {}

void eeconfig_init_kb(void)
{
    eeconfig_update_kb(0);
    eeconfig_init_user();
    eeconfig_init_custom();

#ifdef NKRO_ENABLE
    //keymap_config.nkro = 1;
    //eeconfig_update_keymap(keymap_config.raw);
#endif

#ifdef RGB_ENABLE
    rgb_led_config_init();
#endif

#ifdef STATE_SCAN_ENABLE 
    uint8_t debounce = (0<<4) | 5;
    eeconfig_update_debounce(debounce);
#endif
}

uint8_t eeconfig_read_usb(void)
{
    //return 3;
    return eeprom_read_byte(EECONFIG_USB);
}
void eeconfig_update_usb(uint8_t usb)
{
    eeprom_write_byte(EECONFIG_USB, usb);
}

uint8_t eeconfig_read_debounce(void)
{
    return eeprom_read_byte(EECONFIG_DEBOUNCE);
}
void eeconfig_update_debounce(uint8_t debounce)
{
    eeprom_write_byte(EECONFIG_DEBOUNCE, debounce);
}

uint8_t eeconfig_read_pole(void)
{
    return eeprom_read_byte(EECONFIG_POLE);
}
void eeconfig_update_pole(uint8_t pole)
{
    eeprom_write_byte(EECONFIG_POLE, pole);
}

uint8_t eeconfig_read_rt_sens(void)
{
    return eeprom_read_byte(EECONFIG_RT_SENS);
}

void eeconfig_update_rt_sens(uint8_t rt)
{
    eeprom_write_byte(EECONFIG_RT_SENS, rt);
}

uint8_t eeconfig_read_top_sens(void)
{
    return eeprom_read_byte(EECONFIG_TOP_SENS);
}

void eeconfig_update_top_sens(uint8_t top)
{
    eeprom_write_byte(EECONFIG_TOP_SENS, top);
}

uint8_t eeconfig_read_btm_sens(void)
{
    return eeprom_read_byte(EECONFIG_BTM_SENS);
}

void eeconfig_update_btm_sens(uint8_t btm)
{
    eeprom_write_byte(EECONFIG_BTM_SENS, btm);
}