#ifndef IMGPROC_H
#define IMGPROC_H

#include "img.h"

img_t *rgb_to_gray(img_t *src);

img_t *binarize(img_t *src, uint8_t threshold);
img_t *binarize_otsu(img_t *src);

img_t *quantize(img_t *src, uint8_t level);

img_t *average_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h);
img_t *max_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *gaussian_filter(img_t *src, uint32_t kernel_size, double stddev);
img_t *median_filter(img_t *src, uint32_t kernel_size);

#endif // IMGPROC_H
