#ifndef EM_BMP_H
#define EM_BMP_H

#ifndef EM_BMP_INCLUDED
#define EM_BMP_INCLUDED 

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct em_bmp_file_header { // 14 bytes
    u16 sig;        // "BM" for a bitmap file
    u32 file_size;  // file size in bytes
    u32 __reserved; // unused = 0
    u32 data_ofst;  // offset from file start to data start
} em_bmp_file_header_t;

typedef struct em_bmp_info_header { // 40 bytes
    u32 head_size; // size of info header in bytes = 40
    u32 width;     // horizontal width of bmp in pixels
    u32 height;    // vertical height of bmp in pixels
    u16 planes;    // number of planes = 1
    u16 bpp;       // bits per pixel
    u32 compress;  // 0 = no compression, 1 = 8 bit RLE, 2 = 4 bit RLE
    u32 img_size;  // compressed size of image in bytes ( = 0 if no compression)
    u32 x_ppm;     // horizontal resolution in pixels per metre
    u32 y_ppm;     // vertical resolution in pixels per metre
    u32 cols_used; // number of actually used colours
    u32 imp_cols;  // number of important colours ( = 0 if all are important)
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
        u8 r;      // red intensity
        u8 g;      // green intensity
        u8 b;      // blue intensity
    } *rgb;
    u8 __reserved; // unused = 0
} em_bmp_color_table_t;

typedef struct em_bmp_image {
    em_bmp_file_header_t fh;
    em_bmp_info_header_t ih;
    em_bmp_color_table_t ct; 
    u8 *pixel_data;       // NOTE: Each row is padded to be a multiple of 4 bytes
} em_bmp_image_t;

extern em_bmp_image_t *em_bmp_load(char *path);

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

/* macros assume LITTLE ENDIAN (least significant byte at lowest memory address). */
#define TO_U16(b1, b2) ((u16) ((b1) | (b2 << 8)))
#define TO_U32(b1, b2, b3, b4) ((u32) ((b1) | (b2 << 8) | (b3 << 16) | (b4 << 24)))

#define READ_U16_MOVE(mem, i) (TO_U16(mem[i], mem[i + 1])); (i += 2)
#define READ_U32_MOVE(mem, i) (TO_U32(mem[i], mem[i + 1], mem[i + 2], mem[i + 3])); (i += 4)

static bool _read_bytes_to_buf(FILE *f_ptr, size_t offset, size_t len, u8 *buf)
{
    fseek(f_ptr, offset, SEEK_SET);

    for (size_t i = 0; i < len; i++)
    {
        int read = fgetc(f_ptr);
        if (read == EOF) 
            return false; // Should not overrun file end.

        buf[i] = (u8) read;
    }

    return true;
}

static bool _load_file_header(FILE *f_ptr, em_bmp_file_header_t *fh)
{
    u8 buf[14];
    if (!_read_bytes_to_buf(f_ptr, 0, 14, buf)) 
        return false;

    size_t head = 0;
    fh->sig        = READ_U16_MOVE(buf, head);
    fh->sig        = htons(fh->sig);           // Signature is big endian so we use htons.
    fh->file_size  = READ_U32_MOVE(buf, head);
    fh->__reserved = READ_U32_MOVE(buf, head);
    fh->data_ofst  = READ_U32_MOVE(buf, head);
    
    return true;
}

static bool _load_info_header(FILE *f_ptr, em_bmp_info_header_t *ih)
{
    u8 buf[40];
    if (!_read_bytes_to_buf(f_ptr, 14, 40, buf)) 
        return false;

    size_t head = 0;
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

static u8 *_load_pixel_data(FILE *f_ptr, em_bmp_file_header_t *fh, em_bmp_info_header_t *ih)
{
    u8 *pixels = malloc(ih->img_size);
    if (!pixels) 
    {
        free(pixels);
        return NULL;
    }

    size_t offset = fh->data_ofst;
    size_t len = ih->img_size;
    if (!_read_bytes_to_buf(f_ptr, offset, len, pixels)) 
        return NULL;

    /* Rearrange from BGRA to RGBA */
    u8 tmp;
    for (size_t i = 0; i < len; i += 4)
    {
        tmp = pixels[i];
        pixels[i] = pixels[i + 2];
        pixels[i + 2] = tmp;
    }

    return pixels;
}

em_bmp_image_t *em_bmp_load(char *path)
{
    em_bmp_image_t *out = malloc(sizeof(em_bmp_image_t));
    if (!out)
    {
        free(out);
        return NULL;
    }

    FILE *f_ptr = fopen(path, "rb");
    if (!f_ptr)
    {
        fclose(f_ptr);
        return NULL;
    }

    em_bmp_file_header_t fh;
    if (!_load_file_header(f_ptr, &fh))
    {
        fclose(f_ptr);
        return NULL;
    }

    em_bmp_info_header_t ih;
    if (!_load_info_header(f_ptr, &ih))
    {
        fclose(f_ptr);
        return NULL;
    }

    // TODO: add support for color table (for when there are less than 8 BPP)

    u8 *pixels = _load_pixel_data(f_ptr, &fh, &ih);
    if (!pixels) 
    {
        fclose(f_ptr);
        free(pixels);
        return NULL;
    }

    out->fh = fh;
    out->ih = ih;
    out->ct = (em_bmp_color_table_t) {0}; // NOTE: empty until support added.
    out->pixel_data = pixels;

    fclose(f_ptr);

    return out;
}
#endif // EM_BMP_IMPL

#endif
