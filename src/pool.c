#include "pool.h"

#include <stddef.h>
#include <math.h>

typedef uint8_t (*pool_kernel)(cimg_t*, int, int, int, int, int);

static uint8_t kernel_avg(cimg_t *img, int x, int y, int c, int kw, int kh)
{
    int avg = 0;
    for (int i = 0; i < kh; i++) {
        for (int j = 0; j < kw; j++) {
            avg += img->data[(y + kh) * img->stride + (x + kw) * img->channels + c];
        }
    }
    return (uint8_t)(avg /= kw * kh);
}

static uint8_t kernel_max(cimg_t *img, int x, int y, int c, int kw, int kh)
{
    int max = 0;
    for (int i = 0; i < kh; i++) {
        for (int j = 0; j < kw; j++) {
            uint8_t pixel = img->data[(y + kh) * img->stride + (x + kw) * img->channels + c];
            max = (pixel > max) ? pixel : max;
        }
    }
    return max;
}

static cimg_t *pooling(cimg_t *src, int kernel_w, int kernel_h, pool_kernel kernel)
{
    cimg_t *dst = cimg_create(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    for (int y = 0; y < dst->height; y += kernel_h) {
        for (int x = 0; x < dst->width; x += kernel_w) {
            for (int c = 0; c < dst->channels; c++) {
                uint8_t pooled = kernel(src, x, y, c, kernel_w, kernel_h);
                for (int i = 0; i < kernel_h; i++) {
                    for (int j = 0; j < kernel_w; j++) {
                        dst->data[(y + i) *dst->stride + (x + j) * dst->channels + c] = pooled;
                    }
                }
            }
        }
    }

    return dst;
}

cimg_t *average_pooling(cimg_t *src, int kernel_w, int kernel_h)
{
    if ((kernel_w <= 0) || (kernel_h <= 0)) {
        return NULL;
    }

    return pooling(src, kernel_w, kernel_h, kernel_avg);
}

cimg_t *max_pooling(cimg_t *src, int kernel_w, int kernel_h)
{
    if ((kernel_w <= 0) || (kernel_h <= 0)) {
        return NULL;
    }

    return pooling(src, kernel_w, kernel_h, kernel_max);
}
