#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filter.h"
#include "pnm.h"

const char usage[] = "\
USAGE\n\
    ./test_imgproc src_pnm_file [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

PNM_FORMAT fmt = PNM_FORMAT_ASCII;

void test_gaussianfilter(img_t *src, const char *dst_file, int kernel_size, double stddev)
{
    img_t *dst = gaussian_filter(src, kernel_size, kernel_size, stddev);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_medianfilter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = median_filter(src, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_averagefilter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = average_filter(src, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_motionfilter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = motion_filter(src, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_maxminfilter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = maxmin_filter(gray, kernel_size, kernel_size);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_difffilter(img_t *src, const char *dst_file, bool is_horizontal)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = diff_filter(gray, is_horizontal);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_sobelfilter(img_t *src, const char *dst_file, bool is_horizontal)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = sobel_filter(gray, is_horizontal);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_prewittfilter(img_t *src, const char *dst_file, bool is_horizontal)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = prewitt_filter(gray, is_horizontal);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_laplacianfilter(img_t *src, const char *dst_file)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = laplacian_filter(gray);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_embossfilter(img_t *src, const char *dst_file)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = emboss_filter(gray);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_logfilter(img_t *src, const char *dst_file, int kernel_size, double sigma)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    img_t *dst = log_filter(gray, kernel_size, kernel_size, sigma);

    if (src->colorspace != COLORSPACE_GRAY) {
        img_free(gray);
    }

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

int verify_args(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("%s", usage);
        return 1;
    }

    if (argc == 3) {
        if (strcmp(argv[2], "--compress") == 0) {
            fmt = PNM_FORMAT_BINARY;
        } else {
            printf("error: invalid option \"%s\"\n", argv[2]);
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (verify_args(argc, argv)) {
        exit(EXIT_FAILURE);
    }

    img_t *src = read_pnm(argv[1]);

    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    test_avgpool(src, "avgpool_8x8.ppm", 8, 8);
    test_maxpool(src, "maxpool_8x8.ppm", 8, 8);

    img_free(src);

    return 0;
}
