#include "bmp.h"

static bool _read_bytes_to_buf(FILE *file_ptr, size_t start_offset, size_t length, u8 *buf)
{
	fseek(file_ptr, start_offset, SEEK_SET);

	for (size_t i = 0; i < length; i++)
	{
		int read = fgetc(file_ptr);
		if (read == EOF)
			return false;

		buf[i] = (u8) read;
	}

	return true;
}

static bool _load_file_header(FILE *file_ptr, bmp_file_header_t *fh)
{
	u8 buf[14];
	if (!_read_bytes_to_buf(file_ptr, 0, 14, buf)) return false;

	/* header's signature is big endian because it encodes hex, rest is little endian */
	fh->signature   = TO_U16_BE(buf[0], buf[1]);
	fh->file_size   = TO_U32_LE(buf[2], buf[3], buf[4], buf[5]);
	/* bytes 6-9 are reserved and currently unused */
	fh->data_offset = TO_U32_LE(buf[10], buf[11], buf[12], buf[13]);
	
	return true;
}

static bool _load_info_header(FILE *file_ptr, bmp_info_header_t *ih)
{
	u8 buf[40];
	if (!_read_bytes_to_buf(file_ptr, 14, 40, buf)) return false;

	ih->header_size 	 = TO_U32_LE(buf[0], buf[1], buf[2], buf[3]);
	ih->width 			 = TO_U32_LE(buf[4], buf[5], buf[6], buf[7]);
	ih->height 			 = TO_U32_LE(buf[8], buf[9], buf[10], buf[11]);
	ih->planes 			 = TO_U16_LE(buf[12], buf[13]);
	ih->bits_per_pixel   = TO_U16_LE(buf[14], buf[15]);
	ih->compression_type = TO_U32_LE(buf[16], buf[17], buf[18], buf[19]);
	ih->img_size 		 = TO_U32_LE(buf[20], buf[21], buf[22], buf[23]);
	ih->x_pix_per_m 	 = TO_U32_LE(buf[24], buf[25], buf[26], buf[27]);
	ih->y_pix_per_m 	 = TO_U32_LE(buf[28], buf[29], buf[30], buf[31]);
	ih->cols_used 		 = TO_U32_LE(buf[32], buf[33], buf[34], buf[35]);
	ih->important_cols   = TO_U32_LE(buf[36], buf[37], buf[38], buf[39]);

	return true;
}

static u8 *_load_pixel_data(FILE *file_ptr, bmp_file_header_t *fh, bmp_info_header_t *ih)
{
	u8 *pix = malloc(ih->img_size);
	if (!pix) 
	{
		free(pix);
		return NULL;
	}

	size_t offset = fh->data_offset;
	size_t length = ih->img_size;
	if (!_read_bytes_to_buf(file_ptr, offset, length, pix)) return NULL;

	// rearrange from BGRA to RGBA
	u8 tmp;
	for (size_t i = 0; i < length; i += 4)
	{
		tmp = pix[i];
		pix[i] = pix[i + 2];
		pix[i + 2] = tmp;
	}

	return pix;
}

bmp_image_t *bmp_load_file(char *file_path)
{
	bmp_image_t *out = malloc(sizeof(bmp_image_t));
	if (!out)
	{
		free(out);
		return NULL;
	}

	FILE *file_ptr = fopen(file_path, "rb");
	if (!file_ptr)
	{
		fclose(file_ptr);
		return NULL;
	}

	bmp_file_header_t file_header;
	if (!_load_file_header(file_ptr, &file_header))
	{
		fclose(file_ptr);
		return NULL;
	}

	bmp_info_header_t info_header;
	if (!_load_info_header(file_ptr, &info_header))
	{
		fclose(file_ptr);
		return NULL;
	}

	// TODO: add support for color table (for when there are less than 8 BPP)

	u8 *pixels = _load_pixel_data(file_ptr, &file_header, &info_header);
	if (!pixels) 
	{
		fclose(file_ptr);
		free(pixels);
		return NULL;
	}

	out->file_header = file_header;
	out->info_header = info_header;
	out->color_table = (bmp_color_table_t) {0}; // NOTE: empty until support added
	out->pixel_data = pixels;

	return out;
}
