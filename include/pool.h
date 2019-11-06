#ifndef POOL_H
#define POOL_H

#include "img.h"

img_t *average_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

img_t *max_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h);

#endif // POOL_H
