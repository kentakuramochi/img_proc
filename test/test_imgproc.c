#include "pnm.h"
#include "imgproc.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        puts("USAGE");
        printf("    %s src_pnm_file [--compress]\n", argv[0]);
        puts("OPTIONS");
        puts("    --compress: use binary format (P4/P5/P6) for output"); 
        puts("                ASCII format (P1/P2/P3) is used as default");

        return 1;
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

    cvt_to_gray(src, gray);

    write_pnm(gray, "gray.pgm", fmt);

    img_free(src);
    img_free(gray);

    return 0;
}
