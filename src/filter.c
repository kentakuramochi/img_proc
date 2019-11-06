#include "filter.h"

#include <stddef.h>
#include <math.h>
#include <float.h>

#define M_PI 3.14159265358979

static img_t *filtering(img_t *src, double *kernel, uint32_t kernel_w, uint32_t kernel_h)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);
    if (dst == NULL) {
        return NULL;
    }

    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y ++) {
            for (int x = 0; x < dst->width; x ++) {
                dst->ch[c][y][x] = filter(src->ch[c], x, y, src->width, src->height,
                                          kernel, kernel_w, kernel_h);
            }
        }
    }

    return dst;
}

img_t *gaussian_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h, double stddev)
{
    // create gaussian kernel
    double kernel[kernel_w * kernel_h];

    double c   = 1 / (sqrt(2 * M_PI) * stddev);
    double sum = 0;

    int ofs_x = kernel_w / 2;
    int ofs_y = kernel_h / 2;

    int ky = ofs_y;
    for (int y = 0; y < kernel_h; y++) {
        int kx = ofs_x;
        for (int x = 0; x < kernel_w; x++) {
            kernel[y * kernel_w + x] = c * exp(-(kx * kx  + ky * ky) / (2 * stddev * stddev));
            sum += kernel[y * kernel_w + x];
            kx++;
        }
        ky++;
    }

    c = 1 / sum;
    for (int i = 0; i < (kernel_w * kernel_h); i++) {
        kernel[i] *= c;
    }

    return filtering(src, kernel, kernel_w, kernel_h);
}

// compare function for ascending order
static int cmp_ascend(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

img_t *median_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);
    if (dst == NULL) {
        return NULL;
    }

    const int ksize   = kernel_w * kernel_h;
    const int mid_idx = (ksize - 1) / 2;

    int ofs_x = -(kernel_w / 2);
    int ofs_y = -(kernel_h / 2);

    uint8_t kernel[ksize];
    int ky, kx;
    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y ++) {
            for (int x = 0; x < dst->width; x ++) {

                ky = y + ofs_y;
                for (int i = 0; i < kernel_h; i++) {
                    kx = x + ofs_x;
                    for (int j = 0; j < kernel_h; j++) {
                        if ((ky >= 0) && (ky < src->height) &&
                            (kx >= 0) && (kx < src->width)) {
                            kernel[i * kernel_w + j] = src->ch[c][ky][kx];
                        } else {
                            kernel[i * kernel_w + j] = 0;
                        }
                        kx++;
                    }
                    ky++;
                }

                qsort(kernel, ksize, 1, cmp_ascend);

                dst->ch[c][y][x] = (ksize % 2 == 1) ? kernel[mid_idx] :
                    (kernel[mid_idx] + kernel[mid_idx + 1]) / 2;
            }
        }
    }

    return dst;
}

img_t *average_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);
    if (dst == NULL) {
        return NULL;
    }

    // create kernel
    const int ksize = kernel_w * kernel_h;
    double kernel[ksize];

    for (int y = 0; y < kernel_h; y++) {
        for (int x = 0; x < kernel_w; x++) {
            kernel[y * kernel_w + x] = 1.0 / ksize;
        }
    }

    return filtering(src, kernel, kernel_w, kernel_h);
}

img_t *motion_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);
    if (dst == NULL) {
        return NULL;
    }

    // create kernel
    double kernel[kernel_w * kernel_h];

    for (int i = 0; i < kernel_h; i++) {
        kernel[i * kernel_w + i] = 1.0 / kernel_w;
    }

    return filtering(src, kernel, kernel_w, kernel_h);
}

img_t *maxmin_filter(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    int ofs_x = -(kernel_w / 2);
    int ofs_y = -(kernel_h / 2);

    int ky, kx;
    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y ++) {
            for (int x = 0; x < dst->width; x ++) {

                uint8_t max = 0;
                uint8_t min = UINT8_MAX;
                ky = y + ofs_y;
                for (int i = 0; i < kernel_h; i++) {
                    kx = x + ofs_x;
                    for (int j = 0; j < kernel_h; j++) {
                        if ((ky >= 0) && (ky < src->height) &&
                            (kx >= 0) && (kx < src->width)) {
                            if (max < src->ch[c][ky][kx]) {
                                max = src->ch[c][ky][kx];
                            }
                            if (min > src->ch[c][ky][kx]) {
                                min = src->ch[c][ky][kx];
                            }
                        }
                        kx++;
                    }
                    ky++;
                }

                dst->ch[c][y][x] = max - min;
            }
        }
    }

    return dst;
}

