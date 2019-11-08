#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convert.h"
#include "pool.h"
#include "pnm.h"

const char usage[] = "\
USAGE\n\
    ./sample_pool src_pnm_file [--compress]\n\
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

    test_avgpool(src, "avgpool_8x8.ppm", 8, 8);
    test_maxpool(src, "maxpool_8x8.ppm", 8, 8);

    img_free(src);

    return 0;
}
