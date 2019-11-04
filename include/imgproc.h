#ifndef IMGPROC_H
#define IMGPROC_H

#include "img.h"

#include <stdbool.h>

img_t *rgb_to_gray(img_t *src);

img_t *binarize(img_t *src, uint8_t threshold);
img_t *binarize_otsu(img_t *src);

img_t *quantize(img_t *src, uint8_t level);

img_t *average_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h);
img_t *max_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *gaussian_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h, double stddev);
img_t *median_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);
img_t *average_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);
img_t *motion_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);
img_t *maxmin_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *diff_filter(img_t *src, bool is_horizontal);
img_t *sobel_filter(img_t *src, bool is_horizontal);
img_t *prewitt_filter(img_t *src, bool is_horizontal);
img_t *laplacian_filter(img_t *src);

#endif // IMGPROC_H
