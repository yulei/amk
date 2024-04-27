/**
 * @file anim.c
 *  implementation of the animation codec
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "amk_hal.h"
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

#define FRAME_MAX   512//1024
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

#define ANIM_INVALID_WIDTH      0xFFFFFFFF
#define ANIM_INVALID_HEIGHT     0xFFFFFFFF

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
static bool flashfs_mounted = false;

static bool anim_init(anim_t *anim);
static void anim_scan(anim_t *anim, uint32_t width, uint32_t height);
static bool anim_check_file(const char *path, const char *sig, uint32_t width, uint32_t height);
static FRESULT safe_open(FIL* fp, const TCHAR* path, BYTE mode);
static void safe_close(FIL* f);

bool anim_mount(bool mount)
{
    FRESULT res = FR_OK;
    if (mount) {
        if (flashfs_mounted) return true;

        res = f_mount(&flashfs, "", 1);
        if (res == FR_OK) flashfs_mounted = true;
    } else {
        if (!flashfs_mounted) return true;

        res = f_unmount("");
        flashfs_mounted = false;
    }
    return (res == FR_OK);
}

anim_t *anim_open_with_size(const char *path, anim_type_t type, uint32_t width, uint32_t height)
{
    if (!path || (strlen(path)==0)) {
        strcpy(&anim_inst[type].path[0], ANIM_ROOT_DIR);
    } else {
        if (strlen(path) >= ANIM_FILE_NAME_MAX) {
            return NULL;
        } else {
            strcpy(&anim_inst[type].path[0], path);
        };
    }

    anim_inst[type].type = type;

    anim_scan(&anim_inst[type], width, height);

    if (anim_inst[type].total == 0) {
        amk_printf("anim scan failed, type=%d, width=%d, height=%d\n", type, width, height);
        return NULL;
    }

    if (anim_init(&anim_inst[type])) {
        srand(timer_read32());
        return &anim_inst[type];
    }
    
    return NULL;
}

anim_t *anim_open(const char *path, anim_type_t type)
{
    return anim_open_with_size(path, type, ANIM_INVALID_WIDTH, ANIM_INVALID_HEIGHT);
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

uint32_t anim_get_frames(anim_t* anim)
{
    return anim->obj.header.frames;
}

uint16_t anim_get_delay(anim_t* anim, uint16_t index)
{
    if (index < anim_get_frames(anim)) {
        return anim->obj.header.delays[index];
    }
    
    return 0;
}

bool anim_set_frame(anim_t* anim, uint32_t frame)
{
    if (frame >= anim->obj.header.frames) {
        amk_printf("ANIM set frame at %d failed, max is %d\n", frame, anim->obj.header.frames);
        return false;
    }

    anim->obj.frame = frame;
    return true;
}

uint32_t anim_step(anim_t *anim, uint32_t *delay, void *buf, uint32_t size)
{
    if (anim->obj.frame >= anim->obj.header.frames || anim->obj.frame >= FRAME_MAX) {
        safe_close(&anim->obj.file);
        return 0; // at the file end
    } 

    *delay = (anim->obj.header.frames==1) ? 1000 : anim->obj.header.delays[anim->obj.frame];
    uint32_t frame_size = anim->obj.header.width*anim->obj.header.height*2;//anim->obj.header.format;
    if (size < frame_size) {
        amk_printf("ANIM buffer size too small, size=%d, frame=%d\n", size, frame_size);
        safe_close(&anim->obj.file);
        return 0;
    }

    //if (FR_OK != f_lseek(&anim->obj.file, anim->obj.header.size+frame_size*anim->obj.frame) ) {
        //amk_printf("ANIM failed to seek new position, pos=%d\n", anim->obj.header.size+frame_size*anim->obj.frame);
    if (FR_OK != f_lseek(&anim->obj.file, anim->obj.header.offset+frame_size*anim->obj.frame) ) {
        amk_printf("ANIM failed to seek new position, pos=%d\n", anim->obj.header.offset+frame_size*anim->obj.frame);
        safe_close(&anim->obj.file);
        return 0;
    }

    //__disable_irq();
    UINT readed = 0;
    if (FR_OK != f_read(&anim->obj.file, buf, size, &readed)) {
        amk_printf("ANIM failed to read new frame data, size=%d\n", size);
        safe_close(&anim->obj.file);
    //    __enable_irq();
        return 0;
    }
    //__enable_irq();

    if (size != readed) {
        amk_printf("ANIM read file size mismatch, should=%d, actually=%d\n", size, readed);
        safe_close(&anim->obj.file);
        return 0;
    }

    anim->obj.frame++;
    return readed;
}

uint32_t anim_total(anim_t* anim)
{
    return anim->total;
}

bool anim_next(anim_t *anim)
{
    anim->current++;
    if (anim->current >= anim->total) {
        //safe_close(&anim->obj.file);
        //reset to start
        anim->current = 0;
    }
    return anim_init(anim);
}

bool anim_rewind(anim_t *anim)
{
    //safe_close(&anim->obj.file);
    return anim_init(anim);
}

bool anim_random(anim_t *anim)
{
    //safe_close(&anim->obj.file);
    anim->current = rand() % anim->total;
    return anim_init(anim);
}

void anim_close(anim_t *anim)
{
    safe_close(&anim->obj.file);
    memset(anim, 0, sizeof(anim_t));
}

static bool anim_init(anim_t *anim)
{
   // __disable_irq();
    FRESULT ret = safe_open(&(anim->obj.file), anim->files[anim->current], FA_READ);
    if (ret != FR_OK) {
    //    __enable_irq();
        return false;
    } 
    //__enable_irq();

    memset(&anim->obj.header, 0, sizeof(anim_header_t));
    anim->obj.frame = 0;
    UINT readed = 0;
    //__disable_irq();
    if (FR_OK != f_read(&anim->obj.file, &anim->obj.header, sizeof(anim_header_t)-FRAME_MAX*2, &readed)) {
        f_close(&anim->obj.file);
    //    __enable_irq();
        return false;
    }
    //__enable_irq();
    
    uint32_t frames = anim->obj.header.frames > FRAME_MAX ? FRAME_MAX : anim->obj.header.frames;
    if (FR_OK != f_read(&anim->obj.file, &anim->obj.header.delays[0], frames*2, &readed)) {
        f_close(&anim->obj.file);
        return false;
    }

    switch (anim->type) {
        case ANIM_TYPE_MAIN:
            if (memcmp(ANIM_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
            break;
        case ANIM_TYPE_AUX:
            if (memcmp(AUXI_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
            break;
        case ANIM_TYPE_FONT:
            if (memcmp(AMFT_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
            break;
        case ANIM_TYPE_GLYPH:
            if (memcmp(AMGH_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
            break;
        case ANIM_TYPE_STATUS:
            if (memcmp(ASTS_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
            break;
        case ANIM_TYPE_BACKGROUND:
            if (memcmp(ABKG_SIG, anim->obj.header.signature, 4) != 0) {
                f_close(&anim->obj.file);
                return false;
            }
            break;
        default:
            f_close(&anim->obj.file);
            return false;
    }

    return true;
}


static void anim_scan(anim_t *anim, uint32_t width, uint32_t height)
{
    anim->total = 0;
    anim->current = 0;

    DIR dir;
    FRESULT res = f_opendir(&dir, anim->path);
    if (res == FR_OK) {
        for (;;) {
            FILINFO fno;
            res = f_readdir(&dir, &fno);
            amk_printf("anim SCAN: failed to readdir\n");
            if (res != FR_OK || fno.fname[0] == 0) break;
            if ( (fno.fattrib & AM_DIR) == 0) {
                const char* sig = ANIM_SIG;
                if (anim->type == ANIM_TYPE_AUX) {
                    sig = AUXI_SIG;
                }
                if (anim->type == ANIM_TYPE_FONT) {
                    sig = AMFT_SIG;
                }
                if (anim->type == ANIM_TYPE_GLYPH) {
                    sig = AMGH_SIG;
                }
                if (anim->type == ANIM_TYPE_STATUS) {
                    sig = ASTS_SIG;
                }
                if (anim->type == ANIM_TYPE_BACKGROUND) {
                    sig = ABKG_SIG;
                }

                if (anim_check_file(fno.fname, sig, width, height)) {
                    memcpy(&anim->files[anim->total][0], fno.fname, ANIM_FILE_NAME_MAX);
                    anim->total++;
                    if (anim->total >= ANIM_FILE_MAX) {
                        // oversize, break the loop
                        break;
                    }
                } else {
                    amk_printf("anim SCAN: failed to check: %s, width=%d, height=%d\n", fno.fname, width, height);
                }
            }
        }
        f_closedir(&dir);
    } else {
        amk_printf("anim SCAN: failed to open=%s, res=%d\n", anim->path, res);
    }
} 

static bool anim_check_file(const char *path, const char* sig, uint32_t width, uint32_t height)
{
    FIL file;
    if (FR_OK != f_open(&file, path, FA_READ)) {
        //amk_printf("ANIM check: failed to open file: %s\n", path);
        return false;
    }

    anim_header_t header;
    UINT readed = 0;
    bool result = false;
    if (f_size(&file) <= (sizeof(header) - FRAME_MAX*2)) {
        amk_printf("ANIM check: file size too small\n");
        goto exit;
    }

    // read header without delay
    if (FR_OK != f_read(&file, &header, sizeof(anim_header_t)-FRAME_MAX*2, &readed)) {
        //amk_printf("ANIM check: failed to read file header: %s\n", path);
        goto exit;
    }
    
    if (memcmp(sig, header.signature, 4) != 0) {
        amk_printf("ANIM check: signature invalid: %x\n", &header.signature[0]);
        goto exit;
    }

    if ((width != ANIM_INVALID_WIDTH) && (height != ANIM_INVALID_HEIGHT)) {
        if (header.width != width || header.height != height) {
            amk_printf("ANIM check: invlid size: required (width=%d, height=%d), actually (width=%d, height=%d)\n", 
                width, height, header.width, header.height);
            goto exit;
        }
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

bool anim_load_font(const void* data, void* buf, uint32_t total_frames)
{
    anim_header_t header;
    const uint8_t *src = (const uint8_t *)data; 
    memcpy(&header, src, sizeof(anim_header_t));
    if (header.frames < total_frames) {
        return false;
    }

    src += header.offset;
    uint8_t *dst = (uint8_t *)buf; 
    uint32_t frame_size = header.width*header.height*header.format;
    for (uint32_t i = 0; i < total_frames; i++) {
        memcpy(dst, src, frame_size);
        dst += frame_size;
        src += frame_size;
    }

    return true;
}

static void safe_close(FIL* f)
{
    if (f->obj.fs != 0) {
        f_close(f);
    } else {
        amk_printf("ANIM: invalid file to close\n");
    }
}

static FRESULT safe_open(FIL* fp, const TCHAR* path, BYTE mode)
{
    if (fp->obj.fs != 0) {
        amk_printf("ANIM: invalid file to open\n");
    }
    return f_open(fp, path, mode);
}

static uint32_t text_key_offset(anim_t *anim, uint16_t key)
{
    for (int i = 0; i < anim->obj.header.frames; i++) {
        if (key == anim->obj.header.delays[i]) {
            // find key
            uint32_t text_size = (anim->obj.header.width*anim->obj.header.height)/8;
            return (anim->obj.header.offset+i*text_size);
        }
    }

    return 0;
}

bool anim_get_glyph_bitmap(anim_t *anim, uint16_t key, void *buf, uint32_t size)
{
    if (!anim) return false;
    uint32_t offset = text_key_offset(anim, key);
    if (!offset) return false;

    if (FR_OK != f_lseek(&anim->obj.file, offset) ) {
        amk_printf("TEXT failed to seek key position, pos=%d\n", offset);
        safe_close(&anim->obj.file);
        return false;
    }

    UINT readed = 0;
    if (FR_OK != f_read(&anim->obj.file, buf, size, &readed)) {
        amk_printf("TEXT failed to read key data, size=%d\n", size);
        safe_close(&anim->obj.file);
        return false;
    }

    return true;
}
