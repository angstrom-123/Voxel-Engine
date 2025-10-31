#include "update_system.h"

static void _handle_request(update_system_t *us, us_request_t *r)
{
    INSTRUMENT_FUNC_BEGIN();
    switch (r->type) {
    case USREQ_STAGE:
    {
        chunk_render_info_t *cri = malloc(sizeof(chunk_render_info_t));
        cri->pos = r->pos;
        cri->mesh = r->mesh;
        cri->bufs = us->buffer_pool->dequeue(us->buffer_pool);
        cri->needs_update = true;

        mtx_lock(&us->chunks_lock);

        us->chunks->put_ptr(us->chunks, r->pos, cri);

        mtx_unlock(&us->chunks_lock);
        break;
    }
    case USREQ_RESTAGE:
    {
        mtx_lock(&us->chunks_lock);

        chunk_render_info_t *cri = us->chunks->get_ptr(us->chunks, r->pos);
        cri->mesh = r->mesh;
        cri->needs_update = true;

        mtx_unlock(&us->chunks_lock);
        break;
    }
    case USREQ_UNSTAGE: 
    {
        mtx_lock(&us->chunks_lock);

        if (us->chunks->contains_key(us->chunks, r->pos))
        {
            chunk_render_info_t cri = us->chunks->pop(us->chunks, r->pos);
            us->buffer_pool->enqueue(us->buffer_pool, cri.bufs);

            free(cri.mesh->v_buf);
            free(cri.mesh->i_buf);
            free(cri.mesh);
        }

        mtx_unlock(&us->chunks_lock);
        break;
    }
    }
    INSTRUMENT_FUNC_END();
}

static int _thread_func(void *args)
{
    update_system_thread_args_t *targs = args;
    update_system_t *us = targs->us;
    ENGINE_LOG_OK("Update thread init.\n", NULL);

    while (atomic_load(&us->running))
    {
        mtx_lock(&us->request_lock);
        atomic_store(&us->thread_ready, true);
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
        us->buffer_pool->enqueue(us->buffer_pool, (us_buffer_pair_t) {vbuf, ibuf});
    }

    mtx_init(&us->request_lock, mtx_plain);
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
    mtx_destroy(&us->request_lock);
    cnd_destroy(&us->needs_update);
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

    return (render_data_t) {
        .chunks = us->chunks,
    };
}

void update_sys_return_render_data(update_system_t *us, render_data_t *data)
{
    (void) data;
    mtx_unlock(&us->chunks_lock);
}
