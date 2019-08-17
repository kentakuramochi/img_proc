#ifndef PNM_H
#define PNM_H

#include "img.h"

typedef enum {
    PNM_FORMAT_ASCII  = 0,
    PNM_FORMAT_BINARY = 3
} PNM_FORMAT;

img_t *read_pnm(const char *src);

int write_pnm(img_t *img, const char *dst, PNM_FORMAT format);

#endif // PNM_H
