///
/// @file   convert.c
/// @brief  image conversion function
/// @author kentakuramochi
///

#include "convert.h"

#include <stddef.h>

/// @fn     rgb_to_y_BT601
/// @brief  convert RGB to Y of YUV by ITU-R BT.601
/// @param  r   [in]   source R value of RGB
/// @param  g   [in]   source G value of RGB
/// @param  b   [in]   source B value of RGB
/// @return luma component Y
///
static inline uint8_t rgb_to_y_BT601(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
}

cimg_t *rgb_to_gray(cimg_t *src)
{
    if ((src == NULL) || (src->channels != CH_RGB)) {
        return NULL;
    }

    cimg_t *dst = cimg_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            uint8_t *pixel = &src->data[y * src->stride + x * src->channels];
            dst->data[y * dst->stride + x] = rgb_to_y_BT601(*pixel, *(pixel + 1), *(pixel + 2));
        }
    }

    return dst;
}

cimg_t *binarize(cimg_t *src, uint8_t threshold)
{
    if ((src == NULL) || (src->channels != CH_GRAY)) {
        return NULL;
    }

    cimg_t *dst = cimg_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    for (int i = 0; i < (src->height * src->width); i++) {
        dst->data[i] = (src->data[i] < threshold) ? 0 : UINT8_MAX;
    }

    return dst;
}

cimg_t *binarize_otsu(cimg_t *src)
{
    int n_pix = src->width * src->height;

    // count each pixel value, 0 to 255
    int histgram[256] = { 0 };
    for (int i = 0; i < (src->height * src->width); i++) {
        histgram[src->data[i]]++;
    }

    // determine threshold by discriminant analysis
    uint8_t threshold = 0;
    double  sb_max    = 0.0; // max inner-class variance

    for (int th = 0; th < UINT8_MAX; th++) {
        int    n0 = 0, n1 = 0;      // pixel count of class 0/1
        double w0, w1;              // pixel ratio of class 0/1
        double m0 = 0.0 , m1 = 0.0; // mean of class 0/1

        // categorize values to class 0/1 by temporal threshold
        for (int i = 0; i < th; i++) {
            n0 += histgram[i];
            m0 += i * histgram[i];
        }
        for (int i = th; i <= UINT8_MAX; i++) {
            n1 += histgram[i];
            m1 += i * histgram[i];
        }

        w0 = (double)n0 / n_pix;
        w1 = (double)n1 / n_pix;
        m0 /= n0;
        m1 /= n1;

        // inner-class variance
        double sb = w0 * w1 * (m0 - m1) * (m0 - m1);

        // save threshold maximize inner-class variance
        if (sb > sb_max) {
            threshold = th;
            sb_max    = sb;
        }
    }

    // binarize by the detemined threshold
    return binarize(src, threshold);
}

cimg_t *quantize(cimg_t *src, uint8_t level)
{
    cimg_t *dst = cimg_create(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    // get quantized value per level
    uint8_t q_unit = 256 / level;

    for (int i = 0; i < (src->channels * src->height * src->width); i++) {
        uint8_t thresh = 0;

        // quantize by comparing threshold: (quantized value)/2
        for (int j = 0; j < level; j++) {
            uint8_t qval =  (thresh + 0.5 * q_unit);

            if (src->data[i] >= thresh) {
                dst->data[i] = qval;
            }

            thresh += q_unit;
        }
    }

    return dst;
}

void get_hist(cimg_t *src, int histogram[256], int bin)
{
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    // bin range
    uint8_t level = 256 / bin;

    for (int i = 0; i < (src->width * src->height); i++) {
        uint8_t val = src->data[i];

        // count each bin and save it to start position of each bin
        int th = level;
        for (int j = 0; j <= bin; j++) {
            if (val < th) {
                histogram[(th - level)]++;
                break;
            }
            th += level;
        }
    }

    // copy the count to the same bin
    int bin_s = 0;
    for (int i = 1; i < 256; i++) {
        if (i % level == 0) {
            bin_s += level;
        }
        histogram[i] = histogram[bin_s];
    }
}
