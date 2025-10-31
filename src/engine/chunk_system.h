#ifndef CHUNK_SYSTEM_H
#define CHUNK_SYSTEM_H

#include "chunk_system_types.h"
#include "hashmap.h"
#include "queue.h"
#include "geometry.h"
#include "world_gen.h"
#include "update_system.h"

#include <threads.h>

#define CS_REQUEST(cs, typ, crd) \
    chunk_sys_make_request(cs, (cs_request_t) {.type = typ, .pos = crd})

typedef struct chunk_system {
    HASHMAP(ivec2_chunk_data) *genned;
    CIRCULAR_QUEUE(cs_request) *requests;

    thrd_t worker;
    mtx_t requests_lock;
    mtx_t genned_lock;
    cnd_t needs_update;
    atomic_bool running;
    atomic_bool thread_ready;

    uint32_t seed;
    bool synchronized;
} chunk_system_t;

typedef struct chunk_system_desc {
    size_t chunk_data_capacity;
    size_t request_capacity;
    uint32_t seed;
} chunk_system_desc_t;

typedef struct chunk_system_thread_args {
    chunk_system_t *cs;
    update_system_t *us;
} chunk_system_thread_args_t;

extern void chunk_sys_init(chunk_system_t *cs, const chunk_system_desc_t *desc);
extern void chunk_sys_init_thread(chunk_system_t *cs, chunk_system_thread_args_t *targs);
extern void chunk_sys_cleanup(chunk_system_t *cs);
extern void chunk_sys_make_request(chunk_system_t *cs, cs_request_t request);
extern void chunk_sys_get_surrounding_data(chunk_system_t *cs, ivec2 pos, chunk_data_t *res[3][3]);
extern void chunk_sys_return_surrounding_data(chunk_system_t *cs, chunk_data_t *data[3][3]);

#endif
