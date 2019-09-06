#ifndef IMGPROC_H
#define IMGPROC_H

#include "img.h"

void rgb_to_gray(img_t *src, img_t *dst);

void binarize(img_t *src, img_t *dst, uint8_t threshold);

void binarize_otsu(img_t *src, img_t *dst);

void quantize(img_t *src, img_t *dst, uint8_t level);

#endif // IMGPROC_H
