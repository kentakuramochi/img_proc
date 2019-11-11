#ifndef CONVERT_H
#define CONVERT_H

#include "cimg.h"

cimg_t *rgb_to_gray(cimg_t *src);

cimg_t *binarize(cimg_t *src, uint8_t threshold);

cimg_t *binarize_otsu(cimg_t *src);

cimg_t *quantize(cimg_t *src, uint8_t level);

void get_hist(cimg_t *src, int histgram[256], int bin);

#endif // CONVERT_H
