#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <malloc.h>
#include <memory.h> // memset
#include <stdlib.h> // exit
#include <math.h>

#include <libem/em_perlin.h>

#include "geometry.h"

extern ivec2 *gen_get_required_coords_2(vec3 centre, uint8_t rndr_dist, size_t *coords_len);
extern ivec3 *gen_get_required_coords(vec3 centre, uint8_t rndr_dist, size_t *coords_len);
extern chunk_t *gen_new_chunk(int32_t x, int32_t z);

#endif
