/**
 * @file anim_file.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
*/

#include <string.h>

#include "anim_file.h"
#include "ff.h"
#include "amk_printf.h"

#ifndef ANIM_FILE_DEBUG
#define ANIM_FILE_DEBUG 1
#endif

#if ANIM_FILE_DEBUG
#define af_debug        amk_printf
#else
#define af_debug(...)
#endif

static struct amk_anim_file_system_info amk_anim_file_system;
static struct amk_anim_file_info amk_anim_files[ANIM_FILE_MAX];
static FIL af_file;

static uint8_t  af_type(const char* sig) 
{
    if (memcmp(ANIM_SIG, sig, 4) == 0) {
        return ANIM_TYPE_MAIN;
    }

    if (memcmp(ABKG_SIG, sig, 4) == 0) {
        return ANIM_TYPE_BACKGROUND;
    }

    if (memcmp(AUXI_SIG, sig, 4) == 0) {
        return ANIM_TYPE_AUX;
    }

    if (memcmp(ASTS_SIG, sig, 4) == 0) {
        return ANIM_TYPE_STATUS;
    }

    if (memcmp(AMFT_SIG, sig, 4) == 0) {
        return ANIM_TYPE_FONT;
    }

    if (memcmp(AMGH_SIG, sig, 4) == 0) {
        return ANIM_TYPE_GLYPH;
    }

    return ANIM_TYPE_MAX;
}

static void af_clear()
{
    for (int i = 0; i < ANIM_FILE_MAX; i++) {
        amk_anim_files->used = 0;
    }
    amk_anim_file_system.total_file = 0;
    amk_anim_file_system.total_space = 0;
    amk_anim_file_system.free_space = 0;
}

static void af_check(struct amk_anim_file_info* info)
{
    if (FR_OK != f_open(&af_file, info->name, FA_READ)) {
        return;
    }

    if (info->size > 4) {
        char sig[4];
        UINT readed = 0;
        if (FR_OK == f_read(&af_file, sig, 4, &readed)) {
            info->type = af_type(sig);
        } else {
            af_debug("AMK FILE READ: faile to read signature\n");
        }
    }

    f_close(&af_file);
}

void amk_anim_file_init(void)
{
    if (!anim_mount(true)) {
        af_debug("AMK ANIM: Failed to mount u disk");
    }

    amk_anim_file_refresh();
}

bool amk_anim_file_system_info(struct amk_anim_file_system_info* info)
{
    amk_anim_file_refresh();

    info->total_file = amk_anim_file_system.total_file;
    info->total_space = amk_anim_file_system.total_space;
    info->free_space = amk_anim_file_system.free_space;

    return true;
}

void amk_anim_file_refresh(void)
{
    anim_lock();
    af_clear();
    // get file system info
    FATFS *fs = NULL;
    DWORD free_cluster = 0;
    /* Get volume information and free clusters of drive 1 */
    FRESULT res = f_getfree("0:", &free_cluster, &fs);
    if (res == FR_OK) {
        DWORD total_sector = (fs->n_fatent - 2) * fs->csize;
        DWORD free_sector = free_cluster*fs->csize;
        amk_anim_file_system.total_space = total_sector*FF_MIN_SS;
        amk_anim_file_system.free_space = free_sector*FF_MIN_SS;
        af_debug("AMK FILE SCAN: total size = %d, free size = %d\n", amk_anim_file_system.total_space, amk_anim_file_system.free_space);
    } else {
        af_debug("AMK FILE SCAN: failed to get free space, res=%d\n", res);
    }

    uint32_t index = 0;
    DIR dir;
    res = f_opendir(&dir, ANIM_ROOT_DIR);
    if (res == FR_OK) {
        for (;;) {
            FILINFO fno;
            res = f_readdir(&dir, &fno);
            af_debug("AMK FILE SCAN: failed to readdir\n");
            if (res != FR_OK || fno.fname[0] == 0) break;

            if ( (fno.fattrib & AM_DIR) == 0) {
                struct amk_anim_file_info* info = &amk_anim_files[index];
                memcpy(&info->name[0], fno.fname, ANIM_FILE_NAME_MAX);
                info->index = index++;
                info->used = 1;
                info->size = fno.fsize;
                info->type = ANIM_TYPE_MAX;
                af_check(info);
                amk_anim_file_system.total_file++;
            }
        }
        f_closedir(&dir);
    } else {
        af_debug("AMK FILE SCAN: failed to open=%s, res=%d\n", ANIM_ROOT_DIR, res);
    }
    anim_unlock();
}

