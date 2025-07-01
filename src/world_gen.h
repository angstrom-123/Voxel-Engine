#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <stdio.h>

#include "state.h"
#include "extra_math.h"
#include "mesh.h"

extern void gen_instantiate_cube(state_t *state, em_vec3 pos);
extern void gen_instantiate_chunk(state_t *state, em_vec3 pos);

#endif
