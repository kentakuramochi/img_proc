#include "img.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

img_t *img_allocate(int width, int height, int channels)
{
    if ((width <= 0) || (height <= 0) || (channels <= 0)) {
        return NULL;
    }

    img_t *img = (img_t*)malloc(sizeof(img_t));
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

img_t *img_clone(img_t *src)
{
    img_t *dst = img_allocate(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst->data, src->data, sizeof(uint8_t) * (dst->height * dst->width * dst->channels));

    return dst;
}

void img_free(img_t *img)
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
