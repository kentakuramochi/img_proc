#ifndef FILTER_H
#define FILTER_H

#include "img.h"

#include <stdbool.h>

img_t *gaussian_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h, double stddev);

img_t *median_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *average_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *motion_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *maxmin_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *diff_filter(img_t *src, bool is_horizontal);

img_t *sobel_filter(img_t *src, bool is_horizontal);

img_t *prewitt_filter(img_t *src, bool is_horizontal);

img_t *laplacian_filter(img_t *src);

img_t *emboss_filter(img_t *src);

img_t *log_filter(img_t *src, int kernel_w, int kernel_h, double sigma);

#endif // FILTER_H
