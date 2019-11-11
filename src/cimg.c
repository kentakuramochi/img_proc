#include "cimg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cimg_t *cimg_allocate(int width, int height, int channels)
{
    if ((width <= 0) || (height <= 0) || (channels <= 0)) {
        return NULL;
    }

    cimg_t *img = (cimg_t*)malloc(sizeof(cimg_t));
    if (img == NULL) {
        return NULL;
    }

    img->width    = width;
    img->height   = height;
    img->channels = channels;
    img->stride   = width * channels;

    img->data = (uint8_t*)malloc(sizeof(uint8_t) * (img->channels * img->height * img->width));
    if (img->data == NULL) {
        free(img);
        img = NULL;

        return NULL;
    }

    return img;
}

cimg_t *cimg_clone(cimg_t *img)
{
    cimg_t *dst = cimg_allocate(img->width, img->height, img->channels);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst->data, img->data, sizeof(uint8_t) * (dst->height * dst->width * dst->channels));

    return dst;
}

void cimg_free(cimg_t *img)
{
    if (img == NULL) {
        return;
    }

    free(img->data);
    img->data = NULL;

    free(img);
    img = NULL;

    return;
}
