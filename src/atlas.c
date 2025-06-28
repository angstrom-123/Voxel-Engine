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

void atlas_set_texture(cube_t *cube, atlas_texture_e tex)
{
	printf("%hi\n", MUL_X);
	atlas_offset_t off = _get_tex_offset(tex);
	for (size_t i = 0; i < 24; i += 4)
	{
		cube->vertices[i].u = off.u;
		cube->vertices[i].v = off.v;

		cube->vertices[i + 1].u = off.u + (TEX_SIZE * MUL_X);
		cube->vertices[i + 1].v = off.v;

		cube->vertices[i + 2].u = off.u + (TEX_SIZE * MUL_X);
		cube->vertices[i + 2].v = off.v + (TEX_SIZE * MUL_Y);

		cube->vertices[i + 3].u = off.u;
		cube->vertices[i + 3].v = off.v + (TEX_SIZE * MUL_Y);
	}
}
