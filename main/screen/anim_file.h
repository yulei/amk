/**
 * @file anim_file.h
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
*/

#pragma once

#include <stdbool.h>
#include "anim.h"

struct amk_anim_file_system_info {
    uint32_t total_file;
    uint32_t total_space;
    uint32_t free_space;
};

struct amk_anim_file_info {
    char name[ANIM_FILE_NAME_MAX];
    uint32_t size;
    uint16_t index;
    uint8_t type;
    uint8_t used;
};

void amk_anim_file_init(void);
bool amk_anim_file_system_info(struct amk_anim_file_system_info* info);
void amk_anim_file_refresh(void);
bool amk_anim_file_get_info(uint16_t index, struct amk_anim_file_info* info);
bool amk_anim_file_open(struct amk_anim_file_info* info, bool read);
bool amk_anim_file_write(struct amk_anim_file_info* info, void* data, uint32_t offset, uint32_t size);
bool amk_anim_file_read(struct amk_anim_file_info* info, void* data, uint32_t offset, uint32_t size);
bool amk_anim_file_close(struct amk_anim_file_info* info);
bool amk_anim_file_delete(struct amk_anim_file_info* info);
