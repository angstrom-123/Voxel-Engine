#ifndef UPDATE_SYSTEM_H
#define UPDATE_SYSTEM_H

#include <stdatomic.h>
#include <threads.h>

#include "update_system_types.h"
#include "queue.h"
#include "hashmap.h"
#include "dlist.h"
#include "geometry.h"
#include "constants.h"

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#if !defined(SOKOL_TIME_INCLUDED) 
#include <sokol/sokol_time.h>
#endif

typedef struct render_data {
    HASHMAP(ivec2_render_chunk) *chunks;
} render_data_t;

typedef struct thread_wrapper {
    thrd_t thread;
    atomic_bool running;
    atomic_bool ready;
} thread_wrapper_t;

typedef struct update_system {
    CIRCULAR_QUEUE(us_request) *requests;
    HASHMAP(ivec2_render_chunk) *chunks;
    CIRCULAR_QUEUE(offset) *free;

    sg_buffer vbo;
    sg_buffer ibo;
    vertex_t *v_stage;
    uint32_t *i_stage;
    size_t v_size;
    size_t i_size;

    thread_wrapper_t update_worker;
    thread_wrapper_t tick_worker;

    mtx_t request_lock;
    mtx_t chunks_lock;
    mtx_t buffers_lock;
    cnd_t needs_update;

    double tps;
} update_system_t;

typedef struct update_system_desc {
    size_t request_capacity;
    double ticks_per_second;
    size_t free_capacity;
    size_t chunk_capacity;
} update_system_desc_t;

typedef struct update_system_thread_args {
    update_system_t *us;
} update_system_thread_args_t;

extern void update_sys_init(update_system_t *us, const update_system_desc_t *desc);
extern void update_sys_init_update_thread(update_system_t *us, update_system_thread_args_t *targs);
extern void update_sys_init_tick_thread(update_system_t *us, update_system_thread_args_t *targs);
extern void update_sys_cleanup(update_system_t *us);
extern void update_sys_make_request(update_system_t *us, us_request_t request);
extern render_data_t update_sys_get_render_data(update_system_t *us);
extern void update_sys_return_render_data(update_system_t *us, render_data_t *data);
extern void update_sys_force_buffer_update(update_system_t *us);

#endif 
