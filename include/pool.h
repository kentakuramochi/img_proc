///
/// @file   pool.h
/// @brief  pooling functions
/// @author kentakuramochi
///

#ifndef POOL_H
#define POOL_H

#include "cimg.h"

///
/// @fn     average_pooling
/// @brief  adopt average pooling
/// @param  src         [in]    pointer to source image
/// @param  kernel_w    [in]    pooling kernel width
/// @param  kernel_h    [in]    pooling kernel height
/// @return pointer to processed image, NULL if failed
///
cimg_t *average_pooling(cimg_t *src, int kernel_w, int kernel_h);

///
/// @fn     max_pooling
/// @brief  adopt max pooling
/// @param  src         [in]    pointer to source image
/// @param  kernel_w    [in]    pooling kernel width
/// @param  kernel_h    [in]    pooling kernel height
/// @return pointer to processed image, NULL if failed
///
cimg_t *max_pooling(cimg_t *src, int kernel_w, int kernel_h);

#endif // POOL_H
