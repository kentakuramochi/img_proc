#include "imgproc.h"

#include <stdlib.h>
#include <stddef.h>
#include <float.h>
#include <math.h>

#define M_PI 3.14159265358979

typedef enum {
    KERNEL_MAX,
    KERNEL_AVG
} KERNEL_TYPE;

// conversion formula by ITU-R BT.601
#define BT601(r, g, b) (0.299 * (r) + 0.587 * (g) + 0.114 * (b))

img_t *rgb_to_gray(img_t *src)
{
    if ((src == NULL) && (src->colorspace != COLORSPACE_RGB)) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if ((dst == NULL) && (dst->colorspace != COLORSPACE_GRAY)) {
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
    if ((src == NULL) && (src->colorspace != COLORSPACE_RGB)) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if ((dst == NULL) && (dst->colorspace != COLORSPACE_GRAY)) {
        return NULL;
    }

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            uint8_t gray = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);

            dst->row[y][x] = (gray < threshold) ? 0 : UINT8_MAX;
        }
    }

    return dst;
}

img_t *binarize_otsu(img_t *src)
{
    if ((src == NULL) && (src->colorspace != COLORSPACE_RGB)) {
        return NULL;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);
    if ((dst == NULL) && (dst->colorspace != COLORSPACE_GRAY)) {
        return NULL;
    }

    int n_pix = src->width * src->height;

    int histgram[256] = { 0 };

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            uint8_t gray   = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);
            dst->row[y][x] = gray;

            histgram[gray]++;
        }
    }

    uint8_t threshold = 0;
    double  sb_max    = 0.0;

    for (int th = 0; th < UINT8_MAX; th++) {
        int    n0 = 0, n1 = 0;
        double w0, w1;
        double m0 = 0.0 , m1 = 0.0;

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

        double sb = w0 * w1 * (m0 - m1) * (m0 - m1);

        if (sb > sb_max) {
            threshold = th;
            sb_max    = sb;
        }
    }

    for (int i = 0; i < n_pix; i++) {
        dst->data[i] = (dst->data[i] < threshold) ? 0 : UINT8_MAX;
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
        for (int j = 0; j < kh; j++) {
            if ((kx < 0) || (kx >= w) || (ky < 0) || (ky >= h)) {
                continue;
            }
            sum += p_ch[ky][kx] * kernel[i * kw + j];
            kx++;
        }
        ky++;
    }

    return (uint8_t)sum;
}

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
