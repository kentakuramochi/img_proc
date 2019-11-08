#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pool.h"
#include "pnm.h"

const char usage[] = "\
USAGE\n\
    ./test_imgproc src_pnm_file [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

PNM_FORMAT fmt = PNM_FORMAT_ASCII;

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

    test_gaussianfilter(src, "gaussian_3x3.ppm", 3, 1.3);
    test_medianfilter(src, "median_3x3.ppm", 3);
    test_averagefilter(src, "average_3x3.ppm", 3);
    test_motionfilter(src, "motion_3x3.ppm", 3);
    test_maxminfilter(src, "maxmin_3x3.pgm", 3);
    test_difffilter(src, "diff_h.pgm", true);
    test_difffilter(src, "diff_v.pgm", false);
    test_sobelfilter(src, "sobel_h.pgm", true);
    test_sobelfilter(src, "sobel_v.pgm", false);
    test_prewittfilter(src, "prewitt_h.pgm", true);
    test_prewittfilter(src, "prewitt_v.pgm", false);
    test_laplacianfilter(src, "laplacian.pgm");
    test_embossfilter(src, "emboss.pgm");
    test_logfilter(src, "log_5x5.pgm", 5, 3);

    img_free(src);

    return 0;
}
