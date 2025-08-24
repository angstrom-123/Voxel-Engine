#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <malloc.h>
#include <memory.h> // memset
#include <stdlib.h> // exit
#include <math.h>
#include <assert.h>

#include <libem/em_perlin.h>

#include "geometry.h"

typedef struct ivec2_tuple {
    ivec2 **arrays;
    size_t *sizes;
} ivec2_tuple_t;

extern ivec2_tuple_t gen_get_coords(vec3 centre, uint8_t rndr_dist);
extern chunk_t *gen_new_chunk(int32_t x, int32_t z);

#endif
