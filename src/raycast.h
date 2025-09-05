#ifndef RAYCAST_H
#define RAYCAST_H 

#include <stdbool.h>

#include <libem/em_math.h>

#include "geometry_types.h"
#include "list.h"
#include "state.h"

extern bool raycast_dda(state_t *state, chunk_t **hit_chunk, uvec3 *hit_block);

#endif
