#include "pnm.h"
#include "imgproc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char usage[] = "\
USAGE\n\
    ./test_imgproc src_pnm_file [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

PNM_FORMAT fmt = PNM_FORMAT_ASCII;

// ********************
// test functions
// ********************

void test_grayscale(img_t *src, const char *dst_file)
{
    img_t *dst = rgb_to_gray(src);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_binarize(img_t *src, const char *dst_file, int thresh)
{
    img_t *dst = binarize(src, thresh);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_binarize_otsu(img_t *src, const char *dst_file)
{
    img_t *dst = binarize_otsu(src);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_quantize(img_t *src, const char *dst_file, int level)
{
    img_t *dst = quantize(src, level);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_avgpool(img_t *src, const char *dst_file, int kw, int kh)
{
    img_t *dst = average_pooling(src, kw, kh);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_maxpool(img_t *src, const char *dst_file, int kw, int kh)
{
    img_t *dst = max_pooling(src, kw, kh);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_gaussianfilter(img_t *src, const char *dst_file, int kernel_size, double stddev)
{
    img_t *dst = gaussian_filter(src, kernel_size, stddev);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_medianfilter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = median_filter(src, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_averagefilter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = average_filter(src, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

int verify_args(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("%s", usage);
        return RETURN_FAILURE;
    }

    if (argc == 3) {
        if (strcmp(argv[2], "--compress") == 0) {
            fmt = PNM_FORMAT_BINARY;
        } else {
            printf("error: invalid option \"%s\"\n", argv[2]);
            return RETURN_FAILURE;
        }
    }

    return RETURN_SUCCESS;
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

    // *****************
    // test
    // *****************
    test_grayscale(src, "gray.pgm");
    test_binarize(src, "bin.pgm", 100);
    test_binarize_otsu(src, "bin_otsu.pgm");
    test_quantize(src, "quant_8.ppm", 8);
    test_avgpool(src, "avgpool_8x8.ppm", 8, 8);
    test_maxpool(src, "maxpool_8x8.ppm", 8, 8);
    test_gaussianfilter(src, "gaussian_3x3.ppm", 3, 1.3);
    test_medianfilter(src, "median_3x3.ppm", 3);
    test_averagefilter(src, "average_3x3.ppm", 3);

    img_free(src);

    return EXIT_SUCCESS;
}
