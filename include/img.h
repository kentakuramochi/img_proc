#ifndef IMG_H
#define IMG_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    CH_GRAY = 1,
    CH_RGB  = 3,
    //CH_RGBA = 4,
    //CH_YUV  = 3,
} CH_NUM;

typedef struct {
    int     width;
    int     height;
    int     channels;
    uint8_t *data;
    uint8_t **row;
    uint8_t ***ch;
} img_t;

img_t *img_allocate(int width, int height, int channels);

img_t *img_clone(img_t *src);

void img_free(img_t *img);

#endif // IMG_H
