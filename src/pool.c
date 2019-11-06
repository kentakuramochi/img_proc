#include "pool.h"

#include <stddef.h>

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

static img_t *pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h, KERNEL_TYPE type)
{
    img_t *dst = img_allocate(src->width, src->height, src->colorspace);
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

    for (int c = 0; c < dst->channel; c++) {
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

img_t *average_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    return pooling(src, kernel_w, kernel_h, KERNEL_AVG);
}

img_t *max_pooling(img_t *src, uint32_t kernel_w, uint32_t kernel_h)
{
    return pooling(src, kernel_w, kernel_h, KERNEL_MAX);
}

static uint8_t filter(uint8_t **p_ch, int x, int y, int w, int h, double *kernel, int kw, int kh)
{
    int ofs_x = kw / 2;
    int ofs_y = kh / 2;

    double sum = 0;

    int ky = y - ofs_y;
    for (int i = 0; i < kh; i++) {
        int kx = x - ofs_x;
        for (int j = 0; j < kw; j++) {
            if ((kx >= 0) && (kx < w) && (ky >= 0) && (ky < h)) {
                sum += p_ch[ky][kx] * kernel[i * kw + j];
            }
            kx++;
        }
        ky++;
    }

    sum = fmax(sum, 0);
    sum = fmin(sum, UINT8_MAX);

    return (uint8_t)sum;
}
