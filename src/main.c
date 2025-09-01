#if defined(TEST)
#include <libem/em_impl.h>
#define MY_HASHMAP_IMPL
#include "hashmap.h"
#define MY_DLL_IMPL #include "list.h"

#define TEST_IMPL
#include "test.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    test_main();
    return 0;
}

#else

/* Provide all definitions for sokol in main for simplicity. */
#define SOKOL_IMPL
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_app.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>

/* Providing all definitions for libem also. */
#include <libem/em_impl.h>

#define MY_HASHMAP_IMPL
#include "hashmap.h"

#define MY_DLL_IMPL
#include "list.h"

#include "chunk_loader.h"
#include "shaders/chunk.glsl.h"

static state_t state = {0};

/*
 * BUG: When there is a missing chunk, there is doubled data in the buffers at the first 
 *      2 indices in the table (when iterating through idx0 and idx1 are equal) FIND THE CAUSE!
 *
 *      - Could this be caused by a dangling _next pointer inside of the freed removed chunks 
 *        that damages entries still in the hashmap?
 *       
 */
static void dump_offsets(void)
{
    FILE *f_ptr = fopen("offset_dump.txt", "w");
    if (!f_ptr)
    {
        printf("Offset dump failed.\n");
        return;
    }

    HASHMAP(ivec2_chunk) *m_hot = state.buckets[BUCKET_HOT].chunks;
    em_hashmap_iter_t *it = m_hot->iterator(m_hot);
    size_t ctr = 0;
    while (it->has_next)
    {
        em_hashmap_entry_t *e = it->get(it);
        chunk_t *c = e->val;
        fprintf(f_ptr, "idx:%zu bkt:%zu | pos: %i,%i v: %zu, i: %zu\n", 
                ctr++, it->_idx, c->x, c->z, c->offsets.v_ofst, c->offsets.i_ofst);
        it->next(it);
    }
    fclose(f_ptr);
    printf("Dumped offsets.\n");
}

static void upload_stage(void)
{
    sg_update_buffer(state.cb.vbo, &(sg_range) {
        .ptr = state.cb.v_stg,
        .size = V_STG_SIZE
    });

    sg_update_buffer(state.cb.ibo, &(sg_range) {
        .ptr = state.cb.i_stg,
        .size = I_STG_SIZE
    });

    state.bind.vertex_buffers[0] = state.cb.vbo;
    state.bind.index_buffer = state.cb.ibo;
}

