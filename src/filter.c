///
/// @file   filter.c
/// @brief  image filtering functions
///

#include "filter.h"

#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <float.h>

#include "util.h"

///
/// @typedef    filter_kernel
/// @brief      kernel functions
///
typedef uint8_t (*filter_kernel)(img_t*, int, int, int, double*, int, int);

///
/// @fn     kernel_conv
/// @brief  get convolutional value in kernel
/// @param  src     [in]    pointer to source image
/// @param  x       [in]    x coordinate
/// @param  y       [in]    y coordinate
/// @param  c       [in]    channel
/// @param  kernel  [in]    coefficient kernel
/// @param  kw      [in]    kernel width
/// @param  kh      [in]    kernel height
/// @return convolutional value in kernel
///
static uint8_t kernel_conv(img_t *img, int x, int y, int c, double *kernel, int kw, int kh)
{
    int ofs_x = kw / 2;
    int ofs_y = kh / 2;

    double sum = 0;

    int ky = y - ofs_y;
    for (int i = 0; i < kh; i++) {
        int kx = x - ofs_x;
        for (int j = 0; j < kw; j++) {
            if ((kx >= 0) && (kx < img->width) && (ky >= 0) && (ky < img->height)) {
                sum += img->data[ky * img->stride + kx * img->channels + c] * kernel[i * kw + j];
            }
            kx++;
        }
        ky++;
    }

    sum = fmax(sum, 0);
    sum = fmin(sum, UINT8_MAX);

    return (uint8_t)sum;
}

///
/// @fn     cmp_ascend
/// @brief  compare function for qsort() with ascending order
/// @param  a   [in]    value0 to be sorted
/// @param  b   [in]    value1 to be sorted
/// @return positive if a > b, negative if a < b, 0 otherwise
///
static int cmp_ascend(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

///
/// @fn     kernel_median
/// @brief  get median in kernel
/// @param  src     [in]    pointer to source image
/// @param  x       [in]    x coordinate
/// @param  y       [in]    y coordinate
/// @param  c       [in]    channel
/// @param  kernel  [in]    kernel buffer to store values
/// @param  kw      [in]    kernel width
/// @param  kh      [in]    kernel height
/// @return median in kernel
///
static uint8_t kernel_median(img_t *img, int x, int y, int c, uint8_t *kernel, int kw, int kh)
{
    int ofs_x = kw / 2;
    int ofs_y = kh / 2;

    const int ksize = kw * kh;
    const int mid   = (ksize - 1) / 2;

    int ky = y - ofs_y;
    for (int i = 0; i < kh; i++) {
        int kx = x - ofs_x;
        for (int j = 0; j < kw; j++) {
            if ((kx >= 0) && (kx < img->width) && (ky >= 0) && (ky < img->height)) {
                kernel[i * kw + j] = img->data[ky * img->stride + kx * img->channels + c];
            } else {
                kernel[i * kw + j] = 0;
            }
            kx++;
        }
        ky++;
    }

    // sort with ascending order
    qsort(kernel, ksize, 1, cmp_ascend);

    return (ksize % 2 == 1) ? kernel[mid] : (kernel[mid] + kernel[mid + 1]) / 2;
}

///
/// @fn     kernel_maxmin
/// @brief  get difference between max value and min value in kernel
/// @param  src     [in]    pointer to source image
/// @param  x       [in]    x coordinate
/// @param  y       [in]    y coordinate
/// @param  c       [in]    channel
/// @param  kw      [in]    kernel width
/// @param  kh      [in]    kernel height
/// @return median in kernel
///
static uint8_t kernel_maxmin(img_t *img, int x, int y, int c, int kw, int kh)
{
    int ofs_x = kw / 2;
    int ofs_y = kh / 2;

    uint8_t max = 0;
    uint8_t min = UINT8_MAX;

    int ky = y - ofs_y;
    for (int i = 0; i < kh; i++) {
        int kx = x - ofs_x;
        for (int j = 0; j < kw; j++) {
            if ((kx >= 0) && (kx < img->width) && (ky >= 0) && (ky < img->height)) {
                uint8_t val = img->data[ky * img->stride + kx * img->channels + c];
                max = (val > max) ? val : max;
                min = (val < min) ? val : min;
            }
            kx++;
        }
        ky++;
    }

    return (max - min);
}

///
/// @fn     filtering
/// @brief  adopt filter with specified kernel
/// @param  src         [in]    pointer to source image
/// @param  filter      [in]    coefficient kernel
/// @param  filter_w    [in]    kernel width
/// @param  filter_h    [in]    kernel height
/// @param  kernel      [in]    kernel function
/// @return pointer to processed image, NULL if failed
///
static img_t *filtering(img_t *src, double *filter, int filter_w, int filter_h, filter_kernel kernel)
{
    img_t *dst = img_create(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = kernel(src, x, y, c, filter, filter_w, filter_h);
            }
        }
    }

    return dst;
}

