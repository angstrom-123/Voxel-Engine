#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include "geometry.h"
#include "extra_math.h"
#include "state.h"
#include "memory.h"

typedef struct chunk_set {
	uint16_t cnt;
	chunk_t **chunks;
} chunk_set_t;

extern size_t push_to_state(state_t *state, chunk_set_t chunk_data);
extern em_ivec3 *gen_get_required_coords(em_vec3 centre, uint8_t rndr_dist, size_t *cnt);
extern chunk_set_t gen_chunks_around(em_vec3 centre, uint8_t rndr_dist);
extern chunk_t *gen_new_chunk(int32_t x, int32_t y, int32_t z);

#endif
