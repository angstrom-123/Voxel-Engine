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

#include "chunk_manager.h"
#include "shaders/chunk.glsl.h"

/*
 *
 *  BUG: At some times, cold and stale buffers are not fully flushed. Occasionally
 *       some loaded chunks are not visible at all (become visible again when 
 *       loaded from a different position). These two things could be linked.
 *       Not the biggest deal but needs a fix.
 *
 */

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

    manager_generate_chunks_init(&state);
    upload_stage();
}

static void render(void)
{
    sg_begin_pass(&(sg_pass) {
        .action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = {0.25, 0.5, 0.75, 1.0}
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

        mat4 translation = em_translate_mat4((vec3) {c->x >> 1, 0.0, c->z >> 1});
        mat4 scale = em_scale_mat4((vec3) {0.5, 0.5, 0.5});
        vs_params_t vs_params = {
            .u_mvp = em_mul_mat4(em_mul_mat4(state.cam.vp, translation), scale),
            .u_chnk_pos = {
                (float) c->x,
                0.0,
                (float) c->z
            }
        };

        sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));

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
    manager_cleanup_chunks(&state);

    // BUG: For debugging, see top.
    // printf("cleanup\n");
}

static void tick(void)
{
    manager_demote_chunks(&state);

    // BUG: For debugging, see top.
    // size_t size_hot = state.buckets[BUCKET_HOT].chunks->count;
    // size_t size_cold = state.buckets[BUCKET_COLD].chunks->count;
    // size_t size_stale = state.buckets[BUCKET_STALE].chunks->count;
    // size_t size_free = state.free_list->count;
    // size_t sum = size_hot + size_cold + size_stale;
    // printf("h: %zu, c: %zu, s: %zu, f: %zu, sum: %zu\n", size_hot, 
    //        size_cold, size_stale, size_free, sum);
}

static void frame(void)
{
    state.frame++; // Don't worry, this lasts 9 billion years at 60fps before overflow.
    state.tick++;
    state.l_tick++;
    if (state.tick == 5) 
    {
        tick();
        state.tick = 0;
    }

    if (state.l_tick == 30)
    {
        l_tick();
        state.l_tick = 0;
    }

    /* Check if player has entered new chunk and generate new chunks if they have. */
    ivec2 chunk_pos = {
        floor(state.cam.pos.x / (float) CHUNK_SIZE) * 8,
        floor(state.cam.pos.z / (float) CHUNK_SIZE) * 8
    };
    if (!em_equals_ivec2(state.prev_chunk_pos, chunk_pos)) 
    {
        manager_generate_chunks(&state);
        upload_stage();
        state.prev_chunk_pos = chunk_pos;

        /*
         *  TODO: track which chunk the player is in and then use this for 
         *        colissions. Let's get some nice ground movement going!
         */
    }

    /* Apply player inputs. */
    cam_frame(&(frame_desc_t) {
        .cam        = &state.cam,
        .mouse_dx   = &state.mouse_dx,
        .mouse_dy   = &state.mouse_dy,
        .key_down   = state.key_down,
        .dt         = sapp_frame_duration()
    });

    render();
}

static void cleanup(void)
{
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
        break;

    case SAPP_EVENTTYPE_MOUSE_MOVE:
        state.mouse_dx += event->mouse_dx;
        state.mouse_dy += event->mouse_dy;
        break;

    case SAPP_EVENTTYPE_MOUSE_DOWN:
        sapp_lock_mouse(true);
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
        .sample_count       = 1,
        .window_title       = "Minecraft Remake",
        .icon.sokol_default = true
    };
}

#endif // TEST
