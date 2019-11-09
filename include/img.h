#ifndef IMG_H
#define IMG_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    COLORSPACE_GRAY,
    COLORSPACE_RGB
} COLORSPACE;

typedef struct {
    int         width;
    int         height;
    int         channel;
    COLORSPACE  colorspace;
    uint8_t     *data;
    uint8_t     **row;
    uint8_t     ***ch;
} img_t;

img_t *img_allocate(int width, int height, COLORSPACE colorspace);

img_t *img_clone(img_t *src);

void img_free(img_t *img);

#endif // IMG_H
