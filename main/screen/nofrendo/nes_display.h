/**
 * @file nes_display.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#pragma once

#include <stdint.h>

#define EMU_SCALEDOWN 2
#define RGBVAL16(r,g,b)  ( (((r>>3)&0x1f)<<11) | (((g>>2)&0x3f)<<5) | (((b>>3)&0x1f)<<0) )

#define NATIVE_WIDTH      256
#define NATIVE_HEIGHT     240

#define EMUDISPLAY_WIDTH   (NATIVE_WIDTH / EMU_SCALEDOWN)
#define EMUDISPLAY_HEIGHT  (NATIVE_HEIGHT / EMU_SCALEDOWN)

struct nes_display
{
    void(*set_frame_buffer)(uint16_t*);
	uint16_t* (*get_frame_buffer)(void);
    void (*refresh)(void);
    void (*stop)(void);
    void (*wait)(void);
    uint16_t*(*get_line_buffer)(int);
    void (*set_area)(uint16_t,uint16_t,uint16_t,uint16_t);
    void (*set_area_centered)(void);
#if EMU_SCALEDOWN == 1
    void (*write_line)(int, int, int, uint8_t*, uint16_t*);
#elif EMU_SCALEDOWN == 2
    void (*write_line)(int, int, int, uint8_t*, uint32_t*);
#endif
	void (*fill_screen)(uint16_t);
	//void (*)(const uint16_t*) write_screen;
	//void (*)(int16_t, int16_t, uint16_t) draw_pixel;
	//uint16_t (*)(int16_t, int16_t) get_pixel;
};

extern struct nes_display nes_display;
