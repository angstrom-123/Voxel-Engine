#include "chunk_system.h"

static void _handle_request(chunk_system_t *cs, update_system_t *us, cs_request_t *r)
{
    switch (r->type) {
    case CSREQ_GEN:
    {
        chunk_data_t *data = gen_generate_chunk_data(r->pos, cs->seed);
        // ENGINE_LOG_OK("Adding %i %i inner map.\n", r->pos.x, r->pos.y);
        cs->genned->put_ptr(cs->genned, r->pos, data);

        break;
    }
    case CSREQ_MESH:
    {
        chunk_data_t *c = cs->genned->get_or_default(cs->genned, r->pos, NULL);
        chunk_data_t *n = cs->genned->get_or_default(cs->genned, REL_N(r->pos), NULL);
        chunk_data_t *e = cs->genned->get_or_default(cs->genned, REL_E(r->pos), NULL);
        chunk_data_t *s = cs->genned->get_or_default(cs->genned, REL_S(r->pos), NULL);
        chunk_data_t *w = cs->genned->get_or_default(cs->genned, REL_W(r->pos), NULL);

        if (c && n && e && s && w)
        {
            mesh_t *mesh = geom_generate_full_mesh(c, n, e, s, w);

            mtx_lock(&cs->accumulator_lock);

            update_sys_make_request(us, (us_request_t) {
                .type = USREQ_STAGE,
                .pos = r->pos,
                .mesh = mesh 
            });

            mtx_unlock(&cs->accumulator_lock);
        }
        else 
        {
            ENGINE_LOG_ERROR("Failed to mesh chunk at %i %i because required chunks are not genned.\n",
                      r->pos.x, r->pos.y);
            if (!c) 
                ENGINE_LOG_ERROR("Found current: %i %i\n", 
                          r->pos.x, r->pos.y);
            if (!n) 
                ENGINE_LOG_ERROR("Not found north: %i %i\n", 
                          r->pos.x, r->pos.y + CHUNK_SIZE);
            if (!e) 
                ENGINE_LOG_ERROR("Not found east: %i %i\n", 
                          r->pos.x + CHUNK_SIZE, r->pos.y);
            if (!s) 
                ENGINE_LOG_ERROR("Not found south: %i %i\n", 
                          r->pos.x, r->pos.y - CHUNK_SIZE);
            if (!w) 
                ENGINE_LOG_ERROR("Not found west: %i %i\n", 
                          r->pos.x - CHUNK_SIZE, r->pos.y);
            exit(1);
        }

        break;
    }
    case CSREQ_UNLOAD:
        if (cs->genned->contains_key(cs->genned, r->pos))
            cs->genned->remove(cs->genned, r->pos);
        break;
    }
}

static int _thread_func(void *args)
{
    chunk_system_thread_args_t *targs = args;
    chunk_system_t *cs = targs->cs;
    update_system_t *us = targs->us;

    ENGINE_LOG_OK("Chunk thread init.\n", NULL);

    const size_t BATCH_CNT = 100000;

    while (atomic_load(&cs->running))
    {
        mtx_lock(&cs->requests_lock);
        atomic_store(&cs->thread_ready, true);
        cnd_wait(&cs->needs_update, &cs->requests_lock);

        size_t handled_cnt = 0;
        while (cs->requests->count > 0 && handled_cnt++ < BATCH_CNT)
        {
            cs_request_t *r = cs->requests->dequeue_ptr(cs->requests);
            _handle_request(cs, us, r);
            free(r);
        }

        mtx_unlock(&cs->requests_lock);
    }

    ENGINE_LOG_WARN("Generation thread terminating.\n", NULL);

    return 0;
}

void chunk_sys_init(chunk_system_t *cs, const chunk_system_desc_t *desc)
{
    cs->seed = desc->seed;
    cs->running = false;
    cs->thread_ready = false;

    cs->genned = HASHMAP_NEW(ivec2_chunk_data)(&(em_hashmap_desc_t) {
        .capacity = desc->chunk_data_capacity,
        .cmp_func = (void_cmp_func) HASHMAP_CMP(ivec2),
        .hsh_func = (void_hsh_func) HASHMAP_HSH(ivec2),
        .cln_k_func = (void_cln_func) HASHMAP_CLN_K(ivec2),
        .cln_v_func = (void_cln_func) HASHMAP_CLN_V(chunk_data),
        .flags = EM_FLAG_NO_RESIZE
    });

    cs->requests = CIRCULAR_QUEUE_NEW(cs_request)(&(em_circular_queue_desc_t) {
        .capacity = desc->request_capacity,
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(cs_request),
        .flags = EM_FLAG_NONE // Resizable.
    });

    mtx_init(&cs->requests_lock, mtx_plain);
    mtx_init(&cs->accumulator_lock, mtx_plain);
    cnd_init(&cs->needs_update);
}

void chunk_sys_init_thread(chunk_system_t *cs, chunk_system_thread_args_t *targs)
{
    cs->running = true;
    int res = thrd_create(&cs->worker, _thread_func, targs);
    if (res != thrd_success)
    {
        ENGINE_LOG_ERROR("Failed to initialize worker thread. Aborting.\n", NULL);
        exit(1);
    }

    /* Block until the thread is ready. */
    while (!atomic_load(&cs->thread_ready))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}

void chunk_sys_cleanup(chunk_system_t *cs)
{
    atomic_store(&cs->running, false);
    cnd_signal(&cs->needs_update);
    thrd_join(cs->worker, NULL);

    cs->genned->destroy(cs->genned);
    cs->requests->destroy(cs->requests);

    cnd_destroy(&cs->needs_update);
    mtx_destroy(&cs->requests_lock);
    mtx_destroy(&cs->accumulator_lock);
}

void chunk_sys_make_request(chunk_system_t *cs, cs_request_t r)
{
    mtx_lock(&cs->requests_lock);

    cs->requests->enqueue(cs->requests, r);
    cnd_signal(&cs->needs_update);

    mtx_unlock(&cs->requests_lock);
}
