#include "img.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

img_t *img_allocate(uint32_t width, uint32_t height, COLORSPACE colorspace)
{
    img_t *img = (img_t*)malloc(sizeof(img_t));

    if (img == NULL) {
        return NULL;
    }

    img->width  = width;
    img->height = height;

    switch (colorspace) {
        case COLORSPACE_RGB:
            img->channel = 3;
            break;
        case COLORSPACE_GRAY:
            img->channel = 1;
            break;
        default:
            free(img);
            img = NULL;
            return NULL;
            break;
    }

    img->colorspace = colorspace;

    img->data = (uint8_t*)malloc(sizeof(uint8_t) * (img->channel * img->height * img->width));

    if (img->data == NULL) {
        free(img);
        img = NULL;

        return NULL;
    }

    img->row = (uint8_t**)malloc(sizeof(uint8_t*) * (img->channel * img->height));

    if (img->row == NULL) {
        free(img->data);
        img->data = NULL;

        free(img);
        img = NULL;

        return NULL;
    }

    for (int y = 0; y < (img->channel * img->height); y++) {
        img->row[y] = img->data + (y * img->width);
    }

    img->ch = (uint8_t***)malloc(sizeof(uint8_t**) * img->channel);

    if (img->ch == NULL) {
        free(img->row);
        img->row = NULL;

        free(img->data);
        img->data = NULL;

        free(img);
        img = NULL;

        return NULL;
    }

    for (int c = 0; c < img->channel; c++) {
        img->ch[c] = img->row + (c * img->height);
    }

    return img;
}

img_t *img_clone(img_t *src)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);

    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst->data, src->data, sizeof(uint8_t) * (dst->channel * dst->height * dst->width));

    return dst;
}

void img_free(img_t *img)
{
    if (img == NULL) {
        return;
    }

    free(img->ch);
    img->ch = NULL;

    free(img->row);
    img->row = NULL;

    free(img->data);
    img->data = NULL;

    free(img);
    img = NULL;

    return;
}
