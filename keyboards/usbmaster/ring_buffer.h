/**
 * @file ring_buffer.h
 */

#pragma once

#include <stdint.h>

typedef struct {
    uint32_t    head;
    uint32_t    tail;
    uint32_t    size;
    uint8_t*    data;
} ring_buffer_t;

void rb_init(ring_buffer_t *ring, uint8_t *data, uint32_t size);
void rb_write_byte(ring_buffer_t *ring, uint8_t byte);
uint8_t rb_read_byte(ring_buffer_t *ring);
uint32_t rb_used_count(ring_buffer_t *ring);
uint32_t rb_free_count(ring_buffer_t *ring);

void rb_write(ring_buffer_t *ring, const uint8_t *data, uint32_t size);
void rb_read(ring_buffer_t *ring, uint8_t *data, uint32_t size);