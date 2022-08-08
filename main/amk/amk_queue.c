/**
 * @file amk_queue.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <string.h>
#include <stddef.h>
#include "amk_queue.h"

void amk_queue_init(amk_queue_t* queue, void* items, uint32_t item_size, uint32_t total)
{
    memset(queue, 0, sizeof(amk_queue_t));
    queue->items = items;
    queue->item_size = item_size;
    queue->total = total;

    queue->head = queue->tail = 0;
}

uint32_t amk_queue_size(amk_queue_t* queue)
{
    if (queue->head <= queue->tail) {
        return queue->tail - queue->head;
    } else {
        return queue->tail + queue->total - queue->head;
    }
}

bool amk_queue_empty(amk_queue_t* queue)
{
    return queue->head == queue->tail;
}

bool amk_queue_full(amk_queue_t* queue)
{
    return ((queue->tail + 1) % queue->total) == queue->head;
}

bool amk_queue_put(amk_queue_t* queue, void *item)
{
    if (amk_queue_full(queue)) return false;

    uint8_t *p = ((uint8_t*)queue->items) + queue->tail*queue->item_size;

    memcpy(p, item, queue->item_size);

    queue->tail = (queue->tail + 1) % queue->total;
    return true;
}

bool amk_queue_get(amk_queue_t* queue, void* item)
{
    if (amk_queue_empty(queue)) return false;

    uint8_t *p = ((uint8_t*)queue->items) + queue->head*queue->item_size;
    memcpy(item, p, queue->item_size);

    queue->head = (queue->head + 1) % queue->total;
    return true;
}

void* amk_queue_peek(amk_queue_t* queue)
{
    if (amk_queue_empty(queue)) return NULL;

    uint8_t *p = ((uint8_t*)queue->items) + queue->head*queue->item_size;
    return p;
}

void amk_queue_pop(amk_queue_t* queue)
{
    if (amk_queue_empty(queue)) return;

    queue->head = (queue->head + 1) % queue->total;
}
