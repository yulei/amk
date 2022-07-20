/**
 * @file linear_buffer.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <string.h>
#include "linear_buffer.h"

void linear_buf_init(linear_buf_t *buf)
{
    memset(buf, 0, sizeof(linear_buf_t));
}

bool linear_buf_full(linear_buf_t *buf)
{
    return buf->size == LINEAR_BUFFER_MAX;
}

bool linear_buf_put(linear_buf_t *buf, uint8_t data)
{
    if (linear_buf_full(buf)) return false;
    buf->data[buf->size++] = data;
    return true;
}

uint32_t linear_buf_size(linear_buf_t *buf)
{
    return buf->size;
}

void* linear_buf_data(linear_buf_t *buf)
{
    return buf->data;
}

bool linear_buf_get(linear_buf_t *buf, uint32_t offset, uint8_t *data)
{
    if (offset < buf->size) {
        *data = buf->data[offset];
        return true;
    } else {
        return false;
    }
}
