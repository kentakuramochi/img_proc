///
/// @file   sample_filter.c
/// @brief  sample of filtering
///

#include <stdio.h>
#include <stdlib.h>

#include "convert.h"
#include "filter.h"
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

    img_t *dst;

    dst = gaussian_filter(src, 3, 3, 1.3);
    write_pnm(dst, "./build/out/gaussian_3x3_s1.3.ppm", fmt);
    img_delete(dst);

    dst = median_filter(src, 3, 3);
    write_pnm(dst, "./build/out/median_3x3.ppm", fmt);
    img_delete(dst);

    dst = average_filter(src, 3, 3);
    write_pnm(dst, "./build/out/average_3x3.ppm", fmt);
    img_delete(dst);

    dst = motion_filter(src, 3, 3);
    write_pnm(dst, "./build/out/motion_3x3.ppm", fmt);
    img_delete(dst);

    img_t *gray = rgb_to_gray(src);

    dst = maxmin_filter(gray, 3, 3);
    write_pnm(dst, "./build/out/maxmin_3x3.ppm", fmt);
    img_delete(dst);

    dst = diff_filter(gray, true);
    write_pnm(dst, "./build/out/diff_h.ppm", fmt);
    img_delete(dst);

    dst = diff_filter(gray, false);
    write_pnm(dst, "./build/out/diff_v.ppm", fmt);
    img_delete(dst);

    dst = sobel_filter(gray, true);
    write_pnm(dst, "./build/out/sobel_h.ppm", fmt);
    img_delete(dst);

    dst = sobel_filter(gray, false);
    write_pnm(dst, "./build/out/sobel_v.ppm", fmt);
    img_delete(dst);

    dst = prewitt_filter(gray, true);
    write_pnm(dst, "./build/out/prewitt_h.ppm", fmt);
    img_delete(dst);

    dst = prewitt_filter(gray, false);
    write_pnm(dst, "./build/out/prewitt_v.ppm", fmt);
    img_delete(dst);

    dst = laplacian_filter(gray);
    write_pnm(dst, "./build/out/laplacian.ppm", fmt);
    img_delete(dst);

    dst = emboss_filter(gray);
    write_pnm(dst, "./build/out/emboss.ppm", fmt);
    img_delete(dst);

    dst = log_filter(gray, 5, 5, 3);
    write_pnm(dst, "./build/out/log_5x5_s3.ppm", fmt);
    img_delete(dst);

    img_delete(src);
    img_delete(gray);

    return 0;
}
