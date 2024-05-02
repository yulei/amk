/**
 * @file nofrendo_emu.c
 */

#include <string.h>
#include "nofrendo_emu.h"
#include "nes_display.h"
#include "nes_emu.h"
#include "emuapi.h"
#include "static_memory.h"
#include "ff.h"
#include "arcada_def.h"
#include "amk_printf.h"

#define NES_MAX_FILENAME    128
#define NES_HEAP_SIZE       (120*1024)
static uint32_t nes_heap[NES_HEAP_SIZE/4];

//static FATFS romfs;
//static bool romfs_mounted = false;

static uint16_t* frame_buffer = NULL;
volatile bool emu_inited = false;

/*
static bool mount_romfs(void)
{
    FRESULT res = FR_OK;
    if (romfs_mounted) return true;

    res = f_mount(&romfs, "", 1);
    if (res == FR_OK){
        romfs_mounted = true;
    } else {
        amk_printf("failed to mount nes romfs,error=%d\n", res);
        romfs_mounted = false;
    }
    return (res == FR_OK);
}*/

extern bool nofrendo_mode;

void nofrendo_init(void* buffer, char* path)
{
    emu_inited = false;
    static_memory_create(nes_heap, NES_HEAP_SIZE);
    DIR dir;
    FRESULT res = f_opendir(&dir, path);
    if (res != FR_OK) {
        return;
    }

    char* file_name = NULL;
    for (;;) {
        FILINFO fno;
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;
        if ( (fno.fattrib & AM_DIR) == 0) {
            if (strlen(fno.fname) < NES_MAX_FILENAME) {
                file_name = static_memory_alloc(NES_MAX_FILENAME);
                snprintf_(file_name, NES_HEAP_SIZE, "%s/%s", path, fno.fname);
                break;
            }
        }
    }
    f_closedir(&dir);
    if (!file_name) {
        return;
    }

    memset(buffer, 0, 128*128*2);
    frame_buffer = (uint16_t*)buffer;
    nes_display.set_frame_buffer(frame_buffer);
    emu_Init(file_name);
    if (emu_inited) {
        nofrendo_mode = true;
    }
}

void nofrendo_task(uint32_t ticks)
{
    if (!emu_inited) return;

    emu_Step(0);
}

void nofrendo_deinit(void)
{
    nes_End();
    nofrendo_mode = false;
}