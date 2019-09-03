#include <stddef.h>
#include "imgproc.h"

void rgb_to_gray(img_t *src, img_t *dst)
{
    if (src->color_type != COLOR_TYPE_RGB) {
        return NULL;
    }

    dst->color_type = COLOR_TYPE_GRAY;

    for (int i = 0; i < src->width * src->height; i++) {
        // BT.601
        dst->data[i].gray = (0.299 * src->data[i].rgb.r +
                             0.587 * src->data[i].rgb.g +
                             0.114 * src->data[i].rgb.b);
    }

    return;
}
