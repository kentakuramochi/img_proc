#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convert.h"
#include "filter.h"
#include "pnm.h"

const char usage[] = "\
USAGE\n\
    ./sample_filter src_pnm_file [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

PNM_FORMAT fmt = PNM_FORMAT_ASCII;

void test_gaussian_filter(img_t *src, const char *dst_file, int kernel_size, double stddev)
{
    img_t *dst = gaussian_filter(src, kernel_size, kernel_size, stddev);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_median_filter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = median_filter(src, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_average_filter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = average_filter(src, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_motion_filter(img_t *src, const char *dst_file, int kernel_size)
{
    img_t *dst = motion_filter(src, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    img_free(dst);

    return;
}

void test_maxmin_filter(img_t *src, const char *dst_file, int kernel_size)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = maxmin_filter(gray, kernel_size, kernel_size);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

    img_free(dst);

    return;
}

void test_diff_filter(img_t *src, const char *dst_file, bool is_horizontal)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = diff_filter(gray, is_horizontal);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

    img_free(dst);

    return;
}

void test_sobel_filter(img_t *src, const char *dst_file, bool is_horizontal)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = sobel_filter(gray, is_horizontal);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

    img_free(dst);

    return;
}

void test_prewitt_filter(img_t *src, const char *dst_file, bool is_horizontal)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = prewitt_filter(gray, is_horizontal);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

    img_free(dst);

    return;
}

void test_laplacian_filter(img_t *src, const char *dst_file)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = laplacian_filter(gray);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

    img_free(dst);

    return;
}

void test_emboss_filter(img_t *src, const char *dst_file)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = emboss_filter(gray);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

    img_free(dst);

    return;
}

void test_log_filter(img_t *src, const char *dst_file, int kernel_size, double sigma)
{
    // get grayscale image
    img_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    img_t *dst = log_filter(gray, kernel_size, kernel_size, sigma);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        img_free(gray);
    }

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

    test_gaussian_filter(src, "gaussian_3x3.ppm", 3, 1.3);
    test_median_filter(src, "median_3x3.ppm", 3);
    test_average_filter(src, "average_3x3.ppm", 3);
    test_motion_filter(src, "motion_3x3.ppm", 3);
    test_maxmin_filter(src, "maxmin_3x3.pgm", 3);
    test_diff_filter(src, "diff_h.pgm", true);
    test_diff_filter(src, "diff_v.pgm", false);
    test_sobel_filter(src, "sobel_h.pgm", true);
    test_sobel_filter(src, "sobel_v.pgm", false);
    test_prewitt_filter(src, "prewitt_h.pgm", true);
    test_prewitt_filter(src, "prewitt_v.pgm", false);
    test_laplacian_filter(src, "laplacian.pgm");
    test_emboss_filter(src, "emboss.pgm");
    test_log_filter(src, "log_5x5.pgm", 5, 3);

    img_free(src);

    return 0;
}
