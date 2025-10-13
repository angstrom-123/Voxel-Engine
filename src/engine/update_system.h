#ifndef UPDATE_SYSTEM_H
#define UPDATE_SYSTEM_H

#include <stdatomic.h>
#include <threads.h>

#include "update_system_types.h"
#include "queue.h"
#include "hashmap.h"
#include "dlist.h"

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

typedef struct update_system {
    /*
     *  Buffers and mutexes here,
     *  mutex acquired my mt for render.
     *
     *  worker pulls meshes from gen thread.
     *  gen thread submits meshes to q.
     */
    CIRCULAR_QUEUE(cs_result) *request_queue;
    HASHMAP(ivec2_render_chunk) *chunks;
    DOUBLE_LIST(ivec2) *visible;
    CIRCULAR_QUEUE(offset) *free;

    sg_buffer vbo;
    sg_buffer ibo;
    vertex_t *v_stage;
    uint32_t *i_stage;
    size_t v_size;
    size_t i_size;

    thrd_t update_worker;
    mtx_t request_lock;

    atomic_bool running;
    size_t tps;
} update_system_t;

typedef struct update_system_desc {
    size_t request_capacity;
    size_t ticks_per_second;
} update_system_desc_t;

extern void us_init(update_system_t *us, const update_system_desc_t *desc);
extern void us_cleanup(update_system_t *us);
extern void us_submit(update_system_t *us, cs_result_t result);

#endif 
