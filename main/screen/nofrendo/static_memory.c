/**
 * @file static_memory.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#include "static_memory.h"

static uint8_t* cur = NULL;
static size_t remain = 0;

void static_memory_create(void* mem, size_t total)
{
    cur = mem;
    remain = total;
}

void* static_memory_alloc(size_t size)
{
    // align to 4
    size_t real_size = (size+3) & 0xFFFFFFFC;
    if (real_size > remain) {
        return NULL;
    }

    void* mem = cur;
    cur += real_size;
    remain -= real_size;
    return mem;
}

void static_memory_free(void* mem)
{
    // do nothing
}

void static_memory_destroy(void)
{
    cur = NULL;
    remain = 0;
}