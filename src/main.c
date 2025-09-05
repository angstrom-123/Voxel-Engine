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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <sokol/sokol_nuklear.h>

/* Providing all definitions for libem also. */
#include <libem/em_impl.h>

#define MY_HASHMAP_IMPL
#include "hashmap.h"

#define MY_DLL_IMPL
#include "list.h"

#include "chunk_loader.h"
#include "file_handler.h"
#include "raycast.h"
#include "menu.h"

#include "shaders/chunk.glsl.h"

static state_t state = {0};

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

static void destroy_block(void)
{
    chunk_t *chunk = NULL;
    uvec3 block = {0};
    if (!raycast_dda(&state, &chunk, &block))
        return;

    chunk_t *remesh[3];
    size_t remesh_cnt = 0;

    chunk->blocks->types[block.x][block.y][block.z] = CUBETYPE_AIR;
    chunk->edited = true;
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

    snk_setup(&(snk_desc_t) {
        .enable_set_mouse_cursor = false,
        .dpi_scale = sapp_dpi_scale(),
        .logger.func = slog_func
    });

    state_init_pipeline(&state);
    state_init_bindings(&state);
    state_init_textures(&state);
    state_init_cam(&state);
    state_init_chunk_buffer(&state);
    state_init_data(&state);

    state.game_state = GAME_MENU;
    state.menu_desc.state = MENU_MAIN;

    // load_initial_chunks(&state);
    // upload_stage();

    // state.needs_update = false;
    // state.world_name = "Test_World"; // TODO: Select name / load worlds.
    // state.dir_name = file_make_world_dir(state.world_name);
    // state.seed = 0; // TODO: Select a seed.
    //
    // HASHMAP(ivec2_chunk) *m_hot = state.buckets[BUCKET_HOT].chunks;
    // state.curr_chunk = m_hot->get_ptr(m_hot, (ivec2) {  0,   0});
    // state.curr_north = m_hot->get_ptr(m_hot, (ivec2) {  0,  16});
    // state.curr_east  = m_hot->get_ptr(m_hot, (ivec2) { 16,   0});
    // state.curr_south = m_hot->get_ptr(m_hot, (ivec2) {  0, -16});
    // state.curr_west  = m_hot->get_ptr(m_hot, (ivec2) {-16,   0});
}

static void init_world(void)
{
    state.dir_name = file_make_world_dir(state.menu_desc.selected_name);
    state.seed = strtoul(state.menu_desc.seed_buf, NULL, 16); // hexadecimal

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

static void render_menu(void)
{
    sg_begin_pass(&(sg_pass) {
        .action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = {0.35, 0.6, 0.85, 1.0}
            }
        },
        .swapchain = sglue_swapchain()
    });

    snk_render(sapp_width(), sapp_height());

    sg_end_pass();
    sg_commit();
}

static void render(void)
{
    sg_begin_pass(&(sg_pass) {
        .action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = {0.35, 0.6, 0.85, 1.0}
            }
        },
        .swapchain = sglue_swapchain()
    });

    sg_apply_pipeline(state.pip);

    /* All chunks in the hot bucket are actively in use. */
    bucket_t b_hot = state.buckets[BUCKET_HOT];
    em_hashmap_iter_t *it = b_hot.chunks->iterator(b_hot.chunks);
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
    switch (state.game_state) {
    case GAME_MENU:
    {
        struct nk_context *ctx = snk_new_frame();

        state.menu_desc.win_width = sapp_width();
        state.menu_desc.win_height = sapp_height();

        menu_draw(&state.menu_desc, ctx);
        render_menu();

        if (state.menu_desc.state == MENU_PLAY)
        {
            state.game_state = GAME_RUN;
            init_world();
        }

        break;
    }
    case GAME_RUN:
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

        break;
    }
    case GAME_PAUSE:
    {
        // TODO
        break;
    }
    default:
        break;
    };
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
    if (state.game_state == GAME_RUN)
    {
        switch (event->type) {
        case SAPP_EVENTTYPE_KEY_UP:
            state.key_down[event->key_code] = false;
            break;

        case SAPP_EVENTTYPE_KEY_DOWN:
            state.key_down[event->key_code] = true;
            if (event->key_code == SAPP_KEYCODE_CAPS_LOCK) // I remapped caps lock to esc
                sapp_lock_mouse(false);
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
    else 
    {
        snk_handle_event(event);
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
