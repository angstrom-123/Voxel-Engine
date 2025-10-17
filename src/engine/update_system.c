#include "update_system.h"

static void _handle_request(update_system_t *us, us_request_t *r)
{
    switch (r->type) {
    case USREQ_STAGE:
    {
        // TODO: Minimize number of malloc calls for this data.
        chunk_render_info_t *cri = malloc(sizeof(chunk_render_info_t));
        cri->pos = r->pos;
        cri->offset = us->free->dequeue(us->free);
        cri->index_cnt = r->mesh->i_cnt;

        mtx_lock(&us->chunks_lock);

        us->chunks->put_ptr(us->chunks, r->pos, cri);

        mtx_unlock(&us->chunks_lock);

        mtx_lock(&us->buffers_lock);

        memcpy(&us->v_stage[cri->offset.v_ofst], &r->mesh->v_buf[0], 
               r->mesh->v_cnt * sizeof(vertex_t));
        memcpy(&us->i_stage[cri->offset.i_ofst], &r->mesh->i_buf[0], 
               r->mesh->i_cnt * sizeof(uint32_t));

        mtx_unlock(&us->buffers_lock);

        free(r->mesh->v_buf);
        free(r->mesh->i_buf);
        free(r->mesh);

        break;
    }
    case USREQ_UNSTAGE: 
    {
        if (us->chunks->contains_key(us->chunks, r->pos))
        {
            chunk_render_info_t cri = us->chunks->pop(us->chunks, r->pos);
            us->free->enqueue(us->free, cri.offset);
        }

        break;
    }
    }
}

static void _dump_buffers(update_system_t *us)
{
    FILE *if_ptr = fopen("IBUF.txt", "w");
    for (size_t i = 0; i < us->i_size / sizeof(uint32_t); i++)
        fprintf(if_ptr, "%zu| %u\n", i, us->i_stage[i]);
    fclose(if_ptr);

    FILE *vf_ptr = fopen("VBUF.txt", "w");
    for (size_t i = 0; i < us->v_size / sizeof(vertex_t); i++)
    {
        vertex_t v = us->v_stage[i];
        fprintf(vf_ptr, "%zu| xyz {%i, %i, %i}\n", i, v.x, v.y, v.z);
    }
    fclose(vf_ptr);
}

static void _handle_tick(update_system_t *us)
{
    mtx_lock(&us->chunks_lock);
    mtx_lock(&us->buffers_lock);
    // ENGINE_LOG_OK("Tick.\n", NULL);
    // ENGINE_LOG_WARN("Updating VBO handle %u\n", us->vbo.id);
    // ENGINE_LOG_WARN("Updating IBO handle %u\n", us->ibo.id);

    // sg_update_buffer(us->vbo, &(sg_range) {
    //     .ptr = us->v_stage,
    //     .size = us->v_size
    // });
    // sg_update_buffer(us->ibo, &(sg_range) {
    //     .ptr = us->i_stage,
    //     .size = us->i_size
    // });

    // _dump_buffers(us);

    mtx_unlock(&us->buffers_lock);
    mtx_unlock(&us->chunks_lock);
}

static int _update_thread_func(void *args)
{
    update_system_thread_args_t *targs = args;
    update_system_t *us = targs->us;
    ENGINE_LOG_OK("Update thread init.\n", NULL);

    while (atomic_load(&us->update_worker.running))
    {
        mtx_lock(&us->request_lock);
        atomic_store(&us->update_worker.ready, true);
        cnd_wait(&us->needs_update, &us->request_lock);

        while (us->requests->count > 0)
        {
            us_request_t *r = us->requests->dequeue_ptr(us->requests);
            _handle_request(us, r);
            free(r);
        }

        mtx_unlock(&us->request_lock);
    }

    ENGINE_LOG_WARN("Update thread terminating.\n", NULL);

    return 0;
}