static bool dda_block_cast(chunk_t **hit_chunk, uvec3 *hit_block)
{
    const size_t MAX_STEPS = 10;

    vec3 rd = state.cam.fwd;
    vec3 ro = state.cam.pos;

    HASHMAP(ivec2_chunk) *m_hot = state.buckets[BUCKET_HOT].chunks;
    chunk_t *curr_chunk = m_hot->get_ptr(m_hot, (ivec2) {floorf(ro.x / 16) * 16, 
                                                          floorf(ro.z / 16) * 16});
    vec3 step = {
        .x = (rd.x < 0) ? -1 : 1,
        .y = (rd.y < 0) ? -1 : 1,
        .z = (rd.z < 0) ? -1 : 1
    };
    vec3 rust = {
        .x = 1.0 / em_abs(rd.x),
        .y = 1.0 / em_abs(rd.y),
        .z = 1.0 / em_abs(rd.z)
    };
    uvec3 cell = {
        .x = floorf(ro.x) - curr_chunk->x,
        .y = floorf(ro.y),
        .z = floorf(ro.z) - curr_chunk->z
    };
    vec3 ro_fract = {
        .x = ro.x - floorf(ro.x),
        .y = ro.y - floorf(ro.y),
        .z = ro.z - floorf(ro.z)
    };
    vec3 len = {
        .x = (rd.x < 0) ? ro_fract.x * rust.x : (1.0 - ro_fract.x) * rust.x,
        .y = (rd.y < 0) ? ro_fract.y * rust.y : (1.0 - ro_fract.y) * rust.y,
        .z = (rd.z < 0) ? ro_fract.z * rust.z : (1.0 - ro_fract.z) * rust.z,
    };

    for (size_t i = 0; i < MAX_STEPS; i++)
    {
        cube_type_e type = curr_chunk->blocks->types[cell.x][cell.y][cell.z];
        if (type != CUBETYPE_AIR)
        {
            *hit_chunk = curr_chunk;
            *hit_block = cell;
            return true;
        }

        if (len.x < len.y && len.x < len.z) // x is smallest
        {
            len.x += rust.x;

            if (cell.x == CHUNK_SIZE - 1 && step.x == 1)
            {
                cell.x = 0;
                ivec2 next_pos = {
                    curr_chunk->x + CHUNK_SIZE,
                    curr_chunk->z
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else if (cell.x == 0 && step.x == -1)
            {
                cell.x = CHUNK_SIZE - 1;
                ivec2 next_pos = {
                    curr_chunk->x - CHUNK_SIZE,
                    curr_chunk->z
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else 
            {
                cell.x += step.x;
            }
        }
        else if (len.y < len.z) // y is smallest
        {
            len.y += rust.y;

            if ((cell.y == CHUNK_HEIGHT - 1 && step.y == 1)
               || (cell.y == 0 && step.y == -1))
                return false;
            else 
                cell.y += step.y;
        }
        else // z is smallest
        {
            len.z += rust.z;

            if (cell.z == CHUNK_SIZE - 1 && step.z == 1)
            {
                cell.z = 0;
                ivec2 next_pos = {
                    curr_chunk->x,
                    curr_chunk->z + CHUNK_SIZE
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else if (cell.z == 0 && step.z == -1)
            {
                cell.z = CHUNK_SIZE - 1;
                ivec2 next_pos = {
                    curr_chunk->x,
                    curr_chunk->z - CHUNK_SIZE
                };
                curr_chunk = m_hot->get_ptr(m_hot, next_pos);
            }
            else 
            {
                cell.z += step.z;
            }
        }
    }
    return false;
}

static void destroy_block(void)
{
    chunk_t *chunk = NULL;
    uvec3 block = {0};

    if (!dda_block_cast(&chunk, &block))
        return;

    chunk_t *remesh[3];
    size_t remesh_cnt = 0;

    chunk->blocks->types[block.x][block.y][block.z] = CUBETYPE_AIR;
    remesh[remesh_cnt++] = chunk;

    /* 
     * If the destroyed block is on a chunk boundary then the adjacent chunk 
     * needs to be remeshed to avoid holes in the mesh.
     */
    HASHMAP(ivec2_chunk) *m_hot = state.buckets[BUCKET_HOT].chunks;

    switch (block.x) 
    {
        case 0: 
        {
            ivec2 adj_crd = {chunk->x - CHUNK_SIZE, chunk->z};
            remesh[remesh_cnt++] = m_hot->get_ptr(m_hot, adj_crd);
            break;
        }
        case CHUNK_SIZE - 1: 
        {
            ivec2 adj_crd = {chunk->x + CHUNK_SIZE, chunk->z};
            remesh[remesh_cnt++] = m_hot->get_ptr(m_hot, adj_crd);
            break;
        }
        default:
            break;
    }

    switch (block.z) 
    {
        case 0: 
        {
            ivec2 adj_crd = {chunk->x, chunk->z - CHUNK_SIZE};
            remesh[remesh_cnt++] = m_hot->get_ptr(m_hot, adj_crd);
            break;
        }
        case CHUNK_SIZE - 1: 
        {
            ivec2 adj_crd = {chunk->x, chunk->z + CHUNK_SIZE};
            remesh[remesh_cnt++] = m_hot->get_ptr(m_hot, adj_crd);
            break;
        }
        default:
            break;
    }

    for (size_t i = 0; i < remesh_cnt; i++)
    {
        chunk_t *c = remesh[i];
        ivec2 crd = {c->x, c->z};
        
        chunk_t *n = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0,  16}));
        chunk_t *e = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) { 16,   0}));
        chunk_t *s = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {  0, -16}));
        chunk_t *w = m_hot->get_ptr(m_hot, em_add_ivec2(crd, (ivec2) {-16,   0}));

        mesh_t *m = geom_generate_full_mesh(c->blocks, n->blocks, e->blocks, s->blocks, w->blocks);
        restage_chunk(&state, c, m);
    }

    state.needs_update = true;
}

