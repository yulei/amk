/**
 * bootloader.c
 *  jump to bootloader
 */

#include "bootloader.h"

__attribute__((weak))
void bootloader_jump_kb(void) {}

void bootloader_jump(void)
{
    bootloader_jump_kb();
}