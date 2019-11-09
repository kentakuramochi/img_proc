#ifndef POOL_H
#define POOL_H

#include "img.h"

img_t *average_pooling(img_t *src, int kernel_w, int kernel_h);

img_t *max_pooling(img_t *src, int kernel_w, int kernel_h);

#endif // POOL_H
