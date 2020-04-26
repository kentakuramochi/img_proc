///
/// @file   convert.c
/// @brief  image conversion function
///

#include "convert.h"

#include <stddef.h>
#include <math.h>

#include "util.h"

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

img_t *rgb_to_gray(img_t *src)
{
    if ((src == NULL) || (src->channels != CH_RGB)) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
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

img_t *binarize(img_t *src, uint8_t threshold)
{
    if ((src == NULL) || (src->channels != CH_GRAY)) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    for (int i = 0; i < (src->height * src->width); i++) {
        dst->data[i] = (src->data[i] < threshold) ? 0 : UINT8_MAX;
    }

    return dst;
}

img_t *binarize_otsu(img_t *src)
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

img_t *quantize(img_t *src, uint8_t level)
{
    img_t *dst = img_create(src->width, src->height, src->channels);
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

void get_hist(img_t *src, int histogram[256], int bin)
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

img_t *expand_hist(img_t *src, uint8_t min, uint8_t max)
{
    const int size = src->height * src->width * src->channels;

    uint8_t hist_max = 0;
    uint8_t hist_min = UINT8_MAX;
    for (int i = 0; i < size; i++) {
        uint8_t pixel = src->data[i];
        hist_max = (pixel > hist_max) ? pixel : hist_max;
        hist_min = (pixel < hist_min) ? pixel : hist_min;
    }

    img_t *dst = img_create(src->width, src->height, src->channels);

    double scale = (double)(max - min) / (hist_max - hist_min);

    for (int i = 0; i < size; i++) {
        uint8_t pixel = src->data[i];
        uint8_t norm;
        if (pixel < hist_min){
            norm = min;
        } else if (pixel < hist_max) {
            norm = (uint8_t)(scale * (pixel - hist_min) + min);
        } else {
            norm = max;
        }
        dst->data[i] = norm;
    }

    return dst;
}

img_t *normarize_hist(img_t *src, uint8_t mean, double sigma)
{
    const int size = src->height * src->width * src->channels;

    int hist_mean = 0;
    for (int i = 0; i < size; i++) {
        hist_mean += src->data[i];
    }
    hist_mean /= size;

    double hist_sigma = 0;
    for (int i = 0; i < size; i++) {
        hist_sigma += (src->data[i] - hist_mean) * (src->data[i] - hist_mean);
    }
    hist_sigma /= size;

    hist_sigma = sqrt(hist_sigma);

    img_t *dst = img_create(src->width, src->height, src->channels);

    double scale = sigma / hist_sigma;

    for (int i = 0; i < size; i++) {
        dst->data[i] = (uint8_t)(scale * (src->data[i] - hist_mean) + mean);
    }

    return dst;
}

img_t *equalize_hist(img_t *src)
{
    const int size = src->height * src->width * src->channels;

    int histogram[256] = { 0 };
    uint8_t max = 0;
    for (int i = 0; i < size; i++) {
        uint8_t pixel = src->data[i];
        histogram[pixel]++;
        max = (pixel > max) ? pixel : max;
    }

    double scale = (double)max / size;

    img_t *dst = img_create(src->width, src->height, src->channels);

    for (int i = 0; i < size; i++) {
        int sum_freq = 0;
        for (int j = 0; j < src->data[i]; j++) {
            sum_freq += histogram[j];
        }
        dst->data[i] = (uint8_t)(scale * sum_freq);
    }

    return dst;
}

img_t *gamma_correction(img_t *src, double gamma)
{
    img_t *dst = img_create(src->width, src->height, src->channels);

    for (int i = 0; i < (src->height * src->width * src->channels); i++) {
        double norm = (double)src->data[i] / UINT8_MAX;
        dst->data[i] = (uint8_t)(pow(norm, (1 / gamma)) * UINT8_MAX);
    }

    return dst;
}

img_t *nearest_neighbor(img_t *src, double scale_x, double scale_y)
{
    img_t *dst = img_create((int)(scale_x * src->width), (int)(scale_y * src->height), src->channels);

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = 
                    src->data[(int)round(y / scale_y) * src->stride + (int)round(x / scale_x) * src->channels + c];
            }
        }
    }

    return dst;
}

img_t *bilinear(img_t *src, double scale_x, double scale_y)
{
    img_t *dst = img_create((int)(scale_x * src->width), (int)(scale_y * src->height), src->channels);

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = 
                    src->data[(int)round(y / scale_y) * src->stride + (int)round(x / scale_x) * src->channels + c];
            }
        }
    }

    return dst;
}

img_t *biqubic(img_t *src, double scale_x, double scale_y)
{
    img_t *dst = img_create((int)(scale_x * src->width), (int)(scale_y * src->height), src->channels);

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = 
                    src->data[(int)round(y / scale_y) * src->stride + (int)round(x / scale_x) * src->channels + c];
            }
        }
    }

    return dst;
}

img_t *affine(img_t *src, double sx, double sy, double deg, int tx, int ty)
{
    img_t *dst = img_create((int)(sx * src->width), (int)(sy * src->height), src->channels);

    double a, b, c, d;
    // matrix
    // [ a, b, tx ]
    // [ c, d, ty ]
    // [ 0, 0, 1  ]

    if (deg != 0) {
        a = cos(deg * M_PI / 180);
        b = sin(deg * M_PI / 180);
        c = -sin(deg * M_PI / 180);
        d = cos(deg * M_PI / 180);

        tx = -src->width / 2;
        ty = -src->height / 2;
    } else {
        a =  sx;
        b =  0;
        c =  0;
        d =  sy;
    }

    double det = 1 / (a * d - b * c);

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            int src_x = (int)((d * x - b * y) * det - tx);
            int src_y = (int)((-c * x + a * y) * det - ty);

            if ((src_x < 0) || (src_x >= src->width) || (src_y < 0) || (src_y >= src->height)) {
                for (int c = 0; c < dst->channels; c++) {
                    dst->data[y * dst->stride + x * dst->channels + c] = 0;
                }
            } else {
                for (int c = 0; c < dst->channels; c++) {
                    dst->data[y * dst->stride + x * dst->channels + c] = 
                        src->data[src_y * src->stride + src_x * src->channels + c];
                }
            }
        }
    }

    return dst;
}
