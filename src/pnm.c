///
/// @file   pnm.c
/// @brief  PNM image utilities
/// @author kentakuramochi
///

#include "pnm.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

///
/// @fn     fix_to_uint8
/// @brief  fix quantized pixel value
/// @param  value   [in]   pixel value from PNM file
/// @param  max     [in]   max value in PNM file
/// @return fixed pixel value (unsigned 8bit)
///
static inline uint8_t fix_to_uint8(uint8_t value, uint8_t max)
{
    return (value * UINT8_MAX / max);
}

///
/// @fn     get_next_char
/// @brief  get next charecter from file stream
/// @param  fp  [in]    input file stream
/// @return character valid as token of PNM data
/// @note   skip comment ('#' to '\n')
///
static char get_next_char(FILE *fp)
{
    char c;
    bool is_comment = false;

    while ((c = getc(fp)) != EOF) {
        // comment: from '#' to '\n'
        if (is_comment) {
            if ((c == '\n') || (c == '\r')) {
                is_comment = false;
            }
            continue;
        }

        if (c == '#') {
            is_comment = true;
            continue;
        }

        if (!isspace(c)) {
            break;
        }
    }

    return c;
}

///
/// @fn     get_token
/// @brief  get token valid as PNM data
/// @param  fp      [in]    input file stream
/// @param  buf     [out]   pointer to buffer for storing token
/// @param  size    [in]    buffer size
/// @return token length, 0 if there is no token
///
static size_t get_token(FILE *fp, char *buf, size_t size)
{
    char c = get_next_char(fp);
    size_t i = 0;

    while ((c != EOF) && !isspace(c) && (i < (size - 1))) {
        buf[i++] = c;
        c = getc(fp);
    }

    buf[i] = '\0';

    return i;
}

///
/// @fn     parse_int
/// @brief  parse string as integer
/// @param  str [in]    string to be parsed
/// @return parsed integer, -1 if failed
/// @note   cannot parse nagative value
///
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

///
/// @fn     get_next_int
/// @brief  get next integer from input file stream
/// @param  fp  [in]    input file stream
/// @return parsed integer, -1 if failed
/// @note   expected to return positive integer
///
static int get_next_int(FILE *fp)
{
    // max of int will be 10 digits
    char token[10 + 1];
    get_token(fp, token, sizeof(token));

    return parse_int(token);
}

///
/// @fn     read_pnm_ascii
/// @brief  read ASCII format PNM image file
/// @param  fp      [in]    input file stream
/// @param  img     [out]   image structure
/// @param  max     [in]    max value in image file
/// @param  n_magic [in]    magic number of PNM image (1/2/3)
/// @retval true    succeed to read file
/// @retval false   fail to read file
/// @note   not support 16bit image
///
static bool read_pnm_ascii(FILE *fp, cimg_t *img, int max, uint8_t n_magic)
{
    int tmp;

    // binary
    if (n_magic == 1) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                if ((tmp = get_next_int(fp)) < 0) {
                    return false;
                }
                img->data[y * img->stride + x] = fix_to_uint8(tmp, 1);
            }
        }
    // grayscale
    } else if (n_magic == 2) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                if ((tmp = get_next_int(fp)) < 0) {
                    return false;
                }
                img->data[y * img->stride + x] = fix_to_uint8(tmp, max);
            }
        }
    // color
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                uint8_t *pixel = &img->data[y * img->stride + x * img->channels];

                // red
                if ((tmp = get_next_int(fp)) < 0) {
                    return false;
                }
                *pixel = fix_to_uint8(tmp, max);

                // green
                if ((tmp = get_next_int(fp)) < 0) {
                    return false;
                }
                *(pixel + 1) = fix_to_uint8(tmp, max);

                // blue
                if ((tmp = get_next_int(fp)) < 0) {
                    return false;
                }
                *(pixel + 2) = fix_to_uint8(tmp, max);
            }
        }
    }

    return true;
}

