/**
 * @file anim.c
 *  implementation of the animation codec
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "anim.h"
#include "ff.h"
#include "timer.h"
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

#define AUXI_SIG    "AUXI"
#define ANIM_SIG    "ANIM"
#define FRAME_MAX   256
typedef struct __attribute__((packed)) {
    char signature[4];  //"ANIM" or "AUXI"
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
    FIL file;               // anim file object
    bool opened;
} anim_file_t;

#define ANIM_FILE_NAME_MAX  13
#define ANIM_FILE_MAX       32
#define ANIM_ROOT_DIR       "/"
struct anim_t {
    anim_file_t obj;
    anim_type_t type;
    uint16_t total;
    uint16_t current;
    char path[ANIM_FILE_NAME_MAX];
    char files[ANIM_FILE_MAX][ANIM_FILE_NAME_MAX];
};

static anim_t anim_inst[ANIM_TYPE_MAX];
static FATFS flashfs;

static bool anim_init(anim_t *anim, FIL *file);
static void anim_scan(anim_t *anim);
static bool anim_open_current(anim_t *anim);
static bool anim_check_file(const char *path, const char *sig);

bool anim_mount(bool mount)
{
    FRESULT res = FR_OK;
    if (mount) {
        res = f_mount(&flashfs, "", 1);
    } else {
        res = f_unmount("");
    }
    return (res == FR_OK);
}

anim_t *anim_open(const char *path, anim_type_t type)
{
    if (!path || (strlen(path)==0)) {
        memcpy(&anim_inst[type].path[0], ANIM_ROOT_DIR, ANIM_FILE_NAME_MAX);
    } else {
        memcpy(&anim_inst[type].path[0], path, ANIM_FILE_NAME_MAX);
    }
    anim_inst[type].type = type;

    anim_scan(&anim_inst[type]);

    if (anim_inst[type].total == 0) {
        return NULL;
    }

    if (anim_open_current(&anim_inst[type])) {
        srand(timer_read32());
        return &anim_inst[type];

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
    if (anim->obj.frame >= anim->obj.header.frames || anim->obj.frame >= FRAME_MAX) {
        f_close(&anim->obj.file);
        return 0; // at the file end
    } 

    *delay = (anim->obj.header.frames==1) ? 1000 : anim->obj.header.delays[anim->obj.frame];
    uint32_t frame_size = anim->obj.header.width*anim->obj.header.height*anim->obj.header.format;
    if (size < frame_size) {
        amk_printf("ANIM buffer size too small, size=%d, frame=%d\n", size, frame_size);
        return 0;
    }

    f_lseek(&anim->obj.file, anim->obj.header.offset+frame_size*anim->obj.frame);

    UINT readed = 0;
    f_read(&anim->obj.file, buf, size, &readed);
    anim->obj.frame++;
    return readed;
}

bool anim_next(anim_t *anim)
{
    anim->current++;
    if (anim->current >= anim->total) {
        f_close(&anim->obj.file);
        //reset to start
        anim->current = 0;
    }
    return anim_open_current(anim);
}

bool anim_rewind(anim_t *anim)
{
    return anim_open_current(anim);
}

bool anim_random(anim_t *anim)
{
    f_close(&anim->obj.file);
    anim->current = rand() % anim->total;
    return anim_open_current(anim);
}

void anim_close(anim_t *anim)
{
    f_close(&anim->obj.file);
    memset(anim, 0, sizeof(anim_t));
}

static bool anim_init(anim_t *anim, FIL *file)
{
    memset(&anim->obj.header, 0, sizeof(anim_header_t));
    anim->obj.frame = 0;
    memcpy(&anim->obj.file, file, sizeof(FIL));
    UINT readed = 0;
    if (FR_OK != f_read(&anim->obj.file, &anim->obj.header, sizeof(anim_header_t), &readed)) {
        f_close(&anim->obj.file);
        return false;
    } else {
        if (anim->type == ANIM_TYPE_MAIN) {
            if (memcmp(ANIM_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
        } else {
            if (memcmp(AUXI_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
        }
    }

    return true;
}


static void anim_scan(anim_t *anim)
{
    anim->total = 0;
    anim->current = 0;

    DIR dir;
    FRESULT res = f_opendir(&dir, anim->path);
    if (res == FR_OK) {
        for (;;) {
            FILINFO fno;
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if ( (fno.fattrib & AM_DIR) == 0) {
                const char* sig = (anim->type == ANIM_TYPE_MAIN) ? ANIM_SIG : AUXI_SIG;
                if (anim_check_file(fno.fname, sig)) {
                    memcpy(&anim->files[anim->total][0], fno.fname, ANIM_FILE_NAME_MAX);
                    anim->total++;
                    if (anim->total >= ANIM_FILE_MAX) {
                        // oversize, break the loop
                        continue;
                    }
                }
            }
        }
        f_closedir(&dir);
    }
}

static bool anim_open_current(anim_t *anim)
{
    FIL file;
    FRESULT ret = f_open(&file, anim->files[anim->current], FA_READ);
    if (ret == FR_OK) {
        if (anim_init(&anim_inst[anim->type], &file)) {
            return true;
        } else {
            f_close(&file);
        }
    } 

    return false;
}

static bool anim_check_file(const char *path, const char* sig)
{
    FIL file;
    if (FR_OK != f_open(&file, path, FA_READ)) {
        amk_printf("ANIM check: failed to open file: %s\n", path);
        return false;
    }

    anim_header_t header;
    UINT readed = 0;
    bool result = false;
    if (FR_OK != f_read(&file, &header, sizeof(anim_header_t), &readed)) {
        amk_printf("ANIM check: failed to read file header: %s\n", path);
        goto exit;
    }
    
    if (memcmp(sig, header.signature, 4) != 0) {
        amk_printf("ANIM check: signature invalid: %x\n", &header.signature[0]);
        goto exit;
    }

    if (f_size(&file) != header.total) {
        amk_printf("ANIM check: file size invalid: should:%d, actually:%d\n", header.total, f_size(&file));
        goto exit;
    }

    result = true;
exit:
    f_close(&file);
    return result;  
}
