/**
 * @file nofrendo_port.c
 * @brief osd porting layer
 */

#include <stdint.h>
#include "osd.h"

#define NOFRENDO_HEAP_SIZE  (32*1024)

static uint8_t nofrendo_heap[NOFRENDO_HEAP_SIZE];

uint32_t UMM_MALLOC_CFG_HEAP_SIZE = NOFRENDO_HEAP_SIZE;
void *UMM_MALLOC_CFG_HEAP_ADDR = nofrendo_heap;