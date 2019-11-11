#include "filter.h"

#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <float.h>

#define M_PI 3.14159265358979

typedef uint8_t (*filter_kernel)(cimg_t*, int, int, int, double*, int, int);

static uint8_t kernel_sum(cimg_t *img, int x, int y, int c, double *filter, int kw, int kh)
{
    int ofs_x = kw / 2;
    int ofs_y = kh / 2;

    double sum = 0;

    int ky = y - ofs_y;
    for (int i = 0; i < kh; i++) {
        int kx = x - ofs_x;
        for (int j = 0; j < kw; j++) {
            if ((kx >= 0) && (kx < img->width) && (ky >= 0) && (ky < img->height)) {
                sum += img->data[ky * img->stride + kx * img->channels + c] * filter[i * kw + j];
            }
            kx++;
        }
        ky++;
    }

    sum = fmax(sum, 0);
    sum = fmin(sum, UINT8_MAX);

    return (uint8_t)sum;
}

// compare function for ascending order
static int cmp_ascend(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

static uint8_t kernel_median(cimg_t *img, int x, int y, int c, uint8_t *filter, int kw, int kh)
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
                filter[i * kw + j] = img->data[ky * img->stride + kx * img->channels + c];
            } else {
                filter[i * kw + j] = 0;
            }
            kx++;
        }
        ky++;
    }

    qsort(filter, ksize, 1, cmp_ascend);

    return (ksize % 2 == 1) ? filter[mid] : (filter[mid] + filter[mid + 1]) / 2;
}

static uint8_t kernel_maxmin(cimg_t *img, int x, int y, int c, double *filter, int kw, int kh)
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

static cimg_t *filtering(cimg_t *src, double *filter, int filter_w, int filter_h, filter_kernel kernel)
{
    cimg_t *dst = cimg_allocate(src->width, src->height, src->channels);
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

cimg_t *gaussian_filter(cimg_t *src, int filter_w, int filter_h, double sigma)
{
    // create gaussian filter
    double filter[filter_w * filter_h];

    double c   = 1 / (sqrt(2 * M_PI) * sigma);
    double sum = 0;

    int ofs_x = filter_w / 2;
    int ofs_y = filter_h / 2;

    int ky = ofs_y;
    for (int y = 0; y < filter_h; y++) {
        int kx = ofs_x;
        for (int x = 0; x < filter_w; x++) {
            filter[y * filter_w + x] = c * exp(-(kx * kx  + ky * ky) / (2 * sigma * sigma));
            sum += filter[y * filter_w + x];
            kx++;
        }
        ky++;
    }

    c = 1 / sum;
    for (int i = 0; i < (filter_w * filter_h); i++) {
        filter[i] *= c;
    }

    return filtering(src, filter, filter_w, filter_h, kernel_sum);
}

cimg_t *median_filter(cimg_t *src, int filter_w, int filter_h)
{
    cimg_t *dst = cimg_allocate(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

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

cimg_t *average_filter(cimg_t *src, int filter_w, int filter_h)
{
    cimg_t *dst = cimg_allocate(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    // create filter
    const int ksize = filter_w * filter_h;
    double filter[ksize];

    for (int y = 0; y < filter_h; y++) {
        for (int x = 0; x < filter_w; x++) {
            filter[y * filter_w + x] = 1.0 / ksize;
        }
    }

    return filtering(src, filter, filter_w, filter_h, kernel_sum);
}

cimg_t *motion_filter(cimg_t *src, int filter_w, int filter_h)
{
    cimg_t *dst = cimg_allocate(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    // create filter
    double filter[filter_w * filter_h];

    for (int i = 0; i < filter_h; i++) {
        filter[i * filter_w + i] = 1.0 / filter_w;
    }

    return filtering(src, filter, filter_w, filter_h, kernel_sum);
}

cimg_t *maxmin_filter(cimg_t *src, int filter_w, int filter_h)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    for (int y = 0; y < dst->height; y++) {
        for (int x = 0; x < dst->width; x++) {
            for (int c = 0; c < dst->channels; c++) {
                dst->data[y * dst->stride + x * dst->channels + c] = kernel_maxmin(src, x, y, c, NULL, filter_w, filter_h);
            }
        }
    }

    return dst;
}

cimg_t *diff_filter(cimg_t *src, bool is_horizontal)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = { 0 };
    // using central difference form
    if (is_horizontal) {
        // horizontal
        //    0,   0,    0
        // -1/2,   0,  1/2
        //    0,   0,    0
        filter[3] = -0.5;
        filter[4] =  0.5;
    } else {
        // vertical
        //  0, -1/2, 0
        //  0,  0,   0
        //  0,  1/2, 0
        filter[1] = -0.5;
        filter[4] =  0.5;
    }

    return filtering(src, filter, 3, 3, kernel_sum);
}

cimg_t *sobel_filter(cimg_t *src, bool is_horizontal)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = { 0 };
    if (is_horizontal) {
        // horizontal
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
        // vertical
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

    return filtering(src, filter, 3, 3, kernel_sum);
}

cimg_t *prewitt_filter(cimg_t *src, bool is_horizontal)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = { 0 };
    if (is_horizontal) {
        // horizontal
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
        // vertical
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

    return filtering(src, filter, 3, 3, kernel_sum);
}

cimg_t *laplacian_filter(cimg_t *src)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = {
         0,  1,  0,
         1, -4,  1,
         0,  1,  0
    };

    return filtering(src, filter, 3, 3, kernel_sum);
}

cimg_t *emboss_filter(cimg_t *src)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double filter[3 * 3] = {
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2
    };

    return filtering(src, filter, 3, 3, kernel_sum);
}

cimg_t *log_filter(cimg_t *src, int filter_w, int filter_h, double sigma)
{
    if (src->channels != CH_GRAY) {
        return NULL;
    }

    cimg_t *dst = cimg_allocate(src->width, src->height, CH_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    // create filter
    double filter[filter_w * filter_h];

    double sigma_p2 = sigma * sigma;

    double c = 1 / (2 * M_PI * pow(sigma, 6));

    int ofs_x = filter_w / 2;
    int ofs_y = filter_h / 2;

    double sum = 0;

    int ky = -ofs_y;
    for (int y = 0; y < filter_h; y++) {
        int kx = -ofs_x;
        for (int x = 0; x < filter_w; x++) {
            filter[y * filter_w + x] = c * (kx * kx + ky * ky - 2 * sigma_p2)  * exp(-(kx * kx  + ky * ky) / (2 * sigma_p2));
            sum += filter[y * filter_w + x];
            kx++;
        }
        ky++;
    }

    for (int i = 0; i < (filter_w * filter_h); i++) {
        filter[i] /= sum;
    }

    return filtering(src, filter, filter_w, filter_h, kernel_sum);
}
