#ifndef PNM_H
#define PNM_H

#include "img.h"

#include <stdbool.h>

typedef enum {
    PNM_FORMAT_ASCII,
    PNM_FORMAT_BINARY
} PNM_FORMAT;

img_t *read_pnm(const char *src);

bool write_pnm(img_t *img, const char *dst, PNM_FORMAT format);

#endif // PNM_H
