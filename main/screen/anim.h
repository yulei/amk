/**
 * @file anim.h
 *  custom animation file codec
 */

#pragma once

#include <stdint.h>

typedef struct anim_t anim_t;

void anim_mount(void);

anim_t *anim_open(const char *name);
uint32_t anim_get_width(anim_t* anim);
uint32_t anim_get_height(anim_t* anim);
uint32_t anim_get_bitformat(anim_t* anim);
uint32_t anim_step(anim_t *anim, uint32_t *delay, void *buf, uint32_t size);
//void anim_rewind(anim_t *anim);
void anim_close(anim_t *anim);