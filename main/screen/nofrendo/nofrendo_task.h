/**
 * @file nofrendo_task.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

void nofrendo_init(void);
bool nofrendo_start(const char* rom);
void nofrendo_task(uint32_t ticks);
void nofrendo_pause(void);
bool nofrendo_resume(void);
void nofrendo_stop(void);
void nofrendo_deinit(void);