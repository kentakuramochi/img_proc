///
/// @file   cimg.h
/// @brief  base image structure
/// @author kentakuramochi
///

#ifndef CIMG_H
#define CIMG_H

#include <stdint.h>

///
/// @enum   CH_NUM
/// @brief  num of channels corresponding with each colorspace
///
typedef enum {
    CH_GRAY = 1,    ///< grayscale 1ch
    CH_RGB  = 3,    ///< RGB 3ch
    //CH_RGBA = 4,
    //CH_YUV  = 3,
} CH_NUM;

///
/// @struct cimg_t
/// @brief  image structure (8bit per channel)
///
typedef struct {
    int     width;      ///< width
    int     height;     ///< height
    int     channels;   ///< number of channels
    int     stride;     ///< stride (byte size of a row)
    uint8_t *data;      ///< serialized pixel data
} cimg_t;

///
/// @fn     cimg_create
/// @brief  create new image structure
/// @param  width    [in]   image width
/// @param  height   [in]   image height
/// @param  channels [in]   image channels
/// @return pointer to new image structure
/// @note   pixel value is indefinite (NULL if failed)
///
cimg_t *cimg_create(int width, int height, int channels);

///
/// @fn     cimg_clone
/// @brief  clone image structure
/// @param  img [in]    pointer of clone source image structure
/// @return pointer to clone image structure (NULL if failed)
///
cimg_t *cimg_clone(cimg_t *img);

///
/// @fn     cimg_delete
/// @brief  delete image structure
/// @param  img [in]    pointer to image structure to be deleted
///
void cimg_delete(cimg_t *img);

///
/// @fn     cimg_getelem
/// @brief  get specified element
/// @param  img [in]    pointer to image structure
/// @param  x   [in]    x coordinate
/// @param  y   [in]    y coordinate
/// @param  ch  [in]    channel no
/// @return pixel value of specified element (8bit unsigned)
inline uint8_t cimg_getelem(cimg_t *img, int x, int y, int ch)
{
    return img->data[y * img->stride + x * img->channels + ch];
}

#endif // CIMG_H
