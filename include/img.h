///
/// @file   img.h
/// @brief  base image structure
///

#ifndef IMG_H
#define IMG_H

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
/// @struct img_t
/// @brief  image structure (8bit per channel)
///
typedef struct {
    int     width;      ///< width
    int     height;     ///< height
    int     channels;   ///< number of channels
    int     stride;     ///< stride (byte size of a row)
    uint8_t *data;      ///< pixel data (row-major order, channel-interleaving)
} img_t;

///
/// @fn     img_create
/// @brief  create new image structure
/// @param  width    [in]   image width
/// @param  height   [in]   image height
/// @param  channels [in]   image channels
/// @return pointer to new image structure, NULL if failed
/// @note   pixel value is indefinite
///
img_t *img_create(int width, int height, int channels);

///
/// @fn     img_clone
/// @brief  clone image structure
/// @param  img [in]    pointer of clone source image structure
/// @return pointer to clone image structure, NULL if failed
///
img_t *img_clone(img_t *img);

///
/// @fn     img_delete
/// @brief  delete image structure
/// @param  img [in]    pointer to image structure to be deleted
///
void img_delete(img_t *img);

///
/// @def    IMG_GETELEM
/// @brief  get specified elemet of image structure
/// @note   no range check
///
#define IMG_GETELEM(img, x, y, ch) ((img)->data[(y) * (img)->stride + (x) * (img)->channels + (ch)])

///
/// @fn     img_getelem
/// @brief  get specified element of image structure
/// @param  img [in]    pointer to target image structure
/// @param  x   [in]    x coordinate [0, img->width)
/// @param  y   [in]    y coordinate [0, img->height)
/// @param  ch  [in]    channel no. [0, img->channels)
/// @return value of specified element of target image structure, -1 if out of range
///
inline int img_getelem(img_t *img, int x, int y, int ch)
{
    if ((x < 0)  || (x >= img->width)  ||
        (y < 0)  || (y >= img->height) ||
        (ch < 0) || (ch >= img->channels)) {
        return -1;
    }
    return img->data[y * img->stride + x * img->channels + ch];
}

#endif // IMG_H
