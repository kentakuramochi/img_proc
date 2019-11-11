#ifndef CIMG_H
#define CIMG_H

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
} cimg_t;

inline uint8_t cimg_getelem(cimg_t *img, int x, int y, int ch)
{
    return img->data[y * img->stride + x * img->channels + ch];
}

cimg_t *cimg_allocate(int width, int height, int channels);

cimg_t *cimg_clone(cimg_t *img);

void cimg_free(cimg_t *img);

#endif // CIMG_H
