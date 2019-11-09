#include "pool.h"

#include <stddef.h>
#include <math.h>

typedef enum {
    KERNEL_MAX,
    KERNEL_AVG
} KERNEL_TYPE;

static uint8_t pool_kernel_max(uint8_t **p_ch, int x, int y, int kw, int kh)
{
    int max = 0;
    for (int i = 0; i < kh; i++) {
        for (int j = 0; j < kw; j++) {
            max = (p_ch[y + i][x + j] > max) ? p_ch[y + i][x + j] : max;
        }
    }
    return max;
}

static uint8_t pool_kernel_avg(uint8_t **p_ch, int x, int y, int kw, int kh)
{
    int avg = 0;
    for (int i = 0; i < kh; i++) {
        for (int j = 0; j < kw; j++) {
            avg += p_ch[y + i][x + j];
        }
    }
    return (uint8_t)(avg /= kw * kh);
}

static img_t *pooling(img_t *src, int kernel_w, int kernel_h, KERNEL_TYPE type)
{
    img_t *dst = img_allocate(src->width, src->height, src->channels);
    if (dst == NULL) {
        return NULL;
    }

    uint8_t (*kernel)(uint8_t **, int, int, int, int);

    switch (type) {
        case KERNEL_MAX: kernel = pool_kernel_max; break;
        case KERNEL_AVG: kernel = pool_kernel_avg; break;
        default:
            img_free(dst);
            return NULL;
            break;
    }

    for (int c = 0; c < dst->channels; c++) {
        for (int y = 0; y < dst->height; y += kernel_h) {
            for (int x = 0; x < dst->width; x += kernel_w) {
                uint8_t pooled = kernel(src->ch[c], x, y, kernel_w, kernel_h);

                for (int i = 0; i < kernel_h; i++) {
                    for (int j = 0; j < kernel_w; j++) {
                        dst->ch[c][y + i][x + j] = pooled;
                    }
                }
            }
        }
    }

    return dst;
}

img_t *average_pooling(img_t *src, int kernel_w, int kernel_h)
{
    if ((kernel_w <= 0) || (kernel_h <= 0)) {
        return NULL;
    }

    return pooling(src, kernel_w, kernel_h, KERNEL_AVG);
}

img_t *max_pooling(img_t *src, int kernel_w, int kernel_h)
{
    if ((kernel_w <= 0) || (kernel_h <= 0)) {
        return NULL;
    }

    return pooling(src, kernel_w, kernel_h, KERNEL_MAX);
}
