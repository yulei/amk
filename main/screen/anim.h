/**
 * @file anim.h
 *  custom animation file codec
 */

#pragma once

#include <stdint.h>

#define AUXI_SIG    "AUXI"
#define ANIM_SIG    "ANIM"
#define AMFT_SIG    "AMFT"
#define AMGH_SIG    "AMGH"
#define ASTS_SIG    "ASTS"
#define ABKG_SIG    "ABKG"

#define ANIM_FILE_NAME_MAX  13
#define ANIM_FILE_MAX       16//32
#define ANIM_ROOT_DIR       "/"

typedef enum {
    ANIM_TYPE_MAIN,
    ANIM_TYPE_AUX,
    ANIM_TYPE_FONT,
    ANIM_TYPE_GLYPH,
    ANIM_TYPE_STATUS,
    ANIM_TYPE_BACKGROUND,
    ANIM_TYPE_MAX,
} anim_type_t;

typedef struct anim_t anim_t;

bool anim_mount(bool mount);

anim_t *anim_open(const char *path, anim_type_t type);
anim_t *anim_open_with_size(const char *path, anim_type_t type, uint32_t width, uint32_t height);
uint32_t anim_get_width(anim_t* anim);
uint32_t anim_get_height(anim_t* anim);
uint32_t anim_get_bitformat(anim_t* anim);
uint32_t anim_get_frames(anim_t* anim);
uint16_t anim_get_delay(anim_t* anim, uint16_t index);

uint32_t anim_total(anim_t* anim);
bool anim_set_frame(anim_t* anim, uint32_t frame);
uint32_t anim_step(anim_t *anim, uint32_t *delay, void *buf, uint32_t size);
bool anim_rewind(anim_t *anim);
bool anim_next(anim_t *anim);
bool anim_random(anim_t *anim);
void anim_close(anim_t *anim);

bool anim_load_font(const void* data, void* buf, uint32_t size);

void anim_lock(void);
void anim_unlock(void);

//
// glyph file processing
//
bool anim_load_glyph(const void* data, void* buf, uint32_t size);
bool anim_get_glyph_bitmap(anim_t *anim, uint16_t key, void*buf, uint32_t size);
