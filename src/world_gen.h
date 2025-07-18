#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <memory.h>
#include <limits.h>

#include "geometry.h"
#include "extra_math.h"
#include "perlin.h"

extern ivec3 *gen_get_required_coords(vec3 centre, uint8_t rndr_dist, size_t *cnt);
extern chunk_t *gen_new_chunk(int32_t x, int32_t y, int32_t z);

#endif