///
/// @fn     read_pnm_binary
/// @brief  read binary format PNM image file
/// @param  fp      [in]    input file stream
/// @param  img     [out]   image structure
/// @param  max     [in]    max value in image file
/// @param  n_magic [in]    magic number of PNM image (4/5/6)
/// @retval true    succeed to read file
/// @retval false   fail to read file
/// @note   not support 16bit image
///
static bool read_pnm_binary(FILE *fp, cimg_t *img, int max, uint8_t n_magic)
{
    uint8_t *row;
    uint8_t *item;

    // binary
    if (n_magic == 4) {
        int stride = sizeof(uint8_t) * ((img->width + 7) / 8);

        if ((row = (uint8_t*)malloc(stride)) == NULL) {
            return false;
        }

        int pos   = 0;
        int shift = 8;

        for (int y = 0; y < img->height; y++) {
            pos   = 0;
            shift = 8;

            if (fread(row, stride, 1, fp) != 1) {
                free(row);
                return false;
            }

            // parse packed 8bit to each pixels
            for (int x = 0; x < img->width; x++) {
                shift--;

                img->data[y * img->stride + x] = (row[pos] >> shift) & 1;

                if (shift == 0) {
                    shift = 8;
                    pos++;
                }
            }
        }
    // grayscale
    } else if (n_magic == 5) {
        if ((row = (uint8_t*)malloc(img->stride)) == NULL) {
            return false;
        }

        for (int y = 0; y < img->height; y++) {
            if (fread(row, img->stride, 1, fp) != 1) {
                free(row);
                return false;
            }

            item = row;

            for (int x = 0; x < img->width; x++) {
                img->data[y * img->stride + x] = fix_to_uint8(*(item++), max);
            }
        }
    // color
    } else {
        if ((row = (uint8_t*)malloc(img->stride)) == NULL) {
            return false;
        }

        for (int y = 0; y < img->height; y++) {
            if (fread(row, img->stride, 1, fp) != 1) {
                free(row);
                return false;
            }

            item = row;

            uint8_t *rowdata = &img->data[y * img->stride];

            for (int x = 0; x < img->width; x++) {
                *(rowdata++) = fix_to_uint8(*(item++), max);    // red
                *(rowdata++) = fix_to_uint8(*(item++), max);    // green
                *(rowdata++) = fix_to_uint8(*(item++), max);    // blue
            }
        }
    }

    free(row);

    return true;
}

cimg_t *read_pnm(const char *src)
{
    FILE *fp = fopen(src, "rb");

    if (fp == NULL) {
        return NULL;
    }

    // magic number
    char buf_magic[2 + 1];
    get_token(fp, buf_magic, sizeof(buf_magic));
    uint8_t n_magic = buf_magic[1] - '0';
    if ((n_magic  < 1) || (n_magic > 6)) {
        fclose(fp);
        return NULL;
    }

    int channels;
    switch (n_magic) {  
        // ASCII format
        case 1: channels = CH_GRAY; break;
        case 2: channels = CH_GRAY; break;
        case 3: channels = CH_RGB;  break;
        // binary format
        case 4: channels = CH_GRAY; break;
        case 5: channels = CH_GRAY; break;
        case 6: channels = CH_RGB;  break;
        default:
            fclose(fp);
            return NULL;
            break;
    }

    // size
    int width  = get_next_int(fp);
    int height = get_next_int(fp);
    if ((width <= 0) || (height <= 0)) {
        fclose(fp);
        return NULL;
    }

    // max value
    int max;
    if ((n_magic == 1) || (n_magic == 4)) {
        max = 1;
    } else {
        max = get_next_int(fp);
        if ((max < 1) || (max > UINT8_MAX)) {
            fclose(fp);
            return NULL;
        }
    }

    cimg_t *img = cimg_create(width, height, channels);

    // ASCII format
    if (n_magic <= 3) {
        if (!read_pnm_ascii(fp, img, max, n_magic)) {
            img = NULL;
        }
    // binary format
    } else {
        if (!read_pnm_binary(fp, img, max, n_magic)) {
            img = NULL;
        }
    }

    fclose(fp);

    return img;
}

