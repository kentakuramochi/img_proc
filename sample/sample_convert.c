#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convert.h"
#include "pnm.h"

const char usage[] = "\
USAGE\n\
    ./sample_convert src_pnm_file [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

PNM_FORMAT fmt = PNM_FORMAT_ASCII;

void test_grayscale(cimg_t *src, const char *dst_file)
{
    cimg_t *dst = rgb_to_gray(src);

    write_pnm(dst, dst_file, fmt);

    cimg_delete(dst);

    return;
}

void test_binarize(cimg_t *src, const char *dst_file, int thresh)
{
    // get grayscale image
    cimg_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    cimg_t *dst = binarize(gray, thresh);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        cimg_delete(gray);
    }

    cimg_delete(dst);

    return;
}

void test_binarize_otsu(cimg_t *src, const char *dst_file)
{
    // get grayscale image
    cimg_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    cimg_t *dst = binarize_otsu(gray);

    write_pnm(dst, dst_file, fmt);

    if (src->channels != CH_GRAY) {
        cimg_delete(gray);
    }

    cimg_delete(dst);

    return;
}

void test_quantize(cimg_t *src, const char *dst_file, int level)
{
    cimg_t *dst = quantize(src, level);

    write_pnm(dst, dst_file, fmt);

    cimg_delete(dst);

    return;
}

void test_histgram(cimg_t *src, const char* file)
{
    // get grayscale image
    cimg_t *gray = (src->channels != CH_GRAY) ? rgb_to_gray(src) : src;

    int histgram[256];
    get_hist(gray, histgram, 8);

    FILE *fp = fopen(file, "w");

    for (int i = 0; i < 256; i++) {
        fprintf(fp, "[%d]: %d\n", i, histgram[i]);
    }

    if (src->channels != CH_GRAY) {
        cimg_delete(gray);
    }

    fclose(fp);
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

    cimg_t *src = read_pnm(argv[1]);

    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    test_grayscale(src, "gray.pgm");
    test_binarize(src, "bin.pgm", 100);
    test_binarize_otsu(src, "bin_otsu.pgm");
    test_quantize(src, "quant_8.ppm", 8);
    test_histgram(src, "histgram.txt");

    cimg_delete(src);

    return 0;
}