static void create_block(void)
{
    printf("create\n");
}

static void init(void)
{
    sg_setup(&(sg_desc) {
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    state_init_pipeline(&state);
    state_init_bindings(&state);
    state_init_textures(&state);
    state_init_cam(&state);
    state_init_chunk_buffer(&state);
    state_init_data(&state);

    load_initial_chunks(&state);
    upload_stage();
    state.needs_update = false;

    HASHMAP(ivec2_chunk) *m_hot = state.buckets[BUCKET_HOT].chunks;
    state.curr_chunk = m_hot->get_ptr(m_hot, (ivec2) {  0,   0});
    state.curr_north = m_hot->get_ptr(m_hot, (ivec2) {  0,  16});
    state.curr_east  = m_hot->get_ptr(m_hot, (ivec2) { 16,   0});
    state.curr_south = m_hot->get_ptr(m_hot, (ivec2) {  0, -16});
    state.curr_west  = m_hot->get_ptr(m_hot, (ivec2) {-16,   0});
}

static void render(void)
{
    sg_begin_pass(&(sg_pass) {
        .action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                // .clear_value = {0.25, 0.5, 0.75, 1.0}
                .clear_value = {0.35, 0.6, 0.85, 1.0}
            }
        },
        .swapchain = sglue_swapchain()
    });

    sg_apply_pipeline(state.pip);

    /* All chunks in the hot bucket are actively in use. */
    bucket_t tmp = state.buckets[BUCKET_HOT];
    em_hashmap_iter_t *it = tmp.chunks->iterator(tmp.chunks);
    while (it->has_next)
    {
        chunk_t *c = it->get(it)->val;

        vs_params_t vs_params = {
            .u_mvp = state.cam.vp,
            .u_v = state.cam.view,
            .u_chnk_pos = {
                (float) c->x,
                0.0,
                (float) c->z
            }
        };

        fs_params_t fs_params = {
            // .u_fog_data = {0.25, 0.5, 0.75, state.cam.far}
            .u_fog_data = {0.35, 0.6, 0.85, state.cam.far}
        };

        sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
        sg_apply_uniforms(UB_fs_params, &SG_RANGE(fs_params));

        state.bind.index_buffer_offset = c->offsets.i_ofst * sizeof(uint32_t);
        state.bind.vertex_buffer_offsets[0] = c->offsets.v_ofst * sizeof(vertex_t);

        sg_apply_bindings(&state.bind);
        sg_draw(0, c->index_cnt, 1);

        it->next(it);
    }

    sg_end_pass();
    sg_commit();

    free(it);
}

static void l_tick(void)
{
    cleanup_old_chunks(&state);
    // BUG: Used for debugging chunk buffer doubling issue.
    // printf("%i %i | Hot %zu, Cold %zu, Free %zu\n", 
    //        (int) floorf(state.cam.pos.x / 16) * 16, (int) floorf(state.cam.pos.z / 16) * 16, 
    //        state.buckets[BUCKET_HOT].chunks->count,
    //        state.buckets[BUCKET_COLD].chunks->count, state.free_list->count);
}

