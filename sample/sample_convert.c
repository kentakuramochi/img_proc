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



void test_histgram(img_t *src, const char* file)
{
    img_t *gray;

    if (src->colorspace != COLORSPACE_GRAY) {
        gray = rgb_to_gray(src);
    } else {
        gray = src;
    }

    int histgram[256];
    get_hist(gray, histgram, 8);

    FILE *fp = fopen(file, "w");

    for (int i = 0; i < 256; i++) {
        fprintf(fp, "[%d]: %d\n", i, histgram[i]);
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

    img_t *src = read_pnm(argv[1]);

    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    test_grayscale(src, "gray.pgm");
    test_binarize(src, "bin.pgm", 100);
    test_binarize_otsu(src, "bin_otsu.pgm");
    test_quantize(src, "quant_8.ppm", 8);
    test_histgram(src, "histgram.txt");

    img_free(src);

    return 0;
}
