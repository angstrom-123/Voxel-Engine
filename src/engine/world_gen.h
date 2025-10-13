#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <malloc.h>
#include <memory.h> // memset
#include <stdlib.h> // exit

#include <libem/em_perlin.h>
#include <libem/em_math.h>

#include "geometry_types.h"

typedef struct shell {
    ivec2 *crds;
    size_t cnt;
    size_t lvl;
} shell_t;

extern shell_t *gen_get_shells(ivec2 centre, size_t num_shells, shell_t *start_shell);
extern chunk_data_t *gen_generate_chunk_data(ivec2 pos, uint32_t seed);

#endif