bool amk_anim_file_get_info(uint16_t index, struct amk_anim_file_info* info)
{
    if (index < ANIM_FILE_MAX) {
        if (amk_anim_files[index].used) {
            memcpy(info, &amk_anim_files[index], sizeof(struct amk_anim_file_info));
            return true;
        }
    }
    return false;
}

bool amk_anim_file_open(struct amk_anim_file_info* info, bool read)
{
    if (read) {
        if (FR_OK != f_open(&af_file, info->name, FA_READ|FA_OPEN_EXISTING)) {
            af_debug("AMK ANIM: Failed to open file, %s\n", info->name);
            return false;
        }

        return true;
    } else {
        if (FR_OK != f_open(&af_file, info->name, FA_WRITE|FA_CREATE_ALWAYS)) {
            af_debug("AMK ANIM: Failed to open file, %s\n", info->name);
            return false;
        }
        for (int i = 0; i < ANIM_FILE_MAX; i++) {
            if (amk_anim_files[i].used == 0) {
                memcpy(amk_anim_files[i].name, info->name, ANIM_FILE_NAME_MAX);
                amk_anim_files[i].index = i;
                amk_anim_files[i].used = 1;
                info->index = i;
                break;
            }
        }
        return true;
    }
    return false;
}

bool amk_anim_file_write(struct amk_anim_file_info* info, void* data, uint32_t offset, uint32_t size)
{
    UINT written = 0;
    UINT pos = f_tell(&af_file);
    if (pos != offset) {
        af_debug("AMK ANIM: offset mismatch, want=%d, cur=%d\n", offset, pos);
        if (FR_OK != f_lseek(&af_file, offset)) {
            af_debug("AMK ANIM: Failed to seek file, %s, pos=%d\n", info->name, offset);
            return false;
        }
    }
    if(FR_OK != f_write(&af_file, data, size, &written)) {
        af_debug("AMK ANIM: Failed to write file, %s, size=%d\n", info->name, size);
        return false;
    }

    return true;
}

bool amk_anim_file_read(struct amk_anim_file_info* info, void* data, uint32_t offset, uint32_t size)
{
    UINT readed = 0;
    UINT pos = f_tell(&af_file);
    if (pos != offset) {
        af_debug("AMK ANIM: offset mismatch, want=%d, cur=%d\n", offset, pos);
        if (FR_OK != f_lseek(&af_file, offset)) {
            af_debug("AMK ANIM: Failed to seek file, %s, pos=%d\n", info->name, offset);
            return false;
        }
    }
    if(FR_OK != f_read(&af_file, data, size, &readed)) {
        af_debug("AMK ANIM: Failed to read file, %s, size=%d\n", info->name, size);
        return false;
    }

    return true;
}

bool amk_anim_file_close(struct amk_anim_file_info* info)
{
    f_close(&af_file);
    return true;
}

bool amk_anim_file_delete(struct amk_anim_file_info* info)
{
    if (FR_OK != f_unlink(info->name)) {
        af_debug("AMK ANIM: Failed to delete file, %s\n", info->name);
        return false;
    }

    return true;
}

__attribute__((weak))
void amk_animation_display_start(void){}

__attribute__((weak))
void amk_animation_display_stop(void){}