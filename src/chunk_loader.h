#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include <libem/em_math.h>
#include <libem/em_hashmap.h>
#include <libem/em_list.h>

#include "state.h"
#include "world_gen.h"

extern void stage_chunk(state_t *state, chunk_t *chunk, mesh_t *mesh);
extern void restage_chunk(state_t *state, chunk_t *chunk, mesh_t *mesh);
extern void load_initial_chunks(state_t *state);
extern void load_chunks(state_t *state);
extern void cleanup_old_chunks(state_t *state);

#endif
