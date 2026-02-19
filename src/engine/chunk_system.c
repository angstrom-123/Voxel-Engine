#include "chunk_system.h"

static void _handle_request(chunk_system_t *cs, update_system_t *us, cs_request_t *r)
{
    switch (r->type) {
    case CSREQ_LOAD:
    {
        INSTRUMENT_SCOPE_BEGIN(req_load);
        if (cs->world_dir_path[0] == '\0') // Temporary world: Doesn't save / load chunks.
        {
            chunk_data_t *d = cs->gen_func(r->pos, cs->seed);
            mtx_lock(&cs->genned_lock);
            cs->genned->put_ptr(cs->genned, r->pos, d);
            mtx_unlock(&cs->genned_lock);
            break;
        }

        char file_name[STD_BUFLEN] = {0};
        char file_path[STD_BUFLEN] = {0};
        chunk_file_name(r->pos, file_name);
        multicat(file_path, 3, cs->world_dir_path, SEP, file_name);
        file_t f = {
            .name = file_name,
            .base = cs->world_dir_path,
            .path = file_path,
            .flags = FILEFLAG_BINARY,
        };

        chunk_data_t *d;
        if (file_exists(&f))
        {
            ENGINE_LOG_OK("Decoding a chunk file.", NULL);
            d = decode_chunk_file(&f);
        }
        else 
        {
            d = cs->gen_func(r->pos, cs->seed);
        }

        mtx_lock(&cs->genned_lock);
        cs->genned->put_ptr(cs->genned, r->pos, d);
        mtx_unlock(&cs->genned_lock);
        INSTRUMENT_SCOPE_END(req_load);
        break;
    }
    case CSREQ_MESH:
    case CSREQ_REMESH:
    {
        INSTRUMENT_SCOPE_BEGIN(req_mesh_remesh);
        mtx_lock(&cs->genned_lock);
        chunk_set_t chunks = {
            .cd = cs->genned->get_or_default(cs->genned, r->pos, NULL),
            .nd = cs->genned->get_or_default(cs->genned, REL_N(r->pos), NULL),
            .ed = cs->genned->get_or_default(cs->genned, REL_E(r->pos), NULL),
            .sd = cs->genned->get_or_default(cs->genned, REL_S(r->pos), NULL),
            .wd = cs->genned->get_or_default(cs->genned, REL_W(r->pos), NULL)
        };
        mtx_unlock(&cs->genned_lock);

        ENGINE_ASSERT(chunks.cd != NULL, "Chunk to be meshed doesn't exist.\n");
        ENGINE_ASSERT(chunks.nd != NULL, "North of the chunk to mesh doesn't exist.\n");
        ENGINE_ASSERT(chunks.ed != NULL, "East of the chunk to mesh doesn't exist.\n");
        ENGINE_ASSERT(chunks.sd != NULL, "South of the chunk to mesh doesn't exist.\n");
        ENGINE_ASSERT(chunks.wd != NULL, "West of the chunk to mesh doesn't exist.\n");
        
        mesh_t *mesh_o = NULL; // Opaque geometry for deferred rendering
        geom_generate_mesh(&mesh_o, chunks);

        if (r->type == CSREQ_MESH) US_REQUEST(us, USREQ_STAGE, r->pos, mesh_o);
        else                       US_REQUEST(us, USREQ_RESTAGE, r->pos, mesh_o);
        INSTRUMENT_SCOPE_END(req_mesh_remesh);
        break;
    }
    case CSREQ_UNLOAD: 
    {
        INSTRUMENT_SCOPE_BEGIN(req_unload);
        mtx_lock(&cs->genned_lock);
        chunk_data_t *cd = cs->genned->get_or_default(cs->genned, r->pos, NULL);
        mtx_unlock(&cs->genned_lock);

        if (!cd)
            break;

        if (cd->edited && cs->world_dir_path[0] != '\0')
        {
            char file_name[STD_BUFLEN] = {0};
            char file_path[STD_BUFLEN] = {0};
            chunk_file_name(r->pos, file_name);
            multicat(file_path, 3, cs->world_dir_path, SEP, file_name);
            file_t f = {
                .flags = FILEFLAG_BINARY,
                .base = cs->world_dir_path,
                .path = file_path,
                .name = file_name,
            };

            if (file_exists(&f))
                RUNTIME_ASSERT(file_delete(&f), "Failed to delete chunk file");

            chunk_file_t cf = encode_chunk_file(cd);
            RUNTIME_ASSERT(file_open(&f, USAGE_WRITE_BIN), "Failed to open chunk file");
            RUNTIME_ASSERT(write_chunk_file(&f, cf), "Failed to write to chunk file");
            RUNTIME_ASSERT(file_close(&f), "Failed to close chunk file");

            free(cf.data);
        }

        mtx_lock(&cs->genned_lock);
        cs->genned->remove(cs->genned, r->pos);
        mtx_unlock(&cs->genned_lock);
        INSTRUMENT_SCOPE_END(req_unload);
        break;
    }
    case CSREQ_BREAK:
    {
        INSTRUMENT_SCOPE_BEGIN(req_break);
        mtx_lock(&cs->genned_lock);
        chunk_data_t *c = cs->genned->get_or_default(cs->genned, r->pos, NULL);
        c->types[r->cell.x][r->cell.y][r->cell.z] = CUBETYPE_AIR;
        c->edited = true;
        mtx_unlock(&cs->genned_lock);

        #define REMESH_AT(at) _handle_request(cs, us, &(cs_request_t) {\
            .type = CSREQ_REMESH, .pos = at })

        REMESH_AT(r->pos);
        if (r->cell.z == CHUNK_SIZE - 1) REMESH_AT(REL_N(r->pos));
        if (r->cell.x == CHUNK_SIZE - 1) REMESH_AT(REL_E(r->pos));
        else if (r->cell.z == 0)         REMESH_AT(REL_S(r->pos));
        else if (r->cell.x == 0)         REMESH_AT(REL_W(r->pos));

        INSTRUMENT_SCOPE_END(req_break);
        break;
    }
    case CSREQ_PLACE:
    {
        INSTRUMENT_SCOPE_BEGIN(req_place);
        mtx_lock(&cs->genned_lock);
        chunk_data_t *c = cs->genned->get_or_default(cs->genned, r->pos, NULL);
        c->types[r->cell.x][r->cell.y][r->cell.z] = r->block;
        c->edited = true;
        mtx_unlock(&cs->genned_lock);

        _handle_request(cs, us, &(cs_request_t) {
            .type = CSREQ_REMESH,
            .pos = r->pos
        });

        INSTRUMENT_SCOPE_END(req_place);
        break;
    }
    case CSREQ_INITIAL_LOAD_COMPLETE:
        atomic_store(&cs->initial_load_complete, true);
        break;
    }
}

