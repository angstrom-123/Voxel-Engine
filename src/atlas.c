#include "atlas.h"
#include <stdio.h>

/* 
 * UV_SCALE is equal to the maximum value of the data type used, in this case 
 * it is int16_t. To get the multiplier, I add 1 to this scale (to get the amount 
 * of numbers that can be represented) and then divide by the resolution of the 
 * texture atlas.
 *
 * NOTE: MUL_Y is defined here in case of a non-square atlas 
 */
#define MUL_X (UV_SCALE + 1) / TEX_X
#define MUL_Y (UV_SCALE + 1) / TEX_Y 

#define SET_UV(cube, idx, uv_u, uv_v) cube->vertices[idx].u = uv_u; cube->vertices[idx].v = uv_v

static atlas_offset_t _get_tex_offset(atlas_texture_e tex)
{
	switch (tex) {
	case TEX_GRASS_TOP: 
		return (atlas_offset_t) {
			.u = 0, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_GRASS_SIDE: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_DIRT: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X * 2, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_STONE: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X * 3, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_SAND: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X * 4, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_LOG_TOP: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X * 5, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_LOG_SIDE: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X * 6, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	case TEX_LEAVES: 
		return (atlas_offset_t) {
			.u = TEX_SIZE * MUL_X * 7, 
			.v = UV_SCALE - (TEX_SIZE * MUL_Y)
		};
	default:
		return (atlas_offset_t) {
			.u = 0, 
			.v = 0
		};
	};
}

void atlas_set_bottom(cube_t *cube, atlas_texture_e tex)
{
	atlas_offset_t off = _get_tex_offset(tex);
	int16_t max_u = off.u + (TEX_SIZE * MUL_X);
	int16_t max_v = off.v + (TEX_SIZE * MUL_Y);

	SET_UV(cube, 16, max_u, off.v);
	SET_UV(cube, 17, off.u, off.v);
	SET_UV(cube, 18, off.u, max_v);
	SET_UV(cube, 19, max_u, max_v);
}

void atlas_set_top(cube_t *cube, atlas_texture_e tex)
{
	atlas_offset_t off = _get_tex_offset(tex);
	int16_t max_u = off.u + (TEX_SIZE * MUL_X);
	int16_t max_v = off.v + (TEX_SIZE * MUL_Y);

	SET_UV(cube, 20, max_u, off.v);
	SET_UV(cube, 21, off.u, off.v);
	SET_UV(cube, 22, off.u, max_v);
	SET_UV(cube, 23, max_u, max_v);
}

void atlas_set_texture(cube_t *cube, atlas_texture_e tex)
{
	atlas_offset_t off = _get_tex_offset(tex);
	int16_t max_u = off.u + (TEX_SIZE * MUL_X);
	int16_t max_v = off.v + (TEX_SIZE * MUL_Y);

	/* back */
	SET_UV(cube, 0, max_u, off.v);
	SET_UV(cube, 1, off.u, off.v);
	SET_UV(cube, 2, off.u, max_v);
	SET_UV(cube, 3, max_u, max_v);
	/* front */
	SET_UV(cube, 4, off.u, off.v);
	SET_UV(cube, 5, max_u, off.v);
	SET_UV(cube, 6, max_u, max_v);
	SET_UV(cube, 7, off.u, max_v);
	/* right */
	SET_UV(cube, 8, off.u, off.v);
	SET_UV(cube, 9, off.u, max_v);
	SET_UV(cube, 10, max_u, max_v);
	SET_UV(cube, 11, max_u, off.v);
	/* left */
	SET_UV(cube, 12, max_u, off.v);
	SET_UV(cube, 13, max_u, max_v);
	SET_UV(cube, 14, off.u, max_v);
	SET_UV(cube, 15, off.u, off.v);
	/* bottom */
	SET_UV(cube, 16, max_u, off.v);
	SET_UV(cube, 17, off.u, off.v);
	SET_UV(cube, 18, off.u, max_v);
	SET_UV(cube, 19, max_u, max_v);
	/* top */
	SET_UV(cube, 20, max_u, off.v);
	SET_UV(cube, 21, off.u, off.v);
	SET_UV(cube, 22, off.u, max_v);
	SET_UV(cube, 23, max_u, max_v);
}
