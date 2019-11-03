#ifndef IMG_H
#define IMG_H

#include <stdint.h>

typedef enum {
    COLORSPACE_GRAY,
    COLORSPACE_RGB
} COLORSPACE;

typedef struct {
    uint32_t    width;
    uint32_t    height;
    uint32_t    channel;
    COLORSPACE  colorspace;
    uint8_t     *data;
    uint8_t     **row;
    uint8_t     ***ch;
} img_t;

img_t *img_allocate(uint32_t width, uint32_t height, COLORSPACE colorspace);

img_t *img_clone(img_t *src);

void img_free(img_t *img);

#endif // IMG_H
