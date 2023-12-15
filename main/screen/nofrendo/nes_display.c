/**
 * @file nes_display.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include <string.h>

#include "emuapi.h"
#include "arcada_def.h"
#include "nes_display.h"

static uint16_t* screen = NULL;



static void set_frame_buffer(uint16_t * fb)
{
    screen = fb;
}

static uint16_t* get_frame_buffer(void)
{
    return screen;
}

static void set_area(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
//  arcada.display->startWrite();
//  arcada.display->setAddrWindow(x1, y1, x2-x1+1, y2-y1+1);
}

static void refresh(void)
{
}


static void stop(void)
{
}

static void wait(void)
{
}

static uint16_t* get_line_buffer(int line)
{ 
  return (&screen[line*ARCADA_TFT_WIDTH]);
}

/***********************************************************************************************
    DMA functions
 ***********************************************************************************************/

static void fill_screen(uint16_t color)
{
    uint16_t *dst = &screen[0];
    for (int i = 0; i < EMUDISPLAY_WIDTH*EMUDISPLAY_HEIGHT; i++) {
        *dst++ = color;
    }
}

#if EMU_SCALEDOWN == 1

static void write_line(int width, int height, int stride, uint8_t *buf, uint16_t *palette16)
{
    uint16_t *dst = &screen[EMUDISPLAY_WIDTH*stride];
    while(width--) *dst++ = palette16[*buf++]; // palette16 already byte-swapped
}

#elif EMU_SCALEDOWN == 2

static uint8_t last_line[NATIVE_WIDTH];

static void write_line(int width, int height, int stride, uint8_t *buf, uint32_t *palette32)
{
    if(!(stride & 1)) {
        // Even line number: just copy into last_line buffer for later
        memcpy(last_line, buf, width);
        // (Better still, if we could have nes_ppu.c render directly into
        // alternating scanline buffers, we could avoid the memcpy(), but
        // the EMU_SCALEDOWN definition doesn't reach there and it might
        // require some ugly changes (or the right place to do this might
        // be in nofrendo_arcada.ino). For now though, the memcpy() really
        // isn't a huge burden in the bigger scheme.)
    } else {
        // Odd line number: average 2x2 pixels using last_line and buf.
        uint8_t  *src1 = last_line, // Prior scanline
                *src2 = buf;       // Current scanline
        uint16_t *dst  = &screen[EMUDISPLAY_WIDTH*(stride-1)/2];
        uint32_t  rbg32;
        uint16_t  rgb16;
        uint8_t   idx0, idx1, idx2, idx3;
        width /= 2;
        while(width--) {
        idx0 = *src1++; // Palette index of upper-left pixel
        idx1 = *src2++; // Palette index of lower-left pixel
        idx2 = *src1++; // Palette index of upper-right pixel
        idx3 = *src2++; // Palette index of lower-right pixel
        // Accumulate four 'RBG' palette values...
        rbg32 = palette32[idx0] + palette32[idx1] +
                palette32[idx2] + palette32[idx3];
        // Pallette data is in the form    00RRRRRRRR000BBBBBBBB00GGGGGGGG0
        // so accumulating 4 pixels yields RRRRRRRRRR0BBBBBBBBBBGGGGGGGGGG0.
        // Hold my beer...
        rbg32 &= 0b11111000000111110000011111100000; // Mask 5R, 5B, 6G
        rgb16  = (uint16_t)((rbg32 >> 16) | rbg32);  // Merge high, low words
        //if (test_invert_screen) rgb16 = ~rgb16;
        *dst++ = __builtin_bswap16(rgb16);           // Store big-endian
        }
    }
}

#else
  #error("Only scale 1 or 2 supported")
#endif

static void set_area_centered(void)
{
    set_area((ARCADA_TFT_WIDTH  - EMUDISPLAY_WIDTH ) / 2, 
    (ARCADA_TFT_HEIGHT - EMUDISPLAY_HEIGHT) / 2,
    (ARCADA_TFT_WIDTH  - EMUDISPLAY_WIDTH ) / 2 + EMUDISPLAY_WIDTH  - 1, 
    (ARCADA_TFT_HEIGHT - EMUDISPLAY_HEIGHT) / 2 + EMUDISPLAY_HEIGHT - 1);
}

struct nes_display nes_display = {
    .set_frame_buffer = set_frame_buffer,
	.get_frame_buffer = get_frame_buffer,
    .refresh = refresh,
    .stop = stop,
    .wait = wait,
    .get_line_buffer = get_line_buffer,
    .set_area = set_area,
    .set_area_centered = set_area_centered,
    .write_line = write_line,
	.fill_screen = fill_screen,
	//.write_screen = write_screen,
	//.draw_pixel = draw_pixel,
	//.get_pixel = get_pixel,
};

