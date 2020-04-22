///
/// @file   convert.h
/// @brief  image conversion functions
///

#ifndef CONVERT_H
#define CONVERT_H

#include "img.h"

///
/// @fn     rgb_to_gray
/// @brief  get grayscale image from RGB image
/// @param  src [in]   pointer to source RGB image
/// @return pointer to grayscale image, NULL if failed
///
img_t *rgb_to_gray(img_t *src);

///
/// @fn     binarize
/// @brief  get binary image (0/255) from grayscale image
/// @param  src         [in]   pointer to source grayscale image
/// @param  threshold   [in]   binarize threshold [0, 255]
/// @return pointer to binary image, NULL if failed
///
img_t *binarize(img_t *src, uint8_t threshold);

///
/// @fn     binarize
/// @brief  get binary image (0/255) from grayscale image with Otsu's method
/// @param  src         [in]   pointer to source grayscale image
/// @return pointer to binary image, NULL if failed
///
img_t *binarize_otsu(img_t *src);

///
/// @fn     quantize
/// @brief  get quantized image (0/255)
/// @param  src         [in]   pointer to source image
/// @param  level       [in]   quantization level [1, 256]
/// @return pointer to quantized image, NULL if failed
///
img_t *quantize(img_t *src, uint8_t level);

///
/// @fn     get_hist
/// @brief  get histogram of specified image
/// @param  src         [in]    pointer to source image
/// @param  histogram   [out]   pointer to histogram 
/// @param  bin         [in]    num of bins of histogram
/// @note   length of histogram must be 256
///
void get_hist(img_t *src, int histogram[256], int bin);

///
/// @fn     expand_hist 
/// @brief  expand histogram
/// @param  src [in]    pointer to source image
/// @param  min [in]    min value of histogram range
/// @param  max [in]    max value of histogram range
/// @return pointer to processed image, NULL if failed
///
img_t *expand_hist(img_t *src, uint8_t min, uint8_t max);

///
/// @fn     normarize_hist
/// @brief  normalize histogram with mean and stddev (sigma)
/// @param  src     [in]    pointer to source image
/// @param  mean    [in]    mean of normalized histogram
/// @param  sigma   [in]    stddev of normalized histogram
/// @return pointer to processed image, NULL if failed
///
img_t *normarize_hist(img_t *src, uint8_t mean, double sigma);

///
/// @fn     equalize_hist 
/// @brief  equalize histogram
/// @param  src [in]    pointer to source image
/// @return pointer to processed image, NULL if failed
///
img_t *equalize_hist(img_t *src);

///
/// @fn     gamma_correction
/// @brief  adopt gamma correction
/// @param  src     [in]    pointer to source image
/// @param  gamma   [in]    gamma value of target
/// @return pointer to processed image, NULL if failed
///
img_t *gamma_correction(img_t *src, double gamma);

///
/// @fn     nearest_neighbor
/// @brief  expand image with nearest neighbor interpolation
/// @param  src     [in]    pointer to source image
/// @param  scale_x [in]    scale ratio for x direction
/// @param  scale_y [in]    scale ratio for y direction
/// @return pointer to expanded image, NULL if failed
///
img_t *nearest_neighbor(img_t *src, double scale_x, double scale_y);

///
/// @fn     bilinear
/// @brief  expand image with bilinear interpolation
/// @param  src     [in]    pointer to source image
/// @param  scale_x [in]    scale ratio for x direction
/// @param  scale_y [in]    scale ratio for y direction
/// @return pointer to expanded image, NULL if failed
///
img_t *bilinear(img_t *src, double scale_x, double scale_y);

///
/// @fn     biqubic
/// @brief  expand image with biqubic interpolation
/// @param  src     [in]    pointer to source image
/// @param  scale_x [in]    scale ratio for x direction
/// @param  scale_y [in]    scale ratio for y direction
/// @return pointer to expanded image, NULL if failed
///
img_t *biqubic(img_t *src, double scale_x, double scale_y);

///
/// @fn     affine
/// @brief  affine transform
/// @param  src     [in]    pointer to source image
/// @param  scale_x [in]    scale ratio for x direction
/// @param  scale_y [in]    scale ratio for y direction
/// @return pointer to transformed image, NULL if failed
///
img_t *affine(img_t *src, int tx, int ty);

#endif // CONVERT_H
