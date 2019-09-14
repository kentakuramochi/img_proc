#include "pnm.h"
#include "imgproc.h"
#include <stdio.h>
#include <string.h>

const char usage[] = "\
USAGE\n\
    ./test_imgproc src_pnm_file [--compress]\n\
OPTIONS\n\
    --compress: use binary format (P4/P5/P6) for output\n\
                ASCII format (P1/P2/P3) is used as default\n";

int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("%s", usage);
        return -1;
    }

    PNM_FORMAT fmt = PNM_FORMAT_ASCII;

    if (argc == 3) {
        if (strcmp(argv[2], "--compress") == 0) {
            fmt = PNM_FORMAT_BINARY;
        } else {
            printf("error: invalid option \"%s\"\n", argv[2]);
            return -1;
        }
    }

    img_t *src = read_pnm(argv[1]);

    if (src == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        return -1;
    }

    img_t *dst = rgb_to_gray(src);

    write_pnm(dst, "gray.pgm", fmt);

    img_free(src);
    img_free(dst);

    return 0;
}
