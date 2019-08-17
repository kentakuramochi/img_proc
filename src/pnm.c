#include "pnm.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define NORMALIZE_UINT8(value, max) ((value) * 255 / (max))

static char get_next_char(FILE *fp)
{
    char c;
    int is_comment = 0;

    while ((c = getc(fp)) != EOF) {
        if (is_comment == 1) {
            if (c == '\n' || c == '\r') {
                is_comment = 0;
            }
            continue;
        }

        if (c == '#') {
            is_comment = 1;
            continue;
        }

        if (!isspace(c)) {
            break;
        }
    }

    return c;
}

static int get_token(FILE *fp, char *buf, size_t size)
{
    char c = get_next_char(fp);
    int  i = 0;

    while ((c != EOF) && !isspace(c) && (i < (size - 1))) {
        buf[i++] = c;
        c = getc(fp);
    }

    buf[i] = '\0';

    return i;
}

static int parse_int(const char *str)
{
    int num = 0;

    if (str[0] == '\0') {
        return -1;
    }

    for (int i = 0; str[i] != 0; i++) {
        if (!isdigit((int)str[i])) {
            return -1;
        }

        num *= 10;
        num += str[i] - '0';
    }

    return num;
}

static int get_next_int(FILE *fp)
{
    char token[4];

    get_token(fp, token, sizeof(token));

    return parse_int(token);
}

static int read_pnm_ascii(FILE *fp, img_t *img, int max, uint8_t n_magic)
{
    int tmp;

    if (n_magic == 1) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                if ((tmp = get_next_int(fp)) < 0) {
                    return -1;
                }
                img->map[y][x].gray = tmp;
            }
        }
    } else if (n_magic == 2) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                if ((tmp = get_next_int(fp)) < 0) {
                    return -1;
                }
                img->map[y][x].gray = NORMALIZE_UINT8(tmp, max);
            }
        }
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                if ((tmp = get_next_int(fp)) < 0) {
                    return -1;
                }
                img->map[y][x].rgb.r = NORMALIZE_UINT8(tmp, max);

                if ((tmp = get_next_int(fp)) < 0) {
                    return -1;
                }
                img->map[y][x].rgb.g = NORMALIZE_UINT8(tmp, max);

                if ((tmp = get_next_int(fp)) < 0) {
                    return -1;
                }
                img->map[y][x].rgb.b = NORMALIZE_UINT8(tmp, max);
            }
        }
    }

    return 0;
}

static int read_pnm_binary(FILE *fp, img_t *img, int max, uint8_t n_magic)
{
    uint8_t *row;
    uint8_t *item;
    int     stride;

    if (n_magic == 4) {
        stride = sizeof(uint8_t) * ((img->width + 7) / 8);
        if ((row = (uint8_t*)malloc(stride)) == NULL) {
            return -1;
        }

        int pos   = 0;
        int shift = 8;

        for (int y = 0; y < img->height; y++) {
            pos   = 0;
            shift = 8;

            if (fread(row, stride, 1, fp) != 1) {
                free(row);
                return -1;
            }

            for (int x = 0; x < img->width; x++) {
                shift--;

                img->map[y][x].gray = (row[pos] >> shift) & 1;

                if (shift == 0) {
                    shift = 8;
                    pos++;
                }
            }
        }
    } else if (n_magic == 5) {
        stride = sizeof(uint8_t) * img->width;
        if ((row = (uint8_t*)malloc(stride)) == NULL) {
            return -1;
        }

        for (int y = 0; y < img->height; y++) {
            if (fread(row, stride, 1, fp) != 1) {
                free(row);
                return -1;
            }
            item = row;

            for (int x = 0; x < img->width; x++) {
                img->map[y][x].gray = NORMALIZE_UINT8(*(item++), max);
            }
        }
    } else {
        stride = (sizeof(uint8_t) * 3) * img->width;
        if ((row = (uint8_t*)malloc(stride)) == NULL) {
            return -1;
        }

        for (int y = 0; y < img->height; y++) {
            if (fread(row, stride, 1, fp) != 1) {
                free(row);
                return -1;
            }
            item = row;

            for (int x = 0; x < img->width; x++) {
                img->map[y][x].rgb.r = NORMALIZE_UINT8(*(item++), max);
                img->map[y][x].rgb.g = NORMALIZE_UINT8(*(item++), max);
                img->map[y][x].rgb.b = NORMALIZE_UINT8(*(item++), max);
            }
        }
    }

    free(row);

    return 0;
}

