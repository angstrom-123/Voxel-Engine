#ifndef ATLAS_H
#define ATLAS_H

#include "mesh.h"
#include <stdint.h>

#define TEX_SIZE 16
#define TEX_COUNT 9

#define TEX_X 256
#define TEX_Y 256

#define UV_SCALE 32767

typedef enum atlas_texture {
	TEX_INVALID    = 0,
	TEX_GRASS_TOP  = 1,
	TEX_GRASS_SIDE = 2,
	TEX_DIRT 	   = 3,
	TEX_STONE 	   = 4,
	TEX_SAND 	   = 5,
	TEX_LOG_TOP    = 6,
	TEX_LOG_SIDE   = 7,
	TEX_LEAVES 	   = 8
} atlas_texture_e;

typedef struct atlas_offset {
	int16_t u;
	int16_t v;
} atlas_offset_t;

extern void atlas_set_bottom(cube_t *cube, atlas_texture_e texture);
extern void atlas_set_top(cube_t *cube, atlas_texture_e texture);
extern void atlas_set_texture(cube_t *cube, atlas_texture_e texture);

#endif
