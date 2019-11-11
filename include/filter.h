#ifndef FILTER_H
#define FILTER_H

#include "cimg.h"

#include <stdbool.h>

cimg_t *gaussian_filter(cimg_t *src, int filter_w, int filter_h, double sigma);

cimg_t *median_filter(cimg_t *src, int filter_w, int filter_h);

cimg_t *average_filter(cimg_t *src, int filter_w, int filter_h);

cimg_t *motion_filter(cimg_t *src, int filter_w, int filter_h);

cimg_t *maxmin_filter(cimg_t *src, int filter_w, int filter_h);

cimg_t *diff_filter(cimg_t *src, bool is_horizontal);

cimg_t *sobel_filter(cimg_t *src, bool is_horizontal);

cimg_t *prewitt_filter(cimg_t *src, bool is_horizontal);

cimg_t *laplacian_filter(cimg_t *src);

cimg_t *emboss_filter(cimg_t *src);

cimg_t *log_filter(cimg_t *src, int filter_w, int filter_h, double sigma);

#endif // FILTER_H