img_t *gaussian_filter(img_t *src, int filter_w, int filter_h, double sigma)
{
    // create gaussian kernel
    double filter[filter_w * filter_h];

    double sum = 0;

    int ofs_x = filter_w / 2;
    int ofs_y = filter_h / 2;

    int ky = ofs_y;
    for (int y = 0; y < filter_h; y++) {
        int kx = ofs_x;
        for (int x = 0; x < filter_w; x++) {
            filter[y * filter_w + x] = (1 / (sqrt(2 * M_PI) * sigma)) * exp(-(kx * kx  + ky * ky) / (2 * sigma * sigma));
            sum += filter[y * filter_w + x];
            kx++;
        }
        ky++;
    }

    for (int i = 0; i < (filter_w * filter_h); i++) {
        filter[i] /= sum;
    }

    return filtering(src, filter, filter_w, filter_h, kernel_conv);
}

img_t *median_filter(img_t *src, int filter_w, int filter_h)
{
    img_t *dst = img_create(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    // only used for store values
    uint8_t filter[filter_w * filter_h];

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = kernel_median(src, x, y, c, filter, filter_w, filter_h);
            }
        }
    }

    return dst;
}

img_t *average_filter(img_t *src, int filter_w, int filter_h)
{
    img_t *dst = img_create(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    // create kernel
    const int ksize = filter_w * filter_h;
    double filter[ksize];
    for (int y = 0; y < filter_h; y++) {
        for (int x = 0; x < filter_w; x++) {
            filter[y * filter_w + x] = 1.0 / ksize;
        }
    }

    return filtering(src, filter, filter_w, filter_h, kernel_conv);
}

img_t *motion_filter(img_t *src, int filter_w, int filter_h)
{
    img_t *dst = img_create(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    // create kernel
    // e.g.) 3x3
    // 1/3   0    0
    //  0   1/3   0
    //  0    0   1/3
    double filter[filter_w * filter_h];
    for (int i = 0; i < filter_h; i++) {
        filter[i * filter_w + i] = 1.0 / filter_w;
    }

    return filtering(src, filter, filter_w, filter_h, kernel_conv);
}

img_t *maxmin_filter(img_t *src, int filter_w, int filter_h)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = kernel_maxmin(src, x, y, c, filter_w, filter_h);
            }
        }
    }

    return dst;
}

img_t *diff_filter(img_t *src, bool is_horizontal)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = { 0 };
    // using central difference form
    if (is_horizontal) {
        //    0,   0,    0
        // -1/2,   0,  1/2
        //    0,   0,    0
        filter[3] = -0.5;
        filter[4] =  0.5;
    } else {
        //  0, -1/2, 0
        //  0,  0,   0
        //  0,  1/2, 0
        filter[1] = -0.5;
        filter[4] =  0.5;
    }

    return filtering(src, filter, 3, 3, kernel_conv);
}

img_t *sobel_filter(img_t *src, bool is_horizontal)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = { 0 };
    if (is_horizontal) {
        //  1   0  -1
        //  2   0  -2
        //  1   0  -1
        filter[0] = 1;
        filter[3] = 2;
        filter[6] = 1;
        filter[2] = -1;
        filter[5] = -2;
        filter[8] = -1;
    } else {
        //  1   2   1
        //  0   0   0
        // -1  -2  -1
        filter[0] = 1;
        filter[1] = 2;
        filter[2] = 1;
        filter[6] = -1;
        filter[7] = -2;
        filter[8] = -1;
    }

    return filtering(src, filter, 3, 3, kernel_conv);
}

img_t *prewitt_filter(img_t *src, bool is_horizontal)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = { 0 };
    if (is_horizontal) {
        // -1   0   1
        // -1   0   1
        // -1   0   1
        filter[0] = -1;
        filter[3] = -1;
        filter[6] = -1;
        filter[2] =  1;
        filter[5] =  1;
        filter[8] =  1;
    } else {
        // -1  -1  -1
        //  0   0   0
        //  1   1   1
        filter[0] = -1;
        filter[1] = -1;
        filter[2] = -1;
        filter[6] =  1;
        filter[7] =  1;
        filter[8] =  1;
    }

    return filtering(src, filter, 3, 3, kernel_conv);
}

img_t *laplacian_filter(img_t *src)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = {
         0,  1,  0,
         1, -4,  1,
         0,  1,  0
    };

    return filtering(src, filter, 3, 3, kernel_conv);
}

img_t *emboss_filter(img_t *src)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = {
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2
    };

    return filtering(src, filter, 3, 3, kernel_conv);
}

img_t *log_filter(img_t *src, int filter_w, int filter_h, double sigma)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    img_t *dst = img_create(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    // craete gaussian kernel
    double filter[filter_w * filter_h];

    int ofs_x = filter_w / 2;
    int ofs_y = filter_h / 2;

    double sum = 0;

    int ky = -ofs_y;
    for (int y = 0; y < filter_h; y++) {
        int kx = -ofs_x;
        for (int x = 0; x < filter_w; x++) {
            filter[y * filter_w + x] = (1 / (2 * M_PI * pow(sigma, 6))) *
                                       (kx * kx + ky * ky - 2 * sigma * sigma)  *
                                       exp(-(kx * kx  + ky * ky) / (2 * sigma * sigma));
            sum += filter[y * filter_w + x];
            kx++;
        }
        ky++;
    }

    for (int i = 0; i < (filter_w * filter_h); i++) {
        filter[i] /= sum;
    }

    return filtering(src, filter, filter_w, filter_h, kernel_conv);
}
