#include <stddef.h>
#include <float.h>
#include "imgproc.h"

// conversion formula by ITU-R BT.601
#define BT601(r, g, b) (0.299 * (r) + 0.587 * (g) + 0.114 * (b))

int rgb_to_gray(img_t *src, img_t *dst)
{
    if (src->colorspace != COLORSPACE_RGB) {
        return RETURN_FAILURE;
    }

    if (dst != NULL) {
        img_free(dst);
    }

    dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            dst->row[y][x] = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);
        }
    }

    return RETURN_SUCCESS;
}

int binarize(img_t *src, img_t *dst, uint8_t threshold)
{
    if (src->colorspace != COLORSPACE_RGB) {
        return RETURN_FAILURE;
    }

    if (dst != NULL) {
        img_free(dst);
    }

    dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

    uint8_t gray;

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            gray = BT601(src->ch[0][y][x], src->ch[1][y][x], src->ch[2][y][x]);

            dst->row[y][x] = (gray < threshold) ? 0 : 255;
        }
    }

    return RETURN_SUCCESS;
}

int binarize_otsu(img_t *src, img_t *dst)
{
    if (src->colorspace != COLORSPACE_RGB) {
        return RETURN_FAILURE;
    }

    if (dst != NULL) {
        img_free(dst);
    }

    dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

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

    return RETURN_SUCCESS;
}

int quantize(img_t *src, img_t *dst, uint8_t level)
{
    if (dst != NULL) {
        img_free(dst);
    }

    dst = img_allocate(src->width, src->height, src->colorspace);

    uint8_t q_unit = 256 / level;

    for (int i = 0; i < (src->channel * src->height * src->width); i++) {
        uint8_t thresh = 0;

        for (int j = 0; j < level; j++) {
            uint8_t qval =  (thresh + 0.5 * q_unit);

            if (src->data[i] >= thresh) {
                dst->data[i] = qval;
            }
            /*if (src->data[i].rgb.g >= thresh) {
                dst->data[i].rgb.g = qval;
            }
            if (src->data[i].rgb.b >= thresh) {
                dst->data[i].rgb.b = qval;
            }*/

            thresh += q_unit;
        }
    }

    return RETURN_SUCCESS;
}
