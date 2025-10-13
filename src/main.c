#if defined(TEST)

#include <libem/em_impl.h>

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
#include "dlist.h"
#define MY_CQ_IMPL
#include "queue.h"
#define MY_AL_IMPL
#include "alist.h"

#include "file_handler.h"
#include "raycast.h"
#include "main_menu.h"
#include "logger.h"
#include "state.h"

#include "chunk.glsl.h"

static state_t state = {0};

static void destroy_block(void)
{
    // TODO
}

static void create_block(void)
{
    // TODO
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

    state.render_dist = 8;
    // state.render_dist = 3;
    state.frame = 0;
    state.tick = 0;
    state.l_tick = 0;

    state_init_pipeline(&state);
    state_init_bindings(&state);
    state_init_textures(&state);
    state_init_cam(&state);

    state.game_desc = (game_desc_t) {
        .game_state = GAME_MENUMAIN,
        .popup = POPUP_NONE,
        .selected_str = {0},
        .world_str = {0},
        .seed_str = {0},
        .window = {
            .x = sapp_width(), 
            .y = sapp_height()
        }
    };
}

static void save_cb(void)
{
    // TODO
}

static void init_cb(bool loaded)
{
    state.game_desc.game_state = GAME_RUN;

    if (loaded)
    {
        // TODO
        load_data_t *ld = file_get_world_data(state.game_desc.selected_str);
        state.game_desc.seed = ld->seed;
        free(ld);
    }
    else 
    {
        uint32_t seed = strtoul(state.game_desc.seed_str, NULL, 16); // hexadecimal
        state.game_desc.seed = seed;
        file_make_world_desc(state.game_desc);
    }

    state_init_systems(&state);
    manager_init(&state.lm, &(load_manager_desc_t) {
        .start_pos = (ivec2) {0, 0},
        .load_dist = state.render_dist + 2
    });

    // state.bind.vertex_buffers[0] = state.cs.front->buffers.vbo;
    // state.bind.index_buffer = state.cs.front->buffers.ibo;

    // LOG_OK("MAIN", "Waiting for thread init.\n", NULL);

    // mtx_lock(&state.cs.back->init_lock);
    // cnd_wait(&state.cs.back->thread_initialized, &state.cs.back->init_lock);

    // LOG_OK("MAIN", "Dispatching initial requests.\n", NULL);

    // manager_load_initial(&state.lm, &state.cs);
    // mtx_unlock(&state.cs.back->init_lock);
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

static void dump_buffers()
{
    // FILE *if_ptr = fopen("IBUF.txt", "w");
    // for (size_t i = 0; i < 10000; i++)
    //     fprintf(if_ptr, "%zu| %u\n", i, state.cs.front->buffers.i_stage[i]);
    // fclose(if_ptr);
    //
    // FILE *vf_ptr = fopen("VBUF.txt", "w");
    // for (size_t i = 0; i < 10000; i++)
    // {
    //     vertex_t v = state.cs.front->buffers.v_stage[i];
    //     fprintf(vf_ptr, "%zu| xyz {%i, %i, %i}\n", i, v.x, v.y, v.z);
    // }
    // fclose(vf_ptr);
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

    /* Render all visible chunks. */
    // em_double_list_iter_t *it = state.cs.front->visible->iterator(state.cs.front->visible);
    // while (it->has_next)
    // {
    //     em_double_list_node_t *n = it->get(it);
    //     it->next(it);
    //
    //     ivec2 crd = *(ivec2 *) n->val;
    //     chunk_render_info_t *cri = state.cs.front->chunks->get_or_default(state.cs.front->chunks, 
    //                                                                       crd, NULL);
    //
    //     if (!cri)
    //     {
    //         state.cs.front->visible->remove_node(state.cs.front->visible, n);
    //         continue;
    //     }
    //
    //     if (!cri->visible)
    //         continue;
    //
    //     // if (!done)
    //     // {
    //     //     dump_buffers();
    //     //     done = true;
    //     // }
    //
    //     vs_params_t vs_params = {
    //         .u_mvp = state.cam.vp,
    //         .u_v = state.cam.view,
    //         .u_chnk_pos = {
    //             (float) cri->pos.x,
    //             0.0,
    //             (float) cri->pos.y
    //         }
    //     };
    //
    //     fs_params_t fs_params = {
    //         .u_fog_data = {0.35, 0.6, 0.85, state.cam.far}
    //     };
    //
    //     sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
    //     sg_apply_uniforms(UB_fs_params, &SG_RANGE(fs_params));
    //
    //     state.bind.vertex_buffer_offsets[0] = cri->offset.v_ofst * sizeof(vertex_t);
    //     state.bind.index_buffer_offset = cri->offset.i_ofst * sizeof(uint32_t);
    //
    //     sg_apply_bindings(&state.bind);
    //     sg_draw(0, cri->index_cnt, 1);
    // }
    // free(it);

    sg_end_pass();
    sg_commit();
}

static void l_tick(void)
{
}

static void tick(void)
{
    // TODO: Experiment with where to call the pulls (tick, frame, ltick)
    //       FINALLY JUST PROFILE IT TO SEE WHY ITS SLOW!!!

    /* Pull in new chunks from the loader. */
    const size_t PULLS_PER_FRAME = 1;
    // bool did_pull = cs_pull_n_changes(state.cs.front, state.cs.back, PULLS_PER_FRAME);
    // if (did_pull)
    // {
    //     LOG_OK("MAIN", "Updating.\n", NULL);
    //     cs_upload_stages(state.cs.front); // TODO: Don't need to call this so often.
    //     // state.bind.vertex_buffers[0] = state.cs.front->buffers.vbo;
    //     // state.bind.index_buffer = state.cs.front->buffers.ibo;
    // }

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

    /* See if the player has moved to another chunk and load their chunks if they have. */
    ivec2 curr_pos = {
        floorf(state.cam.pos.x / (float) CHUNK_SIZE) * CHUNK_SIZE,
        floorf(state.cam.pos.z / (float) CHUNK_SIZE) * CHUNK_SIZE
    };

    manager_update(&state.lm, &state.cs, curr_pos);
}

static void frame(void)
{
    switch (state.game_desc.game_state) {
    case GAME_MENUMAIN:
    case GAME_MENULOAD:
    case GAME_MENUNEW:
    case GAME_MENUOPT:
    case GAME_PAUSE:
    {
        struct nk_context *ctx = snk_new_frame();

        state.game_desc.window = (uvec2) {
            sapp_width(),
            sapp_height()
        };

        // menu_draw(&state.game_desc, init_cb, save_cb, ctx);
        render_menu();
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
            .dt       = sapp_frame_duration()
        });

        render();

        break;
    }
    default:
        break;
    };
}