static int _tick_thread_func(void *args)
{
    stm_setup();

    update_system_thread_args_t *targs = args;
    update_system_t *us = targs->us;

    double period_ms = (1.0 / us->tps) * 1000.0;
    uint64_t last_time = stm_now();

    ENGINE_LOG_OK("Tick thread init.\n", NULL);
    atomic_store(&us->tick_worker.ready, true);

    while (atomic_load(&us->tick_worker.running))
    {
        uint64_t curr_time = stm_now();
        double elapsed_ms = stm_ms(stm_diff(curr_time, last_time));

        if (elapsed_ms >= period_ms)
        {
            last_time = curr_time;
            _handle_tick(us);
        }
    }

    ENGINE_LOG_WARN("Tick thread terminating.\n", NULL);

    return 0;
}

void update_sys_init(update_system_t *us, const update_system_desc_t *desc)
{
    us->requests = CIRCULAR_QUEUE_NEW(us_request)(&(em_circular_queue_desc_t) {
        .capacity = desc->request_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(us_request),
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
    mtx_init(&us->buffers_lock, mtx_plain);
    cnd_init(&us->needs_update);

    us->tps = desc->ticks_per_second;

    us->update_worker.running = false;
    us->update_worker.ready = false;

    us->tick_worker.running = false;
    us->tick_worker.ready = false;
}

void update_sys_init_update_thread(update_system_t *us, update_system_thread_args_t *targs)
{
    us->update_worker.running = true;
    int res = thrd_create(&us->update_worker.thread, _update_thread_func, targs);
    if (res != thrd_success)
    {
        ENGINE_LOG_ERROR("Failed to initialize update thread. Aborting.\n", NULL);
        exit(1);
    }

    /* Block until the thread is ready. */
    while (!atomic_load(&us->update_worker.ready))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}

void update_sys_init_tick_thread(update_system_t *us, update_system_thread_args_t *targs)
{
    us->tick_worker.running = true;
    int res = thrd_create(&us->tick_worker.thread, _tick_thread_func, targs);
    if (res != thrd_success)
    {
        ENGINE_LOG_ERROR("Failed to initialize tick thread. Aborting.\n", NULL);
        exit(1);
    }

    /* Block until the thread is ready. */
    while (!atomic_load(&us->tick_worker.ready))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}

void update_sys_cleanup(update_system_t *us)
{
    atomic_store(&us->update_worker.running, false);
    atomic_store(&us->tick_worker.running, false);
    thrd_join(us->update_worker.thread, NULL);
    thrd_join(us->tick_worker.thread, NULL);

    us->chunks->destroy(us->chunks);
    us->free->destroy(us->free);
    us->requests->destroy(us->requests);

    mtx_destroy(&us->chunks_lock);
    mtx_destroy(&us->request_lock);
    mtx_destroy(&us->buffers_lock);
    cnd_destroy(&us->needs_update);

    sg_destroy_buffer(us->vbo);
    sg_destroy_buffer(us->ibo);

    free(us->v_stage);
    free(us->i_stage);
}

void update_sys_make_request(update_system_t *us, us_request_t r)
{
    mtx_lock(&us->request_lock);

    us->requests->enqueue(us->requests, r);
    cnd_signal(&us->needs_update);

    mtx_unlock(&us->request_lock);
}

render_data_t update_sys_get_render_data(update_system_t *us)
{
    mtx_lock(&us->chunks_lock);
    mtx_lock(&us->buffers_lock);

    return (render_data_t) {
        .chunks = us->chunks,
    };
}

void update_sys_return_render_data(update_system_t *us, render_data_t *data)
{
    (void) data;
    mtx_unlock(&us->chunks_lock);
    mtx_unlock(&us->buffers_lock);
}

void update_sys_force_buffer_update(update_system_t *us)
{
    sg_update_buffer(us->vbo, &(sg_range) {
        .ptr = us->v_stage,
        .size = us->v_size
    });

    sg_update_buffer(us->ibo, &(sg_range) {
        .ptr = us->i_stage,
        .size = us->i_size
    });
}
