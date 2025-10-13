#ifndef CHUNK_SYSTEM_H
#define CHUNK_SYSTEM_H

#include "chunk_system_types.h"
#include "hashmap.h"
#include "queue.h"
#include "dlist.h"
#include "geometry.h"
#include "world_gen.h"

#include <threads.h>

typedef struct chunk_system {
    HASHMAP(ivec2_chunk_data) *genned;
    CIRCULAR_QUEUE(cs_request) *requests;
    CIRCULAR_QUEUE(cs_result) *accumulator;

    thrd_t worker;
    mtx_t requests_lock;
    mtx_t accumulator_lock;
    mtx_t init_lock;
    cnd_t needs_update;
    cnd_t thread_initialized;
    atomic_bool running;

    uint32_t seed;
    bool synchronized;
} chunk_system_t;

typedef struct chunk_system_desc {
    size_t accumulator_capacity;
    size_t request_capacity;
    size_t free_capacity;
    size_t chunk_capacity;
    uint32_t seed;
} chunk_system_desc_t;

extern void cs_init(chunk_system_t *cs, const chunk_system_desc_t *desc);
extern void cs_cleanup(chunk_system_t *cs);

extern void cs_make_request(chunk_system_t *cs, cs_request_t request);
// extern bool cs_pull_n_changes(chunk_system_front_t *csf, chunk_system_back_t *csb, size_t cnt);
// extern void cs_upload_stages(chunk_system_front_t *csf);

#endif
