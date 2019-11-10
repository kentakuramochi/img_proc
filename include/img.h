#ifndef IMG_H
#define IMG_H

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
    int     stride;
    uint8_t *data;
} img_t;

inline uint8_t getelem(img_t *img, int x, int y, int ch)
{
    return img->data[y * img->stride + x * img->channels + ch];
}

img_t *img_allocate(int width, int height, int channels);

img_t *img_clone(img_t *src);

void img_free(img_t *img);

#endif // IMG_H
