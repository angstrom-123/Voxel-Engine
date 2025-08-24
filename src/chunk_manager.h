#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H 

#include "world_gen.h"
#include "geometry_types.h"
#include "hashmap.h"
#include "list.h"
#include "state.h"

extern void manager_generate_chunks_init(state_t *state);
extern void manager_generate_chunks(state_t *state);
extern void manager_demote_chunks(state_t *state);
extern void manager_cleanup_chunks(state_t *state);

#endif
