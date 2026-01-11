#include "texture_handler.h"

static const size_t BUF_LEN = 128;
static const size_t EXT_LEN = 4;

bool texture_load(texture_t *res, const texture_desc_t *desc)
{
    ENGINE_ASSERT(strlen(desc->path) + EXT_LEN <= BUF_LEN,
                  "Texture path length exceeds buffer length");

    char buf[BUF_LEN];
    sprintf(buf, "%s.bmp", desc->path);

    em_bmp_image_t img;
    if (!em_bmp_load(&img, buf)) return false;

    sg_image i = sg_make_image(&(sg_image_desc) {
        .width = img.ih.width,
        .height = img.ih.height,
        .data.subimage[0] = (sg_range) { 
            .ptr = img.pixel_data, 
            .size = img.ih.img_size
        },
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .num_mipmaps = 1.0
    });

    *res = (texture_t) {
        .width = img.ih.width,
        .height = img.ih.height,
        .subimages_x = desc->subimages_x == 0 ? 1 : desc->subimages_x,
        .subimages_y = desc->subimages_y == 0 ? 1 : desc->subimages_y,
        .size = img.ih.img_size,
        .as_image = i,
        .as_view = sg_make_view(&(sg_view_desc) { .texture.image = i } )
    };

    free(img.pixel_data);

    return true;
}

bool texture_mip_load(texture_t *res, const texture_desc_t *desc)
{
    ENGINE_ASSERT(strlen(desc->path) + 3 + EXT_LEN <= BUF_LEN, 
                  "Texture path length exceeds buffer length");
    ENGINE_ASSERT(desc->mip_levels <= SG_MAX_MIPMAPS,
                  "Mipmap amount exceeds maximum");

    em_bmp_image_t mips[desc->mip_levels];
    for (uint8_t i = 0; i < desc->mip_levels; i++)
    {
        char buf[BUF_LEN];
        snprintf(buf, BUF_LEN, "%s%03hhu.bmp", desc->path, i);
        if (!em_bmp_load(&mips[i], buf)) return false;
    }

    sg_image_desc img_desc = {
        .width = mips[0].ih.width,
        .height = mips[0].ih.height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .num_mipmaps = desc->mip_levels
    };
    for (size_t i = 0; i < desc->mip_levels; i++)
    {
        em_bmp_image_t m = mips[i];
        img_desc.data.subimage[0][i] = (sg_range) { 
            .ptr = m.pixel_data,
            .size = m.ih.img_size
        };
    }

    sg_image img = sg_make_image(&img_desc);

    *res = (texture_t) {
        .width = mips[0].ih.width,
        .height = mips[0].ih.height,
        .subimages_x = desc->subimages_x == 0 ? 1 : desc->subimages_x,
        .subimages_y = desc->subimages_y == 0 ? 1 : desc->subimages_y,
        .size = mips[0].ih.img_size * desc->mip_levels,
        .as_image = img,
        .as_view = sg_make_view(&(sg_view_desc) { .texture.image = img } )
    };

    for (size_t i = 0; i < desc->mip_levels; i++)
        free(mips[i].pixel_data);

    return true;
}

vec2 texture_query_subimage_uv(const texture_t *tex)
{
    return VEC2(1.0 / (float) tex->subimages_x, 
                1.0 / (float) tex->subimages_y);
}
