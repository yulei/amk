/**
 * @file nofrendo_emu.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

void nofrendo_init(void* frame_buffer, char* file_name);
void nofrendo_task(uint32_t ticks);
void nofrendo_deinit(void);