static void tick(void)
{
    if (state.left_click)
    {
        destroy_block();
        state.left_click = false;
    }

    if (state.right_click)
    {
        create_block();
        state.right_click = false;
    }

    if (state.needs_update)
    {
        upload_stage();
        state.needs_update = false;
    }

    /* Check if player has entered new chunk and generate new chunks if they have. */
    ivec2 curr_pos = {
        floor(state.cam.pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE,
        floor(state.cam.pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };
    ivec2 curr_chunk_pos = {
        state.curr_chunk->x,
        state.curr_chunk->z 
    };
    if (!em_equals_ivec2(curr_pos, curr_chunk_pos)) 
    {
        load_chunks(&state);
        state.needs_update = true;

        HASHMAP(ivec2_chunk) *m_hot = state.buckets[BUCKET_HOT].chunks;
        state.curr_chunk = m_hot->get_ptr(m_hot, curr_pos);

        ivec2 n_pos = em_add_ivec2(curr_pos, (ivec2) {  0,  16});
        ivec2 e_pos = em_add_ivec2(curr_pos, (ivec2) { 16,   0});
        ivec2 s_pos = em_add_ivec2(curr_pos, (ivec2) {  0, -16});
        ivec2 w_pos = em_add_ivec2(curr_pos, (ivec2) {-16,   0});

        state.curr_north = m_hot->get_or_default(m_hot, n_pos, NULL);
        state.curr_east  = m_hot->get_or_default(m_hot, e_pos, NULL);
        state.curr_south = m_hot->get_or_default(m_hot, s_pos, NULL);
        state.curr_west  = m_hot->get_or_default(m_hot, w_pos, NULL);
    }

}

static void frame(void)
{
    state.frame++; // Don't worry, this lasts 9 billion years at 60fps before overflow.
    state.tick++;
    state.l_tick++;

    if (state.l_tick == 30)
    {
        l_tick();
        state.l_tick = 0;
    }

    if (state.tick == 5) 
    {
        tick();
        state.tick = 0;
    }

    /* Apply player inputs. */
    cam_frame(&(frame_desc_t) {
        .cam      = &state.cam,
        .lmb      = &state.left_click,
        .rmb      = &state.right_click,
        .mouse_dx = &state.mouse_dx,
        .mouse_dy = &state.mouse_dy,
        .key_down = state.key_down,
        .curr     = state.curr_chunk,
        .north    = state.curr_north,
        .east     = state.curr_east,
        .south    = state.curr_south,
        .west     = state.curr_west,
        .dt       = sapp_frame_duration()
    });

    render();
}

static void cleanup(void)
{
    // TODO: Need to destroy each chunk internally too, pass in a destroy func for k and v on init.
    for (size_t i = 0; i < NUM_BUCKETS; i++) 
        state.buckets[i].chunks->destroy(state.buckets[i].chunks);

    sg_destroy_buffer(state.cb.vbo);
    sg_destroy_buffer(state.cb.ibo);

    sg_shutdown();
}

static void event(const sapp_event *event)
{
    switch (event->type) {
    case SAPP_EVENTTYPE_KEY_UP:
        state.key_down[event->key_code] = false;
        break;

    case SAPP_EVENTTYPE_KEY_DOWN:
        state.key_down[event->key_code] = true;
        if (event->key_code == SAPP_KEYCODE_CAPS_LOCK) // I remapped caps lock to esc
            sapp_lock_mouse(false);

        if (event->key_code == SAPP_KEYCODE_RIGHT_ALT)
            dump_offsets();
        break;

    case SAPP_EVENTTYPE_MOUSE_MOVE:
        state.mouse_dx += event->mouse_dx;
        state.mouse_dy += event->mouse_dy;
        break;

    case SAPP_EVENTTYPE_MOUSE_DOWN:
        sapp_lock_mouse(true);
        if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT)
            state.left_click = true;
        else if (event->mouse_button == SAPP_MOUSEBUTTON_RIGHT)
            state.right_click = true;
        break;

    default:
        break;

    }
}

sapp_desc sokol_main(int argc, char* argv[])
{
    /* Not accepting cmdline args, this avoid the compiler warning. */
    (void) argc;
    (void) argv;

    return (sapp_desc) {
        .init_cb            = init,
        .frame_cb           = frame,
        .cleanup_cb         = cleanup,
        .event_cb           = event,
        .logger.func        = slog_func,
        .width              = 1280,
        .height             = 720,
        // .width              = 1920,
        // .height             = 1080,
        .sample_count       = 1,
        .window_title       = "Minecraft Remake",
        .icon.sokol_default = true
    };
}

#endif // TEST