///
/// @fn     write_pnm_ascii
/// @brief  write ASCII format PNM image file
/// @param  fp      [in]    output file stream
/// @param  img     [out]   image structure
/// @param  n_magic [in]    magic number of PNM image (1/2/3)
/// @retval true    succeed to write file
/// @retval false   fail to write file
/// @note   not support 16bit image
///
static bool write_pnm_ascii(FILE *fp, cimg_t *img, uint8_t n_magic)
{
    // grayscale
    if ((n_magic == 1) || (n_magic == 2)) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                fprintf(fp, "%u ", img->data[y * img->stride + x]);
            }
            fseek(fp, -1, SEEK_CUR);
            fputc('\n', fp);
        }
    // color
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                uint8_t *pixel = &img->data[y * img->stride + x * img->channels];
                fprintf(fp, "%u %u %u ", *pixel, *(pixel + 1), *(pixel + 2));
            }
            fseek(fp, -1, SEEK_CUR);
            fputc('\n', fp);
        }
    }

    return true;
}

///
/// @fn     write_pnm_binary
/// @brief  write ASCII format PNM image file
/// @param  fp      [in]    output file stream
/// @param  img     [out]   image structure
/// @param  n_magic [in]    magic number of PNM image (4/5/6)
/// @retval true    succeed to write file
/// @retval false   fail to write file
/// @note   not support 16bit image
///
static bool write_pnm_binary(FILE *fp, cimg_t *img, uint8_t n_magic)
{
    // binary
    if (n_magic == 4) {
        int     shift;
        uint8_t packed_val;

        for (int y = 0; y < img->height; y++) {
            shift      = 8;
            packed_val = 0;

            // pack 8 pixels to 8bit value
            for (int x = 0; x < img->width; x++) {
                shift--;
                packed_val |= (img->data[y * img->stride + x] << shift);

                if (shift == 0) {
                    putc(packed_val, fp);
                    shift      = 8;
                    packed_val = 0;
                }
            }

            // write remaining pixels of right edge
            if (shift != 8) {
                putc(packed_val, fp);
            }
        }
    // grayscale
    } else if (n_magic == 5) {
        for (int i = 0; i < (img->width * img->height); i++) {
            putc(img->data[i], fp);
        }
    // color
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                uint8_t *pixel = &img->data[y * img->stride + x];
                putc(*pixel,       fp); // red
                putc(*(pixel + 1), fp); // green
                putc(*(pixel + 2), fp); // blue
            }
        }
    }

    return true;
}

bool write_pnm(cimg_t *img, const char *dst, PNM_FORMAT format)
{
    if (img == NULL) {
        return false;
    }

    // get magic number P2/3/5/6 from img format + colorspace
    // grayscale or RGB
    uint8_t n_magic;
    if (format == PNM_FORMAT_ASCII) {
        switch (img->channels) {
            case CH_GRAY: n_magic = 2;  break;
            case CH_RGB:  n_magic = 3;  break;
            default:      return false; break;
        }
    } else { // PNM_FORMAT_BINARY
        switch (img->channels) {
            case CH_GRAY: n_magic = 5;  break;
            case CH_RGB:  n_magic = 6;  break;
            default:      return false; break;
        }
    }

    FILE *fp = fopen(dst, "wb");
    if (fp == NULL) {
        return false;
    }

    // write file header: magic number, width, height, max value (255)
    fprintf(fp, "P%u\n", n_magic);
    fprintf(fp, "%d %d\n", img->width, img->height);
    if ((n_magic != 1) && (n_magic != 4)) {
        fprintf(fp, "%u\n", UINT8_MAX);
    }

    // ASCII format
    if (n_magic <= 3) {
        if (!write_pnm_ascii(fp, img, n_magic)) {
            fclose(fp);
            return false;
        }
    // binary format
    } else {
        if (!write_pnm_binary(fp, img, n_magic)) {
            fclose(fp);
            return false;
        }
    }

    fclose(fp);

    return true;
}
