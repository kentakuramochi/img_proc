///
/// @file   pnm.h
/// @brief  PNM image utilities
/// @author kentakuramochi
///

#ifndef PNM_H
#define PNM_H

#include "img.h"

#include <stdbool.h>

///
/// @enum   PNM_FORMAT
/// @brief  fomat of PNM image
///
typedef enum {
    PNM_FORMAT_ASCII,   ///< ASCII format (P1/P2/P3)
    PNM_FORMAT_BINARY   ///< binary format (P4/P5/P6)
} PNM_FORMAT;

///
/// @fn     read_pnm
/// @brief  load image data from PNM file
/// @param  src [in]   path of PNM source image
/// @return pointer to image, NULL if failed
/// @note   not support 2 Byte value
///
img_t *read_pnm(const char *src);

///
/// @fn     write_pnm
/// @brief  save PNM file from image data
/// @param  img     [in]   pointer to image to be saved
/// @param  dst     [in]   output path of PNM image
/// @param  format  [in]   output format of PNM image
/// @retval true    succeed to save image
/// @retval false   fail to save image
/// @note   not support 2 Byte value
///
bool write_pnm(img_t *img, const char *dst, PNM_FORMAT format);

#endif // PNM_H
