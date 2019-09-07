#ifndef IMGPROC_H
#define IMGPROC_H

#include "img.h"

int rgb_to_gray(img_t *src, img_t *dst);

int binarize(img_t *src, img_t *dst, uint8_t threshold);

int binarize_otsu(img_t *src, img_t *dst);

int quantize(img_t *src, img_t *dst, uint8_t level);

#endif // IMGPROC_H
