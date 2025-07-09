#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include "object.h"
#include "extra_math.h"

extern chunk_t **gen_chunks_around(em_vec3 centre, uint8_t render_dist);
extern chunk_t *gen_new_chunk(em_vec3 pos);

#endif
