/**
 * @file anim.c
 *  implementation of the animation codec
 */

#include <stdbool.h>
#include <string.h>
#include "anim.h"
#include "ff.h"
#include "amk_printf.h"

// anim file format
// the file store data in little endian sequence
//
//           32 bytes                  16 bytes        16 bytes
//  +---------------------------------------------------------------+
//  |  Signature("ANIM")            |  Size(header) | Offset(data)  |
//  +---------------------------------------------------------------+
//  |  Total size (file)            |   Width       |      Height   |
//  +---------------------------------------------------------------+
//  |    Format     |    Frames     |   Delays    ........          |
//  +---------------------------------------------------------------+
//  |                       ................                        |
//  +---------------------------------------------------------------+
//  |   Frame Data(from Offset)  ........                           |
//  +---------------------------------------------------------------+
//

#define FRAME_MAX 128
typedef struct __attribute__((packed)) {
    char signature[4];  //"ANIM"
    uint16_t size;      // size of the header
    uint16_t offset;    // frame data offset
    uint32_t total;     // total size of the file 
    uint16_t width;     // width of the file
    uint16_t height;    // height of the file
    uint16_t format;    // color format
    uint16_t frames;    // total frames
    uint16_t delays[FRAME_MAX]; // array of the delay, assume not over FRAME_MAX
} anim_header_t;

typedef struct {
    anim_header_t header;   // anim file header
    uint16_t frame;         // current frame
    uint16_t delay;         // current delay 
    FIL file;               // anim file object
    //const void *data;       // current frame data 
} anim_file_t;

struct anim_t {
    anim_file_t obj;
    bool used;
};

#define ANIM_FILE_MAX 1
static anim_t anims[ANIM_FILE_MAX] = {
    {.used = false},
};

static FATFS flashfs;
static char flashfs_path[4];

static void anim_reset(anim_t* anim);
static bool anim_init(anim_t* anim, FIL* file);

void anim_mount(void)
{
    f_mount(&flashfs, flashfs_path, 1);
}

anim_t *anim_open(const char *name)
{
    anim_t *cur = NULL;
    for (int i = 0; i < ANIM_FILE_MAX; i++) {
        if ( !anims[i].used) {
            cur = &anims[i];
            break;
        }
    }
    if (!cur) return NULL;
    FIL file;
    FRESULT ret = f_open(&file, name, FA_READ);
    if (ret == FR_OK) {
        if (anim_init(cur, &file)) {
            cur->used = true;
            return cur;
        } else {
            f_close(&file);
        }
    } 
    
    return NULL;
}

uint32_t anim_get_width(anim_t* anim)
{
    return anim->obj.header.width;
}

uint32_t anim_get_height(anim_t* anim)
{
    return anim->obj.header.height;
}

uint32_t anim_get_bitformat(anim_t* anim)
{
    return anim->obj.header.format;
}

uint32_t anim_step(anim_t *anim, uint32_t *delay, void *buf, uint32_t size)
{
    uint32_t frame_size = anim->obj.header.width*anim->obj.header.height*anim->obj.header.format;
    if (size < frame_size) {
        amk_printf("ANIM buffer size too small, size=%d, frame=%d\n", size, frame_size);
        return 0;
    }

    f_lseek(&anim->obj.file, anim->obj.header.offset+frame_size*anim->obj.frame);

    UINT readed = 0;
    f_read(&anim->obj.file, buf, size, &readed);

    *delay = anim->obj.delay;
    anim->obj.frame = (anim->obj.frame+1) % anim->obj.header.frames;
    anim->obj.delay = anim->obj.header.delays[anim->obj.frame];
    return readed;
}

/*void anim_rewind(anim_t *anim)
{
    anim->obj.frame = 0;
    anim->obj.delay = 0;
} */

void anim_close(anim_t *anim)
{
    if (anim->used) {
        f_close(&anim->obj.file);
        anim_reset(anim);
    }
}

static void anim_reset(anim_t *anim)
{
    memset(anim, 0, sizeof(anim_t));
}

static bool anim_init(anim_t *anim, FIL *file)
{
    anim_reset(anim);
    memcpy(&anim->obj.file, file, sizeof(FIL));
    UINT readed = 0;
    if (FR_OK != f_read(&anim->obj.file, &anim->obj.header, sizeof(anim_header_t), &readed)) {
        return false;
    }

    return true;
}