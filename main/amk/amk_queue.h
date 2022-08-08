/**
 * @file amk_queue.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    void        *items;
    uint32_t    item_size;
    uint32_t    head;
    uint32_t    tail;
    uint32_t    total;
} amk_queue_t;

void amk_queue_init(amk_queue_t* queue, void* items, uint32_t item_size, uint32_t total);
uint32_t amk_queue_size(amk_queue_t* queue);
bool amk_queue_empty(amk_queue_t* queue);
bool amk_queue_full(amk_queue_t* queue);
bool amk_queue_put(amk_queue_t* queue, void *item);
bool amk_queue_get(amk_queue_t* queue, void *item);

void* amk_queue_peek(amk_queue_t* queue);
void amk_queue_pop(amk_queue_t* queue);
