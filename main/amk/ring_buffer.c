/**
 * @file ring_buffer.c
 */

#include "ring_buffer.h"


void rb_init(ring_buffer_t *ring, uint8_t *data, uint32_t size)
{
    ring->head = ring->tail = 0;
    ring->size = size;
    ring->data = data;
}

void rb_write_byte(ring_buffer_t *ring, uint8_t byte)
{
    ring->data[ring->head] = byte;
    ring->head = (ring->head+1) % ring->size;
}

uint8_t rb_read_byte(ring_buffer_t *ring)
{
    uint8_t byte = ring->data[ring->tail];
    ring->tail = (ring->tail+1) % ring->size;
    return byte;
}

uint8_t rb_peek_byte(ring_buffer_t *ring)
{
    return ring->data[ring->tail];
}

uint32_t rb_used_count(ring_buffer_t *ring)
{
    if (ring->head >= ring->tail) {
        return ring->head-ring->tail;
    } else {
        return ring->head + ring->size -ring->tail;
    }
}

uint32_t rb_free_count(ring_buffer_t *ring)
{
    return (ring->size - rb_used_count(ring));
}

void rb_write(ring_buffer_t *ring, const uint8_t *data, uint32_t size)
{
    for(uint32_t i = 0; i < size; i++) {
        rb_write_byte(ring, data[i]);
    }
}

void rb_read(ring_buffer_t *ring, uint8_t *data, uint32_t size)
{
    for(uint32_t i = 0; i < size; i++) {
        data[i] = rb_read_byte(ring);
    }
}