static void cleanup(void)
{
    cs_cleanup(&state.cs);
    manager_cleanup(&state.lm);

    sg_shutdown();
}

static void event(const sapp_event *event)
{
    if (state.game_desc.game_state == GAME_RUN)
    {
        switch (event->type) {
        case SAPP_EVENTTYPE_KEY_UP:
            state.key_down[event->key_code] = false;
            break;

        case SAPP_EVENTTYPE_KEY_DOWN:
            state.key_down[event->key_code] = true;
            if (event->key_code == SAPP_KEYCODE_ESCAPE)
            {
                state.game_desc.game_state = GAME_PAUSE;
                sapp_lock_mouse(false);
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_MOVE:
            state.mouse_dx += event->mouse_dx;
            state.mouse_dy += event->mouse_dy;
            break;

        case SAPP_EVENTTYPE_MOUSE_DOWN:
            sapp_lock_mouse(true);

            switch (event->mouse_button) {
            case SAPP_MOUSEBUTTON_LEFT:
                state.left_click = true;
                break;

            case SAPP_MOUSEBUTTON_RIGHT:
                state.right_click = true;
                break;

            default:
                break;
            };

            break;

        default:
            break;

        }
    }
    else if (state.game_desc.game_state == GAME_PAUSE)
    {
        if (event->type == SAPP_EVENTTYPE_KEY_DOWN && event->key_code == SAPP_KEYCODE_ESCAPE)
            state.game_desc.game_state = GAME_RUN;

        // snk_handle_event(event);
    }
    else 
    {
        // snk_handle_event(event);
    }
}

// int main(int argc, char **argv)
// {
//     (void) argc;
//     (void) argv;
//     printf("Hello, Build!\n");
// }

sapp_desc sokol_main(int argc, char* argv[])
{
    /* Not accepting cmdline args, this avoid the compiler warning. */
    (void) argc;
    (void) argv;

    printf("Hello sokol\n");

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