static int _thread_func(void *args)
{
    chunk_system_thread_args_t *targs = args;
    chunk_system_t *cs = targs->cs;
    update_system_t *us = targs->us;

    ENGINE_LOG_OK("Chunk thread init.\n", NULL);

    while (atomic_load(&cs->running))
    {
        mtx_lock(&cs->requests_lock);
        atomic_store(&cs->thread_ready, true);
        cnd_wait(&cs->needs_update, &cs->requests_lock);

        while (cs->requests->count > 0)
        {
            atomic_store(&cs->processing, true);
            cs_request_t *r = cs->requests->dequeue_ptr(cs->requests);
            _handle_request(cs, us, r);
            free(r);
            atomic_store(&cs->processing, false);
        }

        mtx_unlock(&cs->requests_lock);
    }

    ENGINE_LOG_WARN("Generation thread terminating.\n", NULL);

    return 0;
}

void chunk_sys_init(chunk_system_t *cs, const chunk_system_desc_t *desc)
{
    cs->seed                  = desc->seed;
    cs->gen_func              = desc->gen_func;
    cs->running               = false;
    cs->thread_ready          = false;
    cs->initial_load_complete = false;
    cs->receiving             = false;

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
        .flags = EM_FLAG_NO_RESIZE
    });

    mtx_init(&cs->requests_lock, mtx_plain);
    mtx_init(&cs->genned_lock, mtx_plain);
    cnd_init(&cs->needs_update);
}

void chunk_sys_init_thread(chunk_system_t *cs, chunk_system_thread_args_t *targs)
{
    cs->running = true;
    cs->receiving = true;
    int res = thrd_create(&cs->worker, _thread_func, targs);

    ENGINE_ASSERT(res == thrd_success, "Failed to initialize worker thread.\n");

    /* Block until the thread is ready. */
    while (!atomic_load(&cs->thread_ready))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}

static void _await_requests_complete(chunk_system_t *cs)
{
    bool finished = false;
    while (!finished)
    {
        mtx_lock(&cs->requests_lock);

        if (cs->requests->count == 0 && !atomic_load(&cs->processing))
            finished = true;

        mtx_unlock(&cs->requests_lock);

        if (!finished)
        {
            thrd_sleep(&(struct timespec) {
                .tv_sec = THREAD_AWAIT_S,
                .tv_nsec = THREAD_AWAIT_NS
            }, NULL);
        }

    }
}

