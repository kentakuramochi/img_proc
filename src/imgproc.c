#include "imgproc.h"

#include <stdlib.h>
#include <stddef.h>
#include <float.h>
#include <math.h>

const double M_PI = 3.14159265358979;

// conversion formula by ITU-R BT.601
#define BT601(r, g, b) (0.299 * (r) + 0.587 * (g) + 0.114 * (b))

img_t *rgb_to_gray(img_t *src)
{
    if (src->colorspace != COLORSPACE_RGB) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

    if (dst == NULL) {
        return NULL;
    }

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            dst->row[y][x] = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);
        }
    }

    return dst;
}

img_t *binarize(img_t *src, uint8_t threshold)
{
    if (src->colorspace != COLORSPACE_RGB) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

    if (dst == NULL) {
        return NULL;
    }

    uint8_t gray;

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            gray = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);

            dst->row[y][x] = (gray < threshold) ? 0 : 255;
        }
    }

    return dst;
}

img_t *binarize_otsu(img_t *src)
{
    if (src->colorspace != COLORSPACE_RGB) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

    if (dst == NULL) {
        return NULL;
    }

    int n_pix = src->width * src->height;

    int histgram[256] = { 0 };

    uint8_t gray;

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            gray = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);

            dst->row[y][x] = gray;

            histgram[gray]++;
        }
    }

    uint8_t threshold = 0;
    double  sb_max    = 0.0;

    for (int th = 0; th < 255; th++) {
        int    n0 = 0, n1 = 0;
        double w0, w1;
        double m0 = 0.0 , m1 = 0.0;

        for (int i = 0; i < th; i++) {
            n0 += histgram[i];
            m0 += i * histgram[i];
        }

        for (int i = th; i <= 255; i++) {
            n1 += histgram[i];
            m1 += i * histgram[i];
        }

        w0 = (double)n0 / n_pix;
        w1 = (double)n1 / n_pix;
        m0 /= n0;
        m1 /= n1;

        double sb = w0 * w1 * (m0 - m1) * (m0 - m1);

        if (sb > sb_max) {
            threshold = th;
            sb_max    = sb;
        }
    }

    for (int i = 0; i < n_pix; i++) {
        dst->data[i] = (dst->data[i] < threshold) ? 0 : 255;
    }

    return dst;
}

img_t *quantize(img_t *src, uint8_t level)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);

    if (dst == NULL) {
        return NULL;
    }

    uint8_t q_unit = 256 / level;

    for (int i = 0; i < (src->channel * src->height * src->width); i++) {
        uint8_t thresh = 0;

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

img_t *average_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);

    if (dst == NULL) {
        return NULL;
    }

    float k_1 = 1.0 / (kernel_w * kernel_h);

    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y += kernel_h) {
            for (int x = 0; x < dst->width; x += kernel_w) {

                int avg = 0;

                for (int i = 0; i < kernel_h; i++) {
                    for (int j = 0; j < kernel_w; j++) {
                        avg += src->ch[c][y + i][x + j];
                    }
                }

                avg *= k_1;

                for (int i = 0; i < kernel_h; i++) {
                    for (int j = 0; j < kernel_w; j++) {
                        dst->ch[c][y + i][x + j] = avg;
                    }
                }
            }
        }
    }

    return dst;
}

img_t *max_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);

    if (dst == NULL) {
        return NULL;
    }

    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y += kernel_h) {
            for (int x = 0; x < dst->width; x += kernel_w) {

                int max = 0;

                for (int i = 0; i < kernel_h; i++) {
                    for (int j = 0; j < kernel_w; j++) {
                        if (max < src->ch[c][y + i][x + j]) {
                            max = src->ch[c][y + i][x + j];
                        }
                    }
                }

                for (int i = 0; i < kernel_h; i++) {
                    for (int j = 0; j < kernel_w; j++) {
                        dst->ch[c][y + i][x + j] = max;
                    }
                }
            }
        }
    }

    return dst;
}

img_t *gaussian_filter(img_t *src, uint32_t kernel_size, double stddev)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);

    if (dst == NULL) {
        return NULL;
    }

    double kernel[kernel_size * kernel_size];

    double coef = 1 / (sqrt(2 * M_PI) * stddev);
    double sum  = 0;

    int ofs_y = -(int)kernel_size / 2;
    int ofs_x = -(int)kernel_size / 2;

    int ky, kx;

    ky = ofs_y;
    for (int y = 0; y < kernel_size; y++) {
        kx = ofs_x;
        for (int x = 0; x < kernel_size; x++) {
            kernel[y * kernel_size + x] = coef * exp(-(kx * kx  + ky * ky) / (2 * stddev * stddev));
            sum += kernel[y * kernel_size + x];
            kx++;
        }
        ky++;
    }

    for (int y = 0; y < kernel_size; y++) {
        for (int x = 0; x < kernel_size; x++) {
            kernel[y * kernel_size + x] /= sum;
        }
    }

    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y ++) {
            for (int x = 0; x < dst->width; x ++) {
                double filtered = 0;
                ky = y + ofs_y;

                for (int i = 0; i < kernel_size; i++) {
                    kx = x + ofs_x;
                    for (int j = 0; j < kernel_size; j++) {
                        if ((ky >= 0) && (ky < src->height) &&
                            (kx >= 0) && (kx < src->width)) {
                            filtered += src->ch[c][ky][kx] * kernel[i * kernel_size + j];
                        }
                        kx++;
                    }
                    ky++;
                }

                dst->ch[c][y][x] = (uint8_t)filtered;
            }
        }
    }

    return dst;
}

static int cmp_ascend(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

img_t *median_filter(img_t *src, uint32_t kernel_size)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);

    if (dst == NULL) {
        return NULL;
    }

    const int ksize = kernel_size * kernel_size;

    const int mid_idx = (ksize - 1) / 2;

    uint8_t kernel[ksize];

    int ofs_y = -(int)kernel_size / 2;
    int ofs_x = -(int)kernel_size / 2;

    int ky, kx;

    for (int c = 0; c < dst->channel; c++) {
        for (int y = 0; y < dst->height; y ++) {
            for (int x = 0; x < dst->width; x ++) {
                uint8_t median = 0;
                ky = y + ofs_y;

                for (int i = 0; i < kernel_size; i++) {
                    kx = x + ofs_x;
                    for (int j = 0; j < kernel_size; j++) {
                        if ((ky >= 0) && (ky < src->height) &&
                            (kx >= 0) && (kx < src->width)) {
                            kernel[i * kernel_size + j] = src->ch[c][ky][kx];
                        } else {
                            kernel[i * kernel_size + j] = 0;
                        }
                        kx++;
                    }
                    ky++;
                }

                qsort(kernel, ksize, 1, cmp_ascend);

                if (kernel_size % 2 == 1) {
                    median = kernel[mid_idx];
                } else {
                    median = (kernel[mid_idx] + kernel[mid_idx + 1]) / 2;
                }

                dst->ch[c][y][x] = median;
            }
        }
    }

    return dst;
}