img_t *diff_filter(img_t *src, bool is_horizontal)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double kernel[3 * 3] = { 0 };
    // using central difference form
    if (is_horizontal) {
        // horizontal
        //    0,   0,    0
        // -1/2,   0,  1/2
        //    0,   0,    0
        kernel[3] = -0.5;
        kernel[4] =  0.5;
    } else {
        // vertical
        //  0, -1/2, 0
        //  0,  0,   0
        //  0,  1/2, 0
        kernel[1] = -0.5;
        kernel[4] =  0.5;
    }

    return filtering(src, kernel, 3, 3);
}

img_t *sobel_filter(img_t *src, bool is_horizontal)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double kernel[3 * 3] = { 0 };
    if (is_horizontal) {
        // horizontal
        //  1   0  -1
        //  2   0  -2
        //  1   0  -1
        kernel[0] = 1;
        kernel[3] = 2;
        kernel[6] = 1;
        kernel[2] = -1;
        kernel[5] = -2;
        kernel[8] = -1;
    } else {
        // vertical
        //  1   2   1
        //  0   0   0
        // -1  -2  -1
        kernel[0] = 1;
        kernel[1] = 2;
        kernel[2] = 1;
        kernel[6] = -1;
        kernel[7] = -2;
        kernel[8] = -1;
    }

    return filtering(src, kernel, 3, 3);
}

img_t *prewitt_filter(img_t *src, bool is_horizontal)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double kernel[3 * 3] = { 0 };
    if (is_horizontal) {
        // horizontal
        // -1   0   1
        // -1   0   1
        // -1   0   1
        kernel[0] = -1;
        kernel[3] = -1;
        kernel[6] = -1;
        kernel[2] =  1;
        kernel[5] =  1;
        kernel[8] =  1;
    } else {
        // vertical
        // -1  -1  -1
        //  0   0   0
        //  1   1   1
        kernel[0] = -1;
        kernel[1] = -1;
        kernel[2] = -1;
        kernel[6] =  1;
        kernel[7] =  1;
        kernel[8] =  1;
    }

    return filtering(src, kernel, 3, 3);
}

img_t *laplacian_filter(img_t *src)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double kernel[3 * 3] = {
         0,  1,  0,
         1, -4,  1,
         0,  1,  0
    };

    return filtering(src, kernel, 3, 3);
}

img_t *emboss_filter(img_t *src)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    double kernel[3 * 3] = {
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2
    };

    return filtering(src, kernel, 3, 3);
}

img_t *log_filter(img_t *src, int kernel_w, int kernel_h, double sigma)
{
    if (src->colorspace != COLORSPACE_GRAY) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if (dst == NULL) {
        return NULL;
    }

    // create kernel
    double kernel[kernel_w * kernel_h];

    double sigma_p2 = sigma * sigma;

    double c = 1 / (2 * M_PI * pow(sigma, 6));

    int ofs_x = kernel_w / 2;
    int ofs_y = kernel_h / 2;

    double sum = 0;

    int ky = -ofs_y;
    for (int y = 0; y < kernel_h; y++) {
        int kx = -ofs_x;
        for (int x = 0; x < kernel_w; x++) {
            kernel[y * kernel_w + x] = c * (kx * kx + ky * ky - 2 * sigma_p2)  * exp(-(kx * kx  + ky * ky) / (2 * sigma_p2));
            sum += kernel[y * kernel_w + x];
            kx++;
        }
        ky++;
    }

    for (int i = 0; i < (kernel_w * kernel_h); i++) {
        kernel[i] /= sum;
    }

    return filtering(src, kernel, kernel_w, kernel_h);
}