void chunk_sys_cleanup(chunk_system_t *cs)
{
    // Complete all requests in backlog but reveive no more.
    cs->receiving = false;
    _await_requests_complete(cs);

    // Request every loaded chunk to be unloaded directly (bypassing request denial).
    cs->receiving = false;
    mtx_lock(&cs->genned_lock);
    mtx_lock(&cs->requests_lock);
    em_hashmap_iter_t *it = cs->genned->iterator(cs->genned);
    while (it->has_next)
    {
        em_hashmap_entry_t *e = it->get(it);
        ivec2 *pos = e->key;
        cs->requests->enqueue(cs->requests, (cs_request_t) {
            .type = CSREQ_UNLOAD,
            .pos = *pos
        });
        it->next(it);
    }
    free(it);
    mtx_unlock(&cs->requests_lock);
    mtx_unlock(&cs->genned_lock);

    // Wait for all unload requests to be complete.
    cnd_signal(&cs->needs_update);
    _await_requests_complete(cs);
    cs->receiving = true;

    // Kill the worker.
    atomic_store(&cs->running, false);
    cnd_signal(&cs->needs_update);
    thrd_join(cs->worker, NULL);

    // Cleanup chunk system.
    cs->genned->destroy(cs->genned);
    cs->requests->destroy(cs->requests);
    cnd_destroy(&cs->needs_update);
    mtx_destroy(&cs->requests_lock);
    mtx_destroy(&cs->genned_lock);
}

void chunk_sys_unload_all(chunk_system_t *cs)
{
    mtx_lock(&cs->requests_lock);
    
    // Remove all pending requests
    cs->requests->clear(cs->requests);

    mtx_lock(&cs->genned_lock);

    // Request all chunks to be unloaded
    em_hashmap_iter_t *it = cs->genned->iterator(cs->genned);
    while (it->has_next)
    {
        em_hashmap_entry_t *e = it->get(it);
        ivec2 *pos = e->key;
        cs->requests->enqueue(cs->requests, (cs_request_t) {
            .type = CSREQ_UNLOAD,
            .pos = *pos
        });
        it->next(it);
    }
    free(it);

    atomic_store(&cs->initial_load_complete, false);

    mtx_unlock(&cs->genned_lock);
    mtx_unlock(&cs->requests_lock);

    // Wait for all unload requests to be complete.
    cnd_signal(&cs->needs_update);
}

void chunk_sys_make_request(chunk_system_t *cs, cs_request_t r)
{
    if (!cs->receiving) 
    {
        ENGINE_LOG_WARN("Chunk sys blocked request because shutting down.\n", NULL);
        return;
    }
    mtx_lock(&cs->requests_lock);

    cs->requests->enqueue(cs->requests, r);
    cnd_signal(&cs->needs_update);

    mtx_unlock(&cs->requests_lock);
}

void chunk_sys_borrow_surrounding_data(chunk_system_t *cs, ivec2 pos, chunk_set_t *chunks)
{
    INSTRUMENT_FUNC_BEGIN();
    if (!atomic_load(&cs->running)) return;

    mtx_lock(&cs->genned_lock);

    chunks->nwd = cs->genned->get_ptr(cs->genned, REL_NW(pos));
    chunks->nd  = cs->genned->get_ptr(cs->genned, REL_N(pos));
    chunks->ned = cs->genned->get_ptr(cs->genned, REL_NE(pos));
    chunks->wd  = cs->genned->get_ptr(cs->genned, REL_W(pos));
    chunks->cd  = cs->genned->get_ptr(cs->genned, pos);
    chunks->ed  = cs->genned->get_ptr(cs->genned, REL_E(pos));
    chunks->swd = cs->genned->get_ptr(cs->genned, REL_SW(pos));
    chunks->sd  = cs->genned->get_ptr(cs->genned, REL_S(pos));
    chunks->sed = cs->genned->get_ptr(cs->genned, REL_SE(pos));
    INSTRUMENT_FUNC_END();
}

void chunk_sys_return_surrounding_data(chunk_system_t *cs, chunk_set_t *chunks)
{
    (void) cs;
    (void) chunks;
    mtx_unlock(&cs->genned_lock);
}

void chunk_sys_await_initial_load_complete(chunk_system_t *cs)
{
    while (!atomic_load(&cs->initial_load_complete))
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
    }
}
