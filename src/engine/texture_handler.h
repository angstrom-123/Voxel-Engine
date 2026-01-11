#ifndef TEXTURE_HANDLER_H
#define TEXTURE_HANDLER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <libem/em_math.h>

#include "include_sokol.h"
#include "em_bmp.h"
#include "logger.h"

/* Pretty much just wraps an em_bmp_image but with only required fields + extras. */
typedef struct texture {
    uint32_t width;
    uint32_t height;
    uint8_t subimages_x;
    uint8_t subimages_y;
    size_t size;
    sg_image as_image;
    sg_view as_view;
} texture_t;

/* If subimages are left blank then they default to 1. */
typedef struct texture_desc {
    const char *path;
    uint8_t subimages_x;
    uint8_t subimages_y;
    uint8_t mip_levels;
} texture_desc_t;

/*
 * Texture path should not include file extension as it is implicitly .bmp.
 */
extern bool texture_load(texture_t *res, const texture_desc_t *desc);
/* 
 * Texture path should be the base path. Numbers like 000, 001, ... are added
 * directly after the supplied path. .bmp file extension is automatically added.
 */
extern bool texture_mip_load(texture_t *res, const texture_desc_t *desc);
extern vec2 texture_query_subimage_uv(const texture_t *tex);

#endif
