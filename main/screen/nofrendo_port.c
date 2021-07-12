/**
 * @file nofrendo_port.c
 * @brief osd porting layer
 */

#include <stdint.h>
#include "nes.h"
#include "vid_drv.h"
#include "osd.h"
#include "umm_malloc.h"

// memory management 
#define NOFRENDO_HEAP_SIZE  (32*1024)

static uint8_t nofrendo_heap[NOFRENDO_HEAP_SIZE];
uint32_t UMM_MALLOC_CFG_HEAP_SIZE = NOFRENDO_HEAP_SIZE;
void *UMM_MALLOC_CFG_HEAP_ADDR = nofrendo_heap;

void *mem_alloc(int size, bool prefer_fast_memory)
{
    return umm_malloc(size);
}

// filename 
void osd_fullname(char *fullname, const char *shortname)
{
    strncpy(fullname, shortname, PATH_MAX);
}

char *osd_newextension(char *string, char *ext)
{
    // dirty: assume both extensions is 3 characters
	size_t l = strlen(string);
	string[l - 3] = ext[1];
	string[l - 2] = ext[2];
	string[l - 1] = ext[3];

	return string;
}

int osd_makesnapname(char *filename, int len)
{
    // do not support snapshot
    return -1;
}

// audio 
void osd_setsound(void (*playfunc)(void *buffer, int size))
{}

void osd_getsoundinfo(sndinfo_t *info)
{
    // dummy value
	info->sample_rate = 22050;
	info->bps = 16;
}

// video
/* init function - return 0 on success, nonzero on failure */
static int amk_vid_init(int width, int height)
{
    return 0;
}

/* clean up after driver (can be NULL) */
static void amk_vid_shutdown(void)
{
}

/* set a video mode - return 0 on success, nonzero on failure */
static int amk_vid_set_mode(int width, int height)
{
    return 0;
}

/* set up a palette */
static void amk_vid_set_palette(rgb_t *palette)
{
}

/* custom bitmap clear (can be NULL) */
static void amk_vid_clear(uint8 color)
{
}

/* lock surface for writing (required) */
static bitmap_t *amk_vid_lock_write(void)
{
    return NULL;
}

/* free a locked surface (can be NULL) */
static void amk_vid_free_write(int num_dirties, rect_t *dirty_rects)
{}

/* custom blitter - num_dirties == -1 if full blit required */
static void amk_vid_blit(bitmap_t *primary, int num_dirties, rect_t *dirty_rects)
{}

static viddriver_t amk_vid_driver = {
    .name = "",
    .init = amk_vid_init,
    .shutdown = amk_vid_shutdown,
    .set_mode = amk_vid_set_mode,
    .set_palette = amk_vid_set_pallette,
    .clear = amk_vid_clear,
    .lock_write = amk_vid_lock_write,
    .free_write = amk_vid_free_write,
    .custom_blit = amk_vid_blit,
    .invalidate = false,
};

void osd_getvideoinfo(vidinfo_t *info)
{
    info->default_width = NES_SCREEN_WIDTH;
    info->default_height = NES_SCREEN_HEIGHT;
    info->driver = &amk_vid_driver;
}


// input
void osd_getinput(void)
{}

void osd_getmouse(int *x, int *y, int *button)
{}

// main loop
int osd_init(void)
{
    return 0;
}

void osd_shutdown(void)
{}

int osd_main(int argc, char *argv[])
{
    return 0;
}

int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
    return 0;
}