img_t *read_pnm(const char *src)
{
    FILE *fp = fopen(src, "rb");
    if (fp == NULL) {
        return NULL;
    }

    char buf_magic[3];
    get_token(fp, buf_magic, sizeof(buf_magic));
    uint8_t n_magic = buf_magic[1] - '0';
    if ((n_magic  < 1) || (n_magic > 6)) {
        fclose(fp);
        return NULL;
    }

    color_type_t color_type;
    if (n_magic <= 3) {
        color_type = (color_type_t)(n_magic);
    } else {
        color_type = (color_type_t)(n_magic - 3);
    }

    uint32_t width  = get_next_int(fp);
    uint32_t height = get_next_int(fp);
    if ((width <= 0) || (height <= 0)) {
        fclose(fp);
        return NULL;
    }

    uint8_t max;
    if ((n_magic == 1) || (n_magic == 4)) {
        max = 1;
    } else {
        max = get_next_int(fp);
        if ((max < 1) || (max > 255)) {
            fclose(fp);
            return NULL;
        }
    }

    img_t *img = img_allocate(width, height, color_type);

    if (n_magic <= 3) {
        if (read_pnm_ascii(fp, img, max, n_magic) < 0) {
            img = NULL;
        }
    } else {
        if (read_pnm_binary(fp, img, max, n_magic) < 0) {
            img = NULL;
        }
    }

    fclose(fp);

    return img;
}

static int write_pnm_ascii(FILE *fp, img_t *img, uint8_t n_magic)
{
    if ((n_magic == 1) || (n_magic == 2)) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                fprintf(fp, "%u ", img->map[y][x].gray);
            }
            fseek(fp, -1, SEEK_CUR);
            fputc('\n', fp);
        }
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                fprintf(fp, "%u %u %u ",
                    img->map[y][x].rgb.r, img->map[y][x].rgb.g, img->map[y][x].rgb.b);
            }
            fseek(fp, -1, SEEK_CUR);
            fputc('\n', fp);
        }
    }

    return 0;
}

static int write_pnm_binary(FILE *fp, img_t *img, uint8_t n_magic)
{
    if (n_magic == 4) {
        int     shift;
        uint8_t packed_val;
        for (int y = 0; y < img->height; y++) {
            shift      = 8;
            packed_val = 0;
            for (int x = 0; x < img->width; x++) {
                shift--;
                packed_val |= (img->map[y][x].gray << shift);

                if (shift == 0) {
                    putc(packed_val, fp);
                    shift      = 8;
                    packed_val = 0;
                }
            }

            if (shift != 8) {
                putc(packed_val, fp);
            }
        }
    } else if (n_magic == 5) {
        for (int i = 0; i < (img->width * img->height); i++) {
            putc(img->data[i].gray, fp);
        }
    } else {
        for (int i = 0; i < (img->width * img->height); i++) {
            putc(img->data[i].rgb.r, fp);
            putc(img->data[i].rgb.g, fp);
            putc(img->data[i].rgb.b, fp);
        }
    }

    return 0;
}

int write_pnm(img_t *img, const char *dst, PNM_FORMAT format)
{
    if (img == NULL) {
        return -1;
    }

    uint8_t n_magic = (int)img->color_type + (int)format;
    printf("magic: %d\n", n_magic);
    if ((n_magic < 1) || (n_magic > 6)) {
        return -1;
    }

    FILE *fp = fopen(dst, "wb");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "P%d\n", n_magic);
    fprintf(fp, "%u %u\n", img->width, img->height);

    if ((n_magic != 1) && (n_magic != 4)) {
        fprintf(fp, "%u\n", 255);
    }

    if (n_magic <= 3) {
        write_pnm_ascii(fp, img, n_magic);
    } else {
        write_pnm_binary(fp, img, n_magic);
    }

    fclose(fp);

    return 0;
}
