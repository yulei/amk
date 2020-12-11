/**
 * @file image.h
 *  interface for the binary image
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    IMG_SET,
    IMG_CUR,
    IMG_END,
} img_seek_t;

typedef struct {
    void    *data;
    size_t  total;
    size_t  pos;
} image_t;
    
int img_read(image_t *image, void *buf, size_t size);
int img_write(image_t *image, const void *buf, size_t size);
int img_seek(image_t *image, int offset, img_seek_t pos);

int img_resize(const void* src_buf, size_t src_w, size_t src_h, void* dst_buf, size_t dst_w, size_t dst_h);