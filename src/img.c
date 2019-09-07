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

    img->width      = width;
    img->height     = height;
    img->colorspace = colorspace;

    img->data = (pixel_t*)malloc(sizeof(pixel_t) * (img->height * img->width));
    if (img->data == NULL) {
        free(img);
        img = NULL;

        return NULL;
    }

    img->map = (pixel_t**)malloc(sizeof(pixel_t*) * img->height);
    if (img->map == NULL) {
        free(img->data);
        img->data = NULL;

        free(img);
        img = NULL;

        return NULL;
    }

    for (int y = 0; y < img->height; y++) {
        img->map[y] = &img->data[y * img->width];
    }

    return img;
}

img_t *img_clone(img_t *src)
{
    img_t *img = img_allocate(src->width, src->height, src->colorspace);
    if (img == NULL) {
        return NULL;
    }

    memcpy(img->data, src->data, (sizeof(pixel_t) * img->width * img->height));

    return img;
}

void img_free(img_t *img)
{
    free(img->map);
    img->map = NULL;

    free(img->data);
    img->data = NULL;

    free(img);
    img = NULL;

    return;
}
