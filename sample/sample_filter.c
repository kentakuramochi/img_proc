///
/// @file   sample_filter.c
/// @brief  sample of filtering
/// @author kentakuramochi
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

    cimg_t *src = read_pnm(argv[1]);
    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    PNM_FORMAT fmt = PNM_FORMAT_ASCII;

    cimg_t *dst;

    dst = gaussian_filter(src, 3, 3, 1.3);
    write_pnm(dst, "gaussian_3x3_s1.3.ppm", fmt);
    cimg_delete(dst);

    dst = median_filter(src, 3, 3);
    write_pnm(dst, "median_3x3.ppm", fmt);
    cimg_delete(dst);

    dst = average_filter(src, 3, 3);
    write_pnm(dst, "average_3x3.ppm", fmt);
    cimg_delete(dst);

    dst = motion_filter(src, 3, 3);
    write_pnm(dst, "motion_3x3.ppm", fmt);
    cimg_delete(dst);

    cimg_t *gray = rgb_to_gray(src);

    dst = maxmin_filter(gray, 3, 3);
    write_pnm(dst, "maxmin_3x3.ppm", fmt);
    cimg_delete(dst);

    dst = diff_filter(gray, true);
    write_pnm(dst, "diff_h.ppm", fmt);
    cimg_delete(dst);

    dst = diff_filter(gray, false);
    write_pnm(dst, "diff_v.ppm", fmt);
    cimg_delete(dst);

    dst = sobel_filter(gray, true);
    write_pnm(dst, "diff_h.ppm", fmt);
    cimg_delete(dst);

    dst = sobel_filter(gray, false);
    write_pnm(dst, "diff_v.ppm", fmt);
    cimg_delete(dst);

    dst = prewitt_filter(gray, true);
    write_pnm(dst, "prewitt_h.ppm", fmt);
    cimg_delete(dst);

    dst = prewitt_filter(gray, false);
    write_pnm(dst, "prewitt_v.ppm", fmt);
    cimg_delete(dst);

    dst = laplacian_filter(gray);
    write_pnm(dst, "laplacian.ppm", fmt);
    cimg_delete(dst);

    dst = emboss_filter(gray);
    write_pnm(dst, "emboss.ppm", fmt);
    cimg_delete(dst);

    dst = log_filter(gray, 5, 5, 3);
    write_pnm(dst, "log_5x5_s3.ppm", fmt);
    cimg_delete(dst);

    cimg_delete(src);
    cimg_delete(gray);

    return 0;
}
