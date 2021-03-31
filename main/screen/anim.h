/**
 * @file anim.h
 *  custom animation file codec
 */

#pragma once

#include <stdint.h>

typedef enum {
    ANIM_TYPE_MAIN,
    ANIM_TYPE_AUX,
    ANIM_TYPE_MAX,
} anim_type_t;

typedef struct anim_t anim_t;

bool anim_mount(bool mount);

anim_t *anim_open(const char *path, anim_type_t type);
uint32_t anim_get_width(anim_t* anim);
uint32_t anim_get_height(anim_t* anim);
uint32_t anim_get_bitformat(anim_t* anim);
uint32_t anim_step(anim_t *anim, uint32_t *delay, void *buf, uint32_t size);
bool anim_rewind(anim_t *anim);
bool anim_next(anim_t *anim);
bool anim_random(anim_t *anim);
void anim_close(anim_t *anim);