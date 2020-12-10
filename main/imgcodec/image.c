/**
 * @file image.c
 */

#include <string.h>
#include "image.h"

int img_read(image_t *image, void *buf, size_t size)
{
    uint8_t* p = (uint8_t*)image->data;
    size_t to_read = image->pos+size >= image->total ? image->total - image->pos : size;
    memcpy(buf, p + image->pos, to_read);
    image->pos += to_read;
    return to_read;
}

int img_write(image_t *image, const void *buf, size_t size)
{
    // not implemented now
    return -1;
}

int img_seek(image_t *image, int offset, img_seek_t pos)
{
    switch(pos) {
    case IMG_SET:
        image->pos = offset;
        break;
    case IMG_CUR:
        image->pos += offset;
        break;
    case IMG_END:
        if (offset < 0) {
            image->total += offset;
        } else {
        }
        break;
    default:
        break;
    }
    return image->pos;
}

int img_resize(const void* src_buf, size_t src_w, size_t src_h, void* dst_buf, size_t dst_w, size_t dst_h)
{
    //simple resize
    return -1;
}