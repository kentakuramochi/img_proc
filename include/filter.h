///
/// @file   filter.h
/// @brief  image filtering functions
///

#ifndef FILTER_H
#define FILTER_H

#include "img.h"

#include <stdbool.h>

///
/// @fn     gaussian_filter 
/// @brief  adopt gaussian filter
/// @param  src         [in]    pointer to source image
/// @param  filter_w    [in]    filter kernel width
/// @param  filter_h    [in]    filter kernel height
/// @param  sigma       [in]    stddev of gaussian kernel
/// @return pointer to processed image, NULL if failed
///
img_t *gaussian_filter(img_t *src, int filter_w, int filter_h, double sigma);

///
/// @fn     median_filter 
/// @brief  adopt median filter
/// @param  src         [in]    pointer to source image
/// @param  filter_w    [in]    filter kernel width
/// @param  filter_h    [in]    filter kernel height
/// @return pointer to processed image, NULL if failed
///
img_t *median_filter(img_t *src, int filter_w, int filter_h);

///
/// @fn     average_filter 
/// @brief  adopt average filter
/// @param  src         [in]    pointer to source image
/// @param  filter_w    [in]    filter kernel width
/// @param  filter_h    [in]    filter kernel height
/// @return pointer to processed image, NULL if failed
///
img_t *average_filter(img_t *src, int filter_w, int filter_h);

///
/// @fn     motion_filter 
/// @brief  adopt motion filter
/// @param  src         [in]    pointer to source image
/// @param  filter_w    [in]    filter kernel width
/// @param  filter_h    [in]    filter kernel height
/// @return pointer to processed image, NULL if failed
///
img_t *motion_filter(img_t *src, int filter_w, int filter_h);

///
/// @fn     maxmin_filter 
/// @brief  adopt max-min filter
/// @param  src         [in]    pointer to source grayscale image
/// @param  filter_w    [in]    filter kernel width
/// @param  filter_h    [in]    filter kernel height
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *maxmin_filter(img_t *src, int filter_w, int filter_h);

///
/// @fn     diff_filter 
/// @brief  adopt differential filter
/// @param  src             [in]    pointer to source grayscale image
/// @param  filter_w        [in]    filter kernel width
/// @param  filter_h        [in]    filter kernel height
/// @param  is_horizontal   [in]    differential direction: horizontal (true)/ vertical (false)
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *diff_filter(img_t *src, bool is_horizontal);

///
/// @fn     sobel_filter 
/// @brief  adopt sobel filter
/// @param  src             [in]    pointer to source grayscale image
/// @param  filter_w        [in]    filter kernel width
/// @param  filter_h        [in]    filter kernel height
/// @param  is_horizontal   [in]    differential direction: horizontal (true)/ vertical (false)
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *sobel_filter(img_t *src, bool is_horizontal);

///
/// @fn     prewitt_filter 
/// @brief  adopt prewitt filter
/// @param  src             [in]    pointer to source grayscale image
/// @param  filter_w        [in]    filter kernel width
/// @param  filter_h        [in]    filter kernel height
/// @param  is_horizontal   [in]    differential direction: horizontal (true)/ vertical (false)
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *prewitt_filter(img_t *src, bool is_horizontal);

///
/// @fn     laplacian_filter 
/// @brief  adopt laplacian filter
/// @param  src [in]    pointer to source grayscale image
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *laplacian_filter(img_t *src);

///
/// @fn     emboss_filter 
/// @brief  adopt emboss filter
/// @param  src [in]    pointer to source grayscale image
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *emboss_filter(img_t *src);

///
/// @fn     log_filter 
/// @brief  adopt LoG (Laplacian of Gaussian) filter
/// @param  src         [in]    pointer to source grayscale image
/// @param  filter_w    [in]    filter kernel width
/// @param  filter_h    [in]    filter kernel height
/// @param  sigma       [in]    stddev of gaussian kernel
/// @return pointer to processed image, NULL if failed
/// @note   only grayscale input is allowed
///
img_t *log_filter(img_t *src, int filter_w, int filter_h, double sigma);

#endif // FILTER_H
