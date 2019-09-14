#ifndef IMGPROC_H
#define IMGPROC_H

#include "img.h"

img_t *rgb_to_gray(img_t *src);

img_t *binarize(img_t *src, uint8_t threshold);

img_t *binarize_otsu(img_t *src);

img_t *quantize(img_t *src, uint8_t level);

#endif // IMGPROC_H
