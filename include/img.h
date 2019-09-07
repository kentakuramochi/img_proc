#ifndef IMG_H
#define IMG_H

#include <stdint.h>

typedef enum {
    RETURN_SUCCESS = 0,
    RETURN_FAILURE
} RETURN;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

typedef struct {
    color_t rgb;
    uint8_t gray;
} pixel_t;

typedef enum {
    COLORSPACE_GRAY,
    COLORSPACE_RGB
} COLORSPACE;

typedef struct {
    uint32_t     width;
    uint32_t     height;
    pixel_t      *data;
    pixel_t      **map;
    COLORSPACE   colorspace;
} img_t;

img_t *img_allocate(uint32_t width, uint32_t height, COLORSPACE colorspace);

img_t *img_clone(img_t *src);

void img_free(img_t *img);

#endif // IMG_H
