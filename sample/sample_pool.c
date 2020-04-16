///
/// @file   sample_pool.c
/// @brief  sample of pooling
/// @author kentakuramochi
///

#include <stdio.h>
#include <stdlib.h>

#include "convert.h"
#include "pool.h"
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
    
    dst = average_pooling(src, 8, 8);
    write_pnm(dst, "avgpool_8x8.ppm", fmt);
    img_delete(dst);

    dst = max_pooling(src, 8, 8);
    write_pnm(dst, "maxpool_8x8.ppm", fmt);
    img_delete(dst);

    img_delete(src);

    return 0;
}
