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

    img_t *gray = img_allocate(src->width, src->height, COLOR_TYPE_GRAY);
    rgb_to_gray(src, gray);
    write_pnm(gray, "gray.pgm", fmt);
    img_t *bin = img_allocate(src->width, src->height, COLOR_TYPE_GRAY);
    binarize(src, bin, 100);
    write_pnm(bin, "bin.pgm", fmt);

    binarize_otsu(src, bin);
    write_pnm(bin, "bin_otsu.pgm", fmt);

    img_t *quantized = img_allocate(src->width, src->height, COLOR_TYPE_RGB);
    quantize(src, quantized, 4);
    write_pnm(quantized, "quantized.ppm", fmt);

    img_free(src);
    img_free(gray);
    img_free(bin);
    img_free(quantized);

    return 0;
}
