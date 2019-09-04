#include <stddef.h>
#include <float.h>
#include "imgproc.h"

void rgb_to_gray(img_t *src, img_t *dst)
{
    if (src->color_type != COLOR_TYPE_RGB) {
        return;
    }

    dst->color_type = COLOR_TYPE_GRAY;

    for (int i = 0; i < src->width * src->height; i++) {
        // BT.601
        dst->data[i].gray = (0.299 * src->data[i].rgb.r +
                             0.587 * src->data[i].rgb.g +
                             0.114 * src->data[i].rgb.b);
    }

    return;
}

void binarize(img_t *src, img_t *dst, uint8_t threshold)
{
    if (src->color_type != COLOR_TYPE_RGB) {
        return;
    }

    for (int i = 0; i < src->width * src->height; i++) {
        // BT.601
        uint8_t gray = (0.299 * src->data[i].rgb.r +
                        0.587 * src->data[i].rgb.g +
                        0.114 * src->data[i].rgb.b);

        dst->data[i].gray = (gray < threshold) ? 0 : 255;
    }

    return;
}

void binarize_otsu(img_t *src, img_t *dst)
{
    if (src->color_type != COLOR_TYPE_RGB) {
        return;
    }

    int n_pix = src->width * src->height;

    int histgram[256] = { 0 };

    for (int i = 0; i < n_pix; i++) {
        // BT.601
        uint8_t gray = (0.299 * src->data[i].rgb.r +
                        0.587 * src->data[i].rgb.g +
                        0.114 * src->data[i].rgb.b);

        dst->data[i].gray = gray;

        histgram[gray]++;
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

        w0 = n0 / n_pix;
        w1 = n1 / n_pix;
        m0 /= n0;
        m1 /= n1;

        double sb = w0 * w1 * (m0 - m1) * (m0 - m1);

        if (sb > sb_max) {
            threshold = th;
            sb_max    = sb;
        }
    }

    for (int i = 0; i < n_pix; i++) {
        dst->data[i].gray = (dst->data[i].gray < threshold) ? 0 : 255;
    }

    return;
}
