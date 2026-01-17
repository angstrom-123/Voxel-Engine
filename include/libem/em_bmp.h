#ifndef EM_BMP_H
#define EM_BMP_H

#ifndef EM_BMP_INCLUDED
#define EM_BMP_INCLUDED 

#include "em_global.h"

#include <stdbool.h>
#include <malloc.h>

typedef struct em_bmp_file_header { // 14 bytes
    USHORT sig;      // "BM" for a bitmap file
    UINT file_size;  // file size in bytes
    UINT __reserved; // unused = 0
    UINT data_ofst;  // offset from file start to data start
} em_bmp_file_header_t;

typedef struct em_bmp_info_header { // 40 bytes
    UINT head_size; // size of info header in bytes = 40
    UINT width;     // horizontal width of bmp in pixels
    UINT height;    // vertical height of bmp in pixels
    USHORT planes;  // number of planes = 1
    USHORT bpp;     // bits per pixel
    UINT compress;  // 0 = no compression, 1 = 8 bit RLE, 2 = 4 bit RLE
    UINT img_size;  // compressed size of image in bytes ( = 0 if no compression)
    UINT x_ppm;     // horizontal resolution in pixels per metre
    UINT y_ppm;     // vertical resolution in pixels per metre
    UINT cols_used; // number of actually used colours
    UINT imp_cols;  // number of important colours ( = 0 if all are important)
} em_bmp_info_header_t;

/*
 * TODO: Add support for various BPP formats (and color table to support them)
 * 
 * NOTE: This is not currently used, the struct is a placeholder.
 * Only used if the info header specifies less than 8 bits per pixel.
 * Colors should be ordered by importance.
 */
typedef struct em_bmp_color_table { // variable size as per cols_used in info
    struct {
        UBYTE r;      // red intensity
        UBYTE g;      // green intensity
        UBYTE b;      // blue intensity
    } *rgb;
    UBYTE __reserved; // unused = 0
} em_bmp_color_table_t;

typedef struct em_bmp_image {
    em_bmp_file_header_t fh;
    em_bmp_info_header_t ih;
    em_bmp_color_table_t ct; 
    UBYTE *pixel_data;       // NOTE: Each row is padded to be a multiple of 4 bytes
} em_bmp_image_t;

extern bool em_bmp_load(em_bmp_image_t *img, const char *path);

#endif // EM_BMP_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_BMP_IMPL 

#include <netinet/in.h> // htons
#include <stdio.h> // FILE
#ifndef EM_BINARY_INCLUDED
#error "Please include em_binary.h before em_bmp.h"
#endif

static bool _load_file_header(FILE *f_ptr, em_bmp_file_header_t *fh)
{
    SIZE buf_len = 14;
    UBYTE buf[buf_len];
    if (!binary_read(f_ptr, 0, &buf_len, buf)) 
        return false;

    SIZE head = 0;
    fh->sig        = READ_U16_MOVE(buf, head);
    fh->sig        = htons(fh->sig);           // Signature is big endian so we use htons.
    fh->file_size  = READ_U32_MOVE(buf, head);
    fh->__reserved = READ_U32_MOVE(buf, head);
    fh->data_ofst  = READ_U32_MOVE(buf, head);
    
    return true;
}

static bool _load_info_header(FILE *f_ptr, em_bmp_info_header_t *ih)
{
    SIZE buf_len = 40;
    UBYTE buf[40];
    if (!binary_read(f_ptr, 14, &buf_len, buf)) 
        return false;

    SIZE head = 0;
    ih->head_size = READ_U32_MOVE(buf, head);
    ih->width     = READ_U32_MOVE(buf, head);
    ih->height    = READ_U32_MOVE(buf, head);
    ih->planes       = READ_U16_MOVE(buf, head);
    ih->bpp       = READ_U16_MOVE(buf, head);
    ih->compress  = READ_U32_MOVE(buf, head);
    ih->img_size  = READ_U32_MOVE(buf, head);
    ih->x_ppm     = READ_U32_MOVE(buf, head);
    ih->y_ppm     = READ_U32_MOVE(buf, head);
    ih->cols_used = READ_U32_MOVE(buf, head);
    ih->imp_cols  = READ_U32_MOVE(buf, head);

    return true;
}

static UBYTE *_load_pixel_data(FILE *f_ptr, em_bmp_file_header_t *fh, em_bmp_info_header_t *ih)
{
    UBYTE *pixels = malloc(ih->img_size);
    if (!pixels) 
    {
        free(pixels);
        return NULL;
    }

    SIZE offset = fh->data_ofst;
    SIZE len = ih->img_size;
    if (!binary_read(f_ptr, offset, &len, pixels)) 
        return NULL;

    /* Rearrange from BGRA to RGBA */
    UBYTE tmp;
    for (SIZE i = 0; i < len; i += 4)
    {
        tmp = pixels[i];
        pixels[i] = pixels[i + 2];
        pixels[i + 2] = tmp;
    }

    return pixels;
}

bool em_bmp_load(em_bmp_image_t *img, const char *path)
{
    FILE *f_ptr = fopen(path, "rb");
    if (!f_ptr)
        return false;

    em_bmp_file_header_t fh;
    if (!_load_file_header(f_ptr, &fh))
    {
        fclose(f_ptr);
        return false;
    }

    em_bmp_info_header_t ih;
    if (!_load_info_header(f_ptr, &ih))
    {
        fclose(f_ptr);
        return false;
    }

    // TODO: add support for color table (for when there are less than 8 BPP)

    UBYTE *pixels = _load_pixel_data(f_ptr, &fh, &ih);
    if (!pixels) 
    {
        fclose(f_ptr);
        free(pixels);
        return false;
    }

    fclose(f_ptr);
        
    *img = (em_bmp_image_t) {
        .fh = fh,
        .ih = ih,
        .ct = (em_bmp_color_table_t) {0},
        .pixel_data = pixels
    };
    return true;
}
#endif // EM_BMP_IMPL

#endif
