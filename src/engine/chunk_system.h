#ifndef CHUNK_SYSTEM_H
#define CHUNK_SYSTEM_H

#include "data_structures.h"
#include "chunk_system_types.h"
#include "geometry.h"
#include "update_system.h"
#include "files.h"

#include <stdio.h>
#include <threads.h>

#define CS_REQUEST(cs, typ, crd) \
    chunk_sys_make_request(cs, (cs_request_t) \
            { .type = typ, .pos = crd })
#define CS_REQUEST_BREAK(cs, crd, cel) \
    chunk_sys_make_request(cs, (cs_request_t) \
            { .type = CSREQ_BREAK, .pos = crd, .cell = cel });
#define CS_REQUEST_PLACE(cs, crd, cel, blo) \
    chunk_sys_make_request(cs, (cs_request_t) \
            { .type = CSREQ_PLACE, .pos = crd, .cell = cel, .block = blo });

typedef struct chunk_system {
    HASHMAP(ivec2_chunk_data) *genned;
    CIRCULAR_QUEUE(cs_request) *requests;

    chunk_data_t *( *gen_func)(ivec2, uint32_t);

    thrd_t worker;
    mtx_t requests_lock;
    mtx_t genned_lock;
    cnd_t needs_update;
    atomic_bool running;
    atomic_bool thread_ready;
    atomic_bool initial_load_complete;
    atomic_bool processing;

    uint32_t seed;
    char world_dir_path[STD_BUFLEN];
    bool synchronized;
    bool receiving;
} chunk_system_t;

typedef struct chunk_system_desc {
    size_t chunk_data_capacity;
    size_t request_capacity;
    uint32_t seed;
    chunk_data_t *( *gen_func)(ivec2, uint32_t);
} chunk_system_desc_t;

typedef struct chunk_system_thread_args {
    chunk_system_t *cs;
    update_system_t *us;
} chunk_system_thread_args_t;

extern void chunk_sys_init(chunk_system_t *cs, const chunk_system_desc_t *desc);
extern void chunk_sys_init_thread(chunk_system_t *cs, chunk_system_thread_args_t *targs);
extern void chunk_sys_cleanup(chunk_system_t *cs);
extern void chunk_sys_make_request(chunk_system_t *cs, cs_request_t request);
extern void chunk_sys_borrow_surrounding_data(chunk_system_t *cs, ivec2 pos, chunk_set_t *chunks);
extern void chunk_sys_return_surrounding_data(chunk_system_t *cs, chunk_set_t *chunks);
extern void chunk_sys_await_initial_load_complete(chunk_system_t *cs);

#endif
