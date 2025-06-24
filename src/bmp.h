#ifndef BMP_H
#define BMP_H

#include <netinet/in.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * For converting consecutive bytes into various sized unsigned integers.
 * BE suffix = Big Endian
 * LE suffix = Little Endian
 */
#define TO_U16_LE(b1, b2) ((u16) ((b1) | (b2 << 8)))
#define TO_U32_LE(b1, b2, b3, b4) ((u32) ((b1) | (b2 << 8) | (b3 << 16) | (b4 << 24)))
#define TO_U16_BE(b1, b2) htons(TO_U16_LE(b1, b2))

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct bmp_file_header { // 14 bytes
	u16 signature;	 // BM for a bitmap file
	u32 file_size;	 // file size in bytes
	u32 reserved;	 // unused = 0
	u32 data_offset; // offset from file start to data start
} bmp_file_header_t;

typedef struct bmp_info_header { // 40 bytes
	u32 header_size;	  // size of info header in bytes = 40
	u32 width;			  // horizontal width of bmp in pixels
	u32 height;			  // vertical height of bmp in pixels
	u16 planes;			  // number of planes = 1
	u16 bits_per_pixel;	  // bits per pixel
	u32 compression_type; // 0 = no compression, 1 = 8 bit RLE, 2 = 4 bit RLE
	u32 img_size;		  // compressed size of image in bytes ( = 0 if no compression)
	u32 x_pix_per_m;	  // horizontal resolution in pixels per metre
	u32 y_pix_per_m;	  // vertical resolution in pixels per metre
	u32 cols_used;		  // number of actually used colours
	u32 important_cols;	  // number of important colours ( = 0 if all are important)
} bmp_info_header_t;

/*
 * Only used if the info header specifies less than 8 bits per pixel.
 * Colours should be ordered by importance.
 */
typedef struct bmp_color_table {
	u8 red;		 // red intensity
	u8 green;	 // green intensity
	u8 blue;	 // blue intensity
	u8 reserved; // unused = 0
} bmp_color_table_t;

typedef struct bmp_image {
	bmp_file_header_t file_header;
	bmp_info_header_t info_header;
	bmp_color_table_t color_table; 
	u8 *pixel_data; /* bytes per row = ceil((BPP * width) / 32) * 4  */
} bmp_image_t;

typedef struct bmp_sub_image {
	size_t width;
	size_t height;
	u8 *pixel_data;
} bmp_sub_image_t;

bmp_image_t *bmp_load_file(char *file_path);

#endif
