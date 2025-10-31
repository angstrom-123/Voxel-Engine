#ifndef UPDATE_SYSTEM_H
#define UPDATE_SYSTEM_H

#include <stdatomic.h>
#include <threads.h>

#include "update_system_types.h"
#include "queue.h"
#include "hashmap.h"
#include "geometry.h"
#include "base.h"
#include "instrumentor.h"

#define US_REQUEST(us, typ, crd, msh) \
    update_sys_make_request(us, (us_request_t) {.type = typ, .pos = crd, .mesh = msh})

typedef struct render_data {
    HASHMAP(ivec2_cri) *chunks;
} render_data_t;

typedef struct update_system {
    CIRCULAR_QUEUE(us_request) *requests;
    HASHMAP(ivec2_cri) *chunks;
    CIRCULAR_QUEUE(sokol_buffers) *buffer_pool;

    thrd_t worker;
    atomic_bool running;
    atomic_bool thread_ready;

    mtx_t request_lock;
    mtx_t chunks_lock;
    cnd_t needs_update;
} update_system_t;

typedef struct update_system_desc {
    size_t request_capacity;
    size_t free_capacity;
    size_t chunk_capacity;
} update_system_desc_t;

typedef struct update_system_thread_args {
    update_system_t *us;
} update_system_thread_args_t;

extern void update_sys_init(update_system_t *us, const update_system_desc_t *desc);
extern void update_sys_init_thread(update_system_t *us, update_system_thread_args_t *targs);
extern void update_sys_cleanup(update_system_t *us);
extern void update_sys_make_request(update_system_t *us, us_request_t request);
extern render_data_t update_sys_get_render_data(update_system_t *us);
extern void update_sys_return_render_data(update_system_t *us, render_data_t *data);

#endif 
