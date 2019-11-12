///
/// @file   convert.h
/// @brief  image conversion functions
/// @author kentakuramochi
///

#ifndef CONVERT_H
#define CONVERT_H

#include "cimg.h"

///
/// @fn     rgb_to_gray
/// @brief  get grayscale image from RGB image
/// @param  src [in]   pointer to source RGB image
/// @return pointer to grayscale image, NULL if failed
///
cimg_t *rgb_to_gray(cimg_t *src);

///
/// @fn     binarize
/// @brief  get binary image (0/255) from grayscale image
/// @param  src         [in]   pointer to source grayscale image
/// @param  threshold   [in]   binarize threshold [0, 255]
/// @return pointer to binary image, NULL if failed
///
cimg_t *binarize(cimg_t *src, uint8_t threshold);

///
/// @fn     binarize
/// @brief  get binary image (0/255) from grayscale image with Otsu's method
/// @param  src         [in]   pointer to source grayscale image
/// @return pointer to binary image, NULL if failed
///
cimg_t *binarize_otsu(cimg_t *src);

///
/// @fn     quantize
/// @brief  get quantized image (0/255)
/// @param  src         [in]   pointer to source image
/// @param  level       [in]   quantization level [1, 256]
/// @return pointer to quantized image, NULL if failed
///
cimg_t *quantize(cimg_t *src, uint8_t level);

///
/// @fn     get_hist
/// @brief  get histogram of specified image
/// @param  src         [in]    pointer to source image
/// @param  histogram   [out]   pointer to histogram 
/// @param  bin         [in]    num of bins of histogram
/// @note   length of histogram must be 256
///
void get_hist(cimg_t *src, int histogram[256], int bin);

#endif // CONVERT_H
