/**
 * @file amk_apc.h
 * @brief actuation point control and rapid trigger
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct apc_special_key
{
    uint8_t row;
    uint8_t col;
    uint32_t min_preset;
    uint32_t max_preset;
    uint32_t interval_preset;
    uint32_t sens;
};

#ifdef APC_SPECIAL_KEY_NUM
extern struct apc_special_key apc_special_keys[APC_SPECIAL_KEY_NUM];
#endif

void apc_matrix_init(void);
uint32_t apc_matrix_calibrate(uint32_t row, uint32_t col, uint32_t value);
bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t value, bool *valid);
void apc_matrix_update_interval(void);
void apc_matrix_update_interval_at(uint8_t profile, uint32_t row, uint32_t col);
void apc_matrix_update_sensitivity(void);

#define AMK_POLE_OFFSET     (0)
#define AMK_PROFILE_OFFSET  (1)                             // 2 bits
#define AMK_DKS_OFFSET      (3)

#define AMK_POLE_MASK       (1 << AMK_POLE_OFFSET)
#define AMK_PROFILE_MASK    (3 << AMK_PROFILE_OFFSET)       // 2 bits
#define AMK_DKS_MASK        (1 << AMK_DKS_OFFSET)

void amk_set_ms_config(uint8_t config);
uint8_t amk_get_ms_config(void);
void amk_udpate_ms_config(void);