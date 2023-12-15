/**
 * @file nofrendo_task.c
 */

#include "nofrendo_task.h"
#include "nes_display.h"
#include "nes_emu.h"
#include "emuapi.h"
#include "umm_malloc.h"
#include "ff.h"
#include "arcada_def.h"
#include "amk_printf.h"

#define NES_HEAP_SIZE   (128*1024)
static uint32_t nes_heap[NES_HEAP_SIZE/4];

static FATFS romfs;
static bool romfs_mounted = false;

static uint16_t* frame_buffer = NULL;
static bool emu_inited = false;

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
}

void nofrendo_init(void)
{
    umm_init_heap(nes_heap, NES_HEAP_SIZE);
    if (!mount_romfs()) {
        return;
    }

    frame_buffer = (uint16_t*)umm_malloc(ARCADA_TFT_WIDTH*ARCADA_TFT_HEIGHT*2);
    if (!frame_buffer) {
        amk_printf("Failed to allocate frame buffer\n");
        return;
    }

    nes_display.set_frame_buffer(frame_buffer);
    emu_Init("/nes/mario.nes");

//    return;

    emu_inited = true;
}

bool nofrendo_start(const char* rom)
{
    return false;
}

void nofrendo_task(uint32_t ticks)
{
    if (!emu_inited) return;

    emu_Step(0);
}

void nofrendo_pause(void)
{}

bool nofrendo_resume(void)
{
    return false;
}

void nofrendo_stop(void)
{}

void nofrendo_deinit(void)
{}