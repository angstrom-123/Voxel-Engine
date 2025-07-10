#ifndef BMP_H
#define BMP_H

#include <netinet/in.h> // htons
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdio.h> // FILE

/* macros assume LITTLE ENDIAN (least significant byte at lowest memory address) */
#define TO_U16(b1, b2) ((u16) ((b1) | (b2 << 8)))
#define TO_U32(b1, b2, b3, b4) ((u32) ((b1) | (b2 << 8) | (b3 << 16) | (b4 << 24)))

#define READ_U16_MOVE(mem, i) (TO_U16(mem[i], mem[i + 1])); (i += 2)
#define READ_U32_MOVE(mem, i) (TO_U32(mem[i], mem[i + 1], mem[i + 2], mem[i + 3])); (i += 4)

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct bmp_file_header { // 14 bytes
	u16 sig;	 	// BM for a bitmap file
	u32 file_size;	// file size in bytes
	u32 __reserved;	// unused = 0
	u32 data_ofst;  // offset from file start to data start
} bmp_file_header_t;

typedef struct bmp_info_header { // 40 bytes
	u32 head_size; // size of info header in bytes = 40
	u32 width;	   // horizontal width of bmp in pixels
	u32 height;	   // vertical height of bmp in pixels
	u16 planes;	   // number of planes = 1
	u16 bpp;	   // bits per pixel
	u32 compress;  // 0 = no compression, 1 = 8 bit RLE, 2 = 4 bit RLE
	u32 img_size;  // compressed size of image in bytes ( = 0 if no compression)
	u32 x_ppm;	   // horizontal resolution in pixels per metre
	u32 y_ppm;	   // vertical resolution in pixels per metre
	u32 cols_used; // number of actually used colours
	u32 imp_cols;  // number of important colours ( = 0 if all are important)
} bmp_info_header_t;

/*
 * TODO: Add support for various BPP formats (and color table to support them)
 * 
 * NOTE: This is not currently used, the struct is a placeholder.
 * Only used if the info header specifies less than 8 bits per pixel.
 * Colors should be ordered by importance.
 */
typedef struct bmp_color_table { // variable size as per cols_used in info
	struct {
		u8 r; 	   // red intensity
		u8 g; 	   // green intensity
		u8 b; 	   // blue intensity
	} *rgb;
	u8 __reserved; // unused = 0
} bmp_color_table_t;

typedef struct bmp_image {
	bmp_file_header_t fh;
	bmp_info_header_t ih;
	bmp_color_table_t ct; 
	u8 *pixel_data; // NOTE: Each row is padded to be a multiple of 4 bytes
} bmp_image_t;

extern bmp_image_t *bmp_load_file(char *file_path);

#endif
