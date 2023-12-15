/**
 * @file static_memory.h
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#pragma once

#include <stddef.h>
#include <stdint.h>

void static_memory_create(void* mem, size_t total);
void* static_memory_alloc(size_t size);
void static_memory_free(void* mem);
void static_memory_destroy(void);