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

    img_t *dst = binarize(src, threshold);

    write_pnm(dst, "bin.pgm", fmt);

    img_t *dst2 = binarize_otsu(src);

    write_pnm(dst2, "bin_otsu.pgm", fmt);

    img_free(src);
    img_free(dst);
    img_free(dst2);

    return 0;
}
