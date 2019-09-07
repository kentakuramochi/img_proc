#include "pnm.h"
#include "imgproc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char usage[] = "\
USAGE\n\
    ./test_binarize src_pnm_file threshold [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

int main(int argc, char *argv[])
{
    if ((argc < 3) || (argc > 4)) {
        printf("%s", usage);
        return -1;
    }

    PNM_FORMAT fmt = PNM_FORMAT_ASCII;

    if (argc == 4) {
        if (strcmp(argv[3], "--compress") == 0) {
            fmt = PNM_FORMAT_BINARY;
        } else {
            printf("error: invalid option \"%s\"\n", argv[2]);
            return -1;
        }
    }

    uint8_t threshold = atoi(argv[2]);

    img_t *src = read_pnm(argv[1]);

    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        return -1;
    }

    img_t *dst = img_allocate(src->width, src->height, COLORSPACE_GRAY);

    binarize(src, dst, threshold);
    write_pnm(dst, "bin.pgm", fmt);

    binarize_otsu(src, dst);
    write_pnm(dst, "bin_otsu.pgm", fmt);

    img_free(src);
    img_free(dst);

    return 0;
}
