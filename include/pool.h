#ifndef POOL_H
#define POOL_H

#include "cimg.h"

cimg_t *average_pooling(cimg_t *src, int kernel_w, int kernel_h);

cimg_t *max_pooling(cimg_t *src, int kernel_w, int kernel_h);

#endif // POOL_H
