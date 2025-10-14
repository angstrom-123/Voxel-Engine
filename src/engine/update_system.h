#ifndef UPDATE_SYSTEM_H
#define UPDATE_SYSTEM_H

#include <stdatomic.h>
#include <threads.h>

#include "update_system_types.h"
#include "queue.h"
#include "hashmap.h"
#include "dlist.h"
#include "geometry.h"

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

typedef struct render_data {
    HASHMAP(ivec2_render_chunk) *chunks;
    sg_buffer vbo;
    sg_buffer ibo;
} render_data_t;

typedef struct update_system {
    /*
     *  Buffers and mutexes here,
     *  mutex acquired my mt for render.
     *
     *  worker pulls meshes from gen thread.
     *  gen thread submits meshes to q.
     */
    CIRCULAR_QUEUE(cs_result) *requests;
    HASHMAP(ivec2_render_chunk) *chunks;
    CIRCULAR_QUEUE(offset) *free;

    sg_buffer vbo;
    sg_buffer ibo;
    vertex_t *v_stage;
    uint32_t *i_stage;
    size_t v_size;
    size_t i_size;

    thrd_t update_worker;
    mtx_t request_lock;
    mtx_t chunks_lock;
    mtx_t stages_lock;
    mtx_t init_lock;
    cnd_t needs_update;
    cnd_t thread_initialized;

    atomic_bool running;
    size_t tps;
} update_system_t;

typedef struct update_system_desc {
    size_t request_capacity;
    size_t ticks_per_second;
    size_t free_capacity;
    size_t chunk_capacity;
} update_system_desc_t;

extern void update_sys_init(update_system_t *us, const update_system_desc_t *desc);
extern void update_sys_cleanup(update_system_t *us);
extern void update_sys_submit(update_system_t *us, cs_result_t result);
extern render_data_t update_sys_get_render_data(update_system_t *us);
extern void update_sys_return_render_data(update_system_t *us, render_data_t *data);
extern void update_sys_refresh_buffers(update_system_t *us);

#endif 
