///
/// @file   sample_convert.c
/// @brief  sample of image conversion
/// @author kentakuramochi
///

#include <stdio.h>
#include <stdlib.h>

#include "convert.h"
#include "pnm.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("error: specify PNM image\n");
        return 1;
    }

    cimg_t *src = read_pnm(argv[1]);
    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    PNM_FORMAT fmt = PNM_FORMAT_ASCII;

    cimg_t *gray;

    gray = rgb_to_gray(src);
    write_pnm(gray, "gray.pgm", fmt);

    cimg_t *dst;

    dst = binarize(src, 100);
    write_pnm(dst, "bin_100.pgm", fmt);
    cimg_delete(dst);

    dst = binarize_otsu(src);
    write_pnm(dst, "bin_otsu.pgm", fmt);
    cimg_delete(dst);

    dst = quantize(src, 8);
    write_pnm(dst, "quant_8.ppm", fmt);
    cimg_delete(dst);

    int histogram[256];
    get_hist(gray, histogram, 8);

    FILE *fp = fopen("histogram.txt", "w");
    for (int i = 0; i < 256; i++) {
        fprintf(fp, "[%d]: %d\n", i, histogram[i]);
    }
    fclose(fp);

    dst = expand_hist(src, 20, 200);
    write_pnm(dst, "normhist_20-200.ppm", fmt);
    cimg_delete(dst);

    dst = normarize_hist(src, 128, 52);
    write_pnm(dst, "normhist_m128-s52.ppm", fmt);
    cimg_delete(dst);

    dst = equalize_hist(src);
    write_pnm(dst, "eqhist.ppm", fmt);
    cimg_delete(dst);

    dst = gamma_correction(src, 2.2);
    write_pnm(dst, "gamma_2.2.ppm", fmt);
    cimg_delete(dst);

    cimg_delete(src);

    cimg_delete(gray);

    return 0;
}
