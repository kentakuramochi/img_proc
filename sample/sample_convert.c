///
/// @file   sample_convert.c
/// @brief  sample of image conversion
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

    img_t *src = read_pnm(argv[1]);
    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    PNM_FORMAT fmt = PNM_FORMAT_ASCII;

    img_t *gray;

    gray = rgb_to_gray(src);
    write_pnm(gray, "./build/sample/out/gray.pgm", fmt);

    img_t *dst;

    dst = binarize(src, 100);
    write_pnm(dst, "./build/sample/out/bin_100.pgm", fmt);
    img_delete(dst);

    dst = binarize_otsu(src);
    write_pnm(dst, "./build/sample/out/bin_otsu.pgm", fmt);
    img_delete(dst);

    dst = quantize(src, 8);
    write_pnm(dst, "./build/sample/out/quant_8.ppm", fmt);
    img_delete(dst);

    int histogram[256];
    get_hist(gray, histogram, 8);

    FILE *fp = fopen("./build/sample/out/histogram.txt", "w");
    for (int i = 0; i < 256; i++) {
        fprintf(fp, "[%d]: %d\n", i, histogram[i]);
    }
    fclose(fp);

    dst = expand_hist(src, 20, 200);
    write_pnm(dst, "./build/sample/out/normhist_20-200.ppm", fmt);
    img_delete(dst);

    dst = normarize_hist(src, 128, 52);
    write_pnm(dst, "./build/sample/out/normhist_m128-s52.ppm", fmt);
    img_delete(dst);

    dst = equalize_hist(src);
    write_pnm(dst, "./build/sample/out/eqhist.ppm", fmt);
    img_delete(dst);

    dst = gamma_correction(src, 2.2);
    write_pnm(dst, "./build/sample/out/gamma_2.2.ppm", fmt);
    img_delete(dst);

    dst = nearest_neighbor(src, 1.5, 1.5);
    write_pnm(dst, "./build/sample/out/nearest_x1.5.ppm", fmt);
    img_delete(dst);

    dst = bilinear(src, 1.5, 1.5);
    write_pnm(dst, "./build/sample/out/bilnear_x1.5.ppm", fmt);
    img_delete(dst);

    dst = biqubic(src, 1.5, 1.5);
    write_pnm(dst, "./build/sample/out/biqubic_x1.5.ppm", fmt);
    img_delete(dst);

    img_delete(src);

    img_delete(gray);

    return 0;
}
