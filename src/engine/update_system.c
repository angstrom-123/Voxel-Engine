#include "update_system.h"

static void _handle_request(update_system_t *us, us_request_t *r)
{
    switch (r->type) {
    case USREQ_STAGE:
    {
        INSTRUMENT_SCOPE_BEGIN(req_stage);
        chunk_render_info_t *cri = malloc(sizeof(chunk_render_info_t));
        cri->pos = r->pos;
        cri->mesh_o = r->mesh_o;
        cri->bufs_o = us->buffer_pool->dequeue(us->buffer_pool);
        if (cri->mesh_o) cri->needs_update_o = true;

        mtx_lock(&us->chunks_lock);

        us->chunks->put_ptr(us->chunks, r->pos, cri);

        mtx_unlock(&us->chunks_lock);
        INSTRUMENT_SCOPE_END(req_stage);
        break;
    }
    case USREQ_RESTAGE:
    {
        INSTRUMENT_SCOPE_BEGIN(req_restage);
        mtx_lock(&us->chunks_lock);

        chunk_render_info_t *cri = us->chunks->get_ptr(us->chunks, r->pos);
        cri->mesh_o = r->mesh_o;
        cri->needs_update_o = (cri->mesh_o);

        mtx_unlock(&us->chunks_lock);
        INSTRUMENT_SCOPE_END(req_restage);
        break;
    }
    case USREQ_UNSTAGE: 
    {
        INSTRUMENT_SCOPE_BEGIN(req_unstage);
        mtx_lock(&us->chunks_lock);

        if (us->chunks->contains_key(us->chunks, r->pos))
        {
            chunk_render_info_t cri = us->chunks->pop(us->chunks, r->pos);

            us->buffer_pool->enqueue(us->buffer_pool, cri.bufs_o);
            if (cri.mesh_o != NULL)
            {
                free(cri.mesh_o->v_buf);
                free(cri.mesh_o->i_buf);
                free(cri.mesh_o);
            }
        }

        mtx_unlock(&us->chunks_lock);
        INSTRUMENT_SCOPE_END(req_unstage);
        break;
    }
    }
}

static int _thread_func(void *args)
{
    update_system_thread_args_t *targs = args;
    update_system_t *us = targs->us;
    ENGINE_LOG_OK("Update thread init.\n", NULL);

    while (atomic_load(&us->running))
    {
        mtx_lock(&us->requests_lock);
        atomic_store(&us->thread_ready, true);
        cnd_wait(&us->needs_update, &us->requests_lock);

        while (us->requests->count > 0)
        {
            atomic_store(&us->processing, true);
            us_request_t *r = us->requests->dequeue_ptr(us->requests);
            _handle_request(us, r);
            free(r);
            atomic_store(&us->processing, false);
        }

        mtx_unlock(&us->requests_lock);
    }

    ENGINE_LOG_WARN("Update thread terminating.\n", NULL);

    return 0;
}

static void _await_requests_complete(update_system_t *us)
{
    bool finished = false;
    while (!finished)
    {
        mtx_lock(&us->requests_lock);
        if (us->requests->count == 0 && !atomic_load(&us->processing))
            finished = true;

        mtx_unlock(&us->requests_lock);

        if (!finished)
        {
            ENGINE_LOG_WARN("Awaiting US requests", NULL);
            thrd_sleep(&(struct timespec) {
                .tv_nsec = THREAD_AWAIT_NS
            }, NULL);
        }
    }
}

void update_sys_init(update_system_t *us, const update_system_desc_t *desc)
{
    us->requests = CIRCULAR_QUEUE_NEW(us_request)(&(em_circular_queue_desc_t) {
        .capacity = desc->request_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(us_request),
        .flags = EM_FLAG_NO_RESIZE
    });

    us->chunks = HASHMAP_NEW(ivec2_cri)(&(em_hashmap_desc_t) {
        .capacity = desc->chunk_capacity,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(ivec2),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(ivec2),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(ivec2),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(cri),
        .flags = EM_FLAG_NO_RESIZE
    });

    us->buffer_pool = CIRCULAR_QUEUE_NEW(sokol_buffers)(&(em_circular_queue_desc_t) {
        .capacity = desc->free_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(sokol_buffers),
        .flags = EM_FLAG_NO_RESIZE | EM_FLAG_NO_DESTROY_ENTRIES
    });
    for (size_t i = 0; i < us->buffer_pool->size; i++)
    {
        sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc) {
            .size = V_MAX * sizeof(packed_vertex_t),
            .usage = {
                .vertex_buffer = true,
                .dynamic_update = true
            },
        });
        sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc) {
            .size = I_MAX * sizeof(uint32_t),
            .usage = {
                .index_buffer = true,
                .dynamic_update = true
            },
        });
        us->buffer_pool->enqueue(us->buffer_pool, (buffer_pair_t) { vbuf, ibuf });
    }

    mtx_init(&us->requests_lock, mtx_plain);
    mtx_init(&us->chunks_lock, mtx_plain);
    cnd_init(&us->needs_update);

    us->running = false;
    us->thread_ready = false;
}

void update_sys_init_thread(update_system_t *us, update_system_thread_args_t *targs)
{
    us->running = true;
    int res = thrd_create(&us->worker, _thread_func, targs);
    ENGINE_ASSERT(res == thrd_success, "Failed to initialize worker thread.\n");

    /* Block until the thread is ready. */
    while (!atomic_load(&us->thread_ready))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}

void update_sys_cleanup(update_system_t *us)
{
    atomic_store(&us->running, false);
    cnd_signal(&us->needs_update);
    thrd_join(us->worker, NULL);

    us->chunks->destroy(us->chunks);
    us->requests->destroy(us->requests);
    us->buffer_pool->destroy(us->buffer_pool);

    mtx_destroy(&us->chunks_lock);
    mtx_destroy(&us->requests_lock);
    cnd_destroy(&us->needs_update);
}

void update_sys_unstage_all(update_system_t *us)
{
    mtx_lock(&us->requests_lock);
    
    // Remove all pending requests
    us->requests->clear(us->requests);

    mtx_lock(&us->chunks_lock);

    // Request all chunks to be unloaded
    em_hashmap_iter_t *it = us->chunks->iterator(us->chunks);
    while (it->has_next)
    {
        em_hashmap_entry_t *e = it->get(it);
        ivec2 *pos = e->key;
        us->requests->enqueue(us->requests, (us_request_t) {
            .type = USREQ_UNSTAGE,
            .pos = *pos
        });
        it->next(it);
    }
    free(it);
    mtx_unlock(&us->requests_lock);
    mtx_unlock(&us->chunks_lock);

    // Wait for all unstage requests to be complete.
    cnd_signal(&us->needs_update);
    _await_requests_complete(us);
}

void update_sys_make_request(update_system_t *us, us_request_t r)
{
    mtx_lock(&us->requests_lock);

    us->requests->enqueue(us->requests, r);
    cnd_signal(&us->needs_update);

    mtx_unlock(&us->requests_lock);
}

render_data_t update_sys_borrow_render_data(update_system_t *us)
{
    mtx_lock(&us->chunks_lock);

    return (render_data_t) {
        .chunks = us->chunks,
    };
}

void update_sys_return_render_data(update_system_t *us, render_data_t *data)
{
    (void) data;
    mtx_unlock(&us->chunks_lock);
}
