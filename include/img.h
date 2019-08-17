#ifndef IMG_H
#define IMG_H

#include <stdint.h>

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
    COLOR_TYPE_BINARY = 1,
    COLOR_TYPE_GRAY   = 2,
    COLOR_TYPE_RGB    = 3
} color_type_t;

typedef struct {
    uint32_t     width;
    uint32_t     height;
    color_type_t color_type;
    pixel_t      *data;
    pixel_t      **map;
} img_t;

img_t *img_allocate(uint32_t width, uint32_t height, color_type_t color_type);

img_t *img_clone(img_t *src);

void img_free(img_t *img);

#endif // IMG_H
