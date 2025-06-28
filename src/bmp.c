#include "bmp.h"

static bool _read_bytes_to_buf(FILE *f_ptr, size_t offset, size_t len, u8 *buf)
{
	fseek(f_ptr, offset, SEEK_SET);

	for (size_t i = 0; i < len; i++)
	{
		int read = fgetc(f_ptr);
		if (read == EOF)
			return false;

		buf[i] = (u8) read;
	}

	return true;
}

static bool _load_file_header(FILE *f_ptr, bmp_file_header_t *fh)
{
	u8 buf[14];
	if (!_read_bytes_to_buf(f_ptr, 0, 14, buf)) return false;

	size_t head = 0;
	fh->signature 	= READ_U16_MOVE(buf, head);
	fh->signature 	= htons(fh->signature); // signature is big endian, so we use htons
	fh->file_size 	= READ_U32_MOVE(buf, head);
	fh->reserved 	= READ_U32_MOVE(buf, head);
	fh->data_offset = READ_U32_MOVE(buf, head);
	
	return true;
}

static bool _load_info_header(FILE *f_ptr, bmp_info_header_t *ih)
{
	u8 buf[40];
	if (!_read_bytes_to_buf(f_ptr, 14, 40, buf)) return false;

	size_t head = 0;
	ih->header_size 	 = READ_U32_MOVE(buf, head);
	ih->width 			 = READ_U32_MOVE(buf, head);
	ih->height 			 = READ_U32_MOVE(buf, head);
	ih->planes 			 = READ_U16_MOVE(buf, head);
	ih->bits_per_pixel 	 = READ_U16_MOVE(buf, head);
	ih->compression_type = READ_U32_MOVE(buf, head);
	ih->img_size 	 	 = READ_U32_MOVE(buf, head);
	ih->x_pix_per_m    	 = READ_U32_MOVE(buf, head);
	ih->y_pix_per_m    	 = READ_U32_MOVE(buf, head);
	ih->cols_used 	   	 = READ_U32_MOVE(buf, head);
	ih->important_cols 	 = READ_U32_MOVE(buf, head);

	return true;
}

static u8 *_load_pixel_data(FILE *f_ptr, bmp_file_header_t *fh, bmp_info_header_t *ih)
{
	u8 *pixels = malloc(ih->img_size);
	if (!pixels) 
	{
		free(pixels);
		return NULL;
	}

	size_t offset = fh->data_offset;
	size_t len = ih->img_size;
	if (!_read_bytes_to_buf(f_ptr, offset, len, pixels)) return NULL;

	// rearrange from BGRA to RGBA
	u8 tmp;
	for (size_t i = 0; i < len; i += 4)
	{
		tmp = pixels[i];
		pixels[i] = pixels[i + 2];
		pixels[i + 2] = tmp;
	}

	return pixels;
}

bmp_image_t *bmp_load_file(char *path)
{
	bmp_image_t *out = malloc(sizeof(bmp_image_t));
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

	bmp_file_header_t fh;
	if (!_load_file_header(f_ptr, &fh))
	{
		fclose(f_ptr);
		return NULL;
	}

	bmp_info_header_t ih;
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

	out->file_header = fh;
	out->info_header = ih;
	out->color_table = (bmp_color_table_t) {0}; // NOTE: empty until support added
	out->pixel_data = pixels;

	return out;
}
