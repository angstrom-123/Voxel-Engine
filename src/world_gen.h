#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <stdio.h>

#include "src/cube.h"
#include "state.h"
#include "extra_math.h"

extern void gen_instantiate_cube(state_t *state, em_vec3 pos, cube_type_e type);
extern void gen_instantiate_chunk(state_t *state, em_vec3 pos);

#endif
