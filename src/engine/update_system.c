#include "update_system.h"

static void _handle_request(update_system_t *us, cs_result_t *r)
{
    mtx_lock(&us->chunks_lock);
    mtx_lock(&us->stages_lock);

    // TODO: Minimize number of malloc calls for this data.
    chunk_render_info_t *cri = malloc(sizeof(chunk_render_info_t));
    cri->pos = r->pos;
    cri->offset = us->free->dequeue(us->free);
    cri->index_cnt = r->mesh->i_cnt;

    us->chunks->put_ptr(us->chunks, r->pos, cri);

    // if (!r->mesh)
    // {
    //     ENGINE_LOG_ERROR("No mesh in update system pull. Aborting.\n", NULL);
    //     exit(1);
    // }
    //
    memcpy(&us->v_stage[cri->offset.v_ofst], &r->mesh->v_buf[0], r->mesh->v_cnt * sizeof(vertex_t));
    memcpy(&us->i_stage[cri->offset.i_ofst], &r->mesh->i_buf[0], r->mesh->i_cnt * sizeof(uint32_t));

    free(r->mesh->v_buf);
    free(r->mesh->i_buf);
    free(r->mesh);

    mtx_unlock(&us->chunks_lock);
    mtx_unlock(&us->stages_lock);
}

static int _thread_func(void *args)
{
    update_system_t *us = args;
    cnd_signal(&us->thread_initialized);
    ENGINE_LOG_OK("Update thread init.\n", NULL);

    while (atomic_load(&us->running))
    {
        mtx_lock(&us->request_lock);

        cnd_wait(&us->needs_update, &us->request_lock);

        while (us->requests->count > 0)
        {
            cs_result_t *r = us->requests->dequeue_ptr(us->requests);
            _handle_request(us, r);
            free(r);
        }

        mtx_unlock(&us->request_lock);
    }

    ENGINE_LOG_WARN("Update thread terminating.\n", NULL);

    return 0;
}

void update_sys_init(update_system_t *us, const update_system_desc_t *desc)
{
    us->requests = CIRCULAR_QUEUE_NEW(cs_result)(&(em_circular_queue_desc_t) {
        .capacity = desc->request_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(cs_result),
        .flags = EM_FLAG_NONE
    });

    us->chunks = HASHMAP_NEW(ivec2_render_chunk)(&(em_hashmap_desc_t) {
        .capacity = desc->chunk_capacity,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(ivec2),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(ivec2),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(ivec2),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(front_chunk),
        .flags = EM_FLAG_NO_RESIZE
    });

    us->free = CIRCULAR_QUEUE_NEW(offset)(&(em_circular_queue_desc_t) {
        .capacity = desc->free_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(offset),
        .flags = EM_FLAG_NO_RESIZE
    });
    for (size_t i = 0; i < desc->free_capacity; i++)
    {
        us->free->enqueue(us->free, (offset_t) {
            .v_ofst = V_MAX * i,
            .i_ofst = I_MAX * i
        });
    }

    const size_t V_SIZE = V_MAX * desc->free_capacity * sizeof(vertex_t);
    const size_t I_SIZE = I_MAX * desc->free_capacity * sizeof(uint32_t);

    us->v_size = V_SIZE;
    us->v_stage = malloc(V_SIZE);
    us->vbo = sg_make_buffer(&(sg_buffer_desc) {
        .size = V_SIZE,
        .usage = {
            .vertex_buffer = true,
            .dynamic_update = true
        },
        .label = "Vertex Buffer"
    });

    us->i_size = I_SIZE;
    us->i_stage = malloc(I_SIZE);
    us->ibo = sg_make_buffer(&(sg_buffer_desc) {
        .size = I_SIZE,
        .usage = {
            .index_buffer = true,
            .dynamic_update = true
        },
        .label = "Index Buffer"
    });

    mtx_init(&us->request_lock, mtx_plain);
    mtx_init(&us->chunks_lock, mtx_plain);
    mtx_init(&us->stages_lock, mtx_plain);
    mtx_init(&us->init_lock, mtx_plain);
    cnd_init(&us->needs_update);
    cnd_init(&us->thread_initialized);

    us->tps = desc->ticks_per_second;
    us->running = true;

    int res = thrd_create(&us->update_worker, _thread_func, us);
    if (res != thrd_success)
    {
        ENGINE_LOG_ERROR("Failed to initialize update thread. Aborting.\n", NULL);
        exit(1);
    }
}

void update_sys_cleanup(update_system_t *us)
{
    atomic_store(&us->running, false);
    thrd_join(us->update_worker, NULL);

    us->chunks->destroy(us->chunks);
    us->free->destroy(us->free);
    us->requests->destroy(us->requests);

    mtx_destroy(&us->chunks_lock);
    mtx_destroy(&us->request_lock);
    mtx_destroy(&us->stages_lock);
    mtx_destroy(&us->init_lock);
    cnd_destroy(&us->needs_update);
    cnd_destroy(&us->thread_initialized);

    sg_destroy_buffer(us->vbo);
    sg_destroy_buffer(us->ibo);

    free(us->v_stage);
    free(us->i_stage);
}

void update_sys_submit(update_system_t *us, cs_result_t r)
{
    mtx_lock(&us->request_lock);

    us->requests->enqueue(us->requests, r);
    cnd_signal(&us->needs_update);

    mtx_unlock(&us->request_lock);
}

render_data_t update_sys_get_render_data(update_system_t *us)
{
    mtx_lock(&us->chunks_lock);
    mtx_lock(&us->stages_lock);

    sg_update_buffer(us->vbo, &(sg_range) {
        .ptr = us->v_stage,
        .size = us->v_size
    });
    sg_update_buffer(us->ibo, &(sg_range) {
        .ptr = us->i_stage,
        .size = us->i_size
    });

    return (render_data_t) {
        .chunks = us->chunks,
        .vbo = us->vbo,
        .ibo = us->ibo
    };
}

void update_sys_return_render_data(update_system_t *us, render_data_t *data)
{
    (void) data;
    mtx_unlock(&us->chunks_lock);
    mtx_unlock(&us->stages_lock);
}

void update_sys_refresh_buffers(update_system_t *us)
{
    mtx_lock(&us->stages_lock);

    sg_update_buffer(us->vbo, &(sg_range) {
        .ptr = us->v_stage,
        .size = us->v_size
    });

    sg_update_buffer(us->ibo, &(sg_range) {
        .ptr = us->v_stage,
        .size = us->v_size
    });

    mtx_unlock(&us->stages_lock);
}
