#include "pnm.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        puts("description:");
        puts("    read PNM file and write the same image as PNM file with arbitary name");
        puts("usage:");
        printf("    %s src_pnm_file dst_pnm_file [--compress]\n", argv[0]);
        puts("options:");
        puts("    --compress: use binary format (P4/P5/P6) for output"); 
        puts("                ASCII format (P1/P2/P3) is used as default");
        return 1;
    }

    PNM_FORMAT fmt = PNM_FORMAT_ASCII;
    if (argc == 4) {
        if (strcmp(argv[3], "--compress") == 0) {
            fmt = PNM_FORMAT_BINARY;
        } else {
            printf("error: invalid option \"%s\"\n", argv[3]);
        }
    }

    img_t *img = read_pnm(argv[1]);
    if (img == NULL) {
        printf("error: failed to read \"%s\"\n", argv[1]);
        return -1;
    }

    write_pnm(img, argv[2], fmt);

    img_free(img);

    return 0;
}
