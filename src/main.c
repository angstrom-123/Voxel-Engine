#if defined(TEST)
#include <libem/em_impl.h>
#define MY_HASHMAP_IMPL
#include "hashmap.h"
#define MY_DLL_IMPL
#include "list.h"

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

#include "state.h"
#include "world_gen.h"

#include "shaders/chunk.glsl.h"

static state_t state = {0};

static void upload_stage(void)
{
    sg_update_buffer(state.cb.vbo, &(sg_range) {
        .ptr = state.cb.v_stg,
        .size = state.cb.v_cnt * sizeof(vertex_t)
    });
    
    sg_update_buffer(state.cb.ibo, &(sg_range) {
        .ptr = state.cb.i_stg,
        .size = state.cb.i_cnt * sizeof(uint32_t)
    });
    
    state.bind.vertex_buffers[0] = state.cb.vbo;
    state.bind.index_buffer = state.cb.ibo;

    // TODO: Find the player's chunk.
}

/*
 * Loads a chunk into staging RAM, ready to be pushed to sg_buffers for rendering.
 * Staging RAM is split into slots with each individual slot's size defined by 
 * V_MAX and I_MAX in geometry.h. A chunk may occupy multiple slots.
 * When staging a new chunk, any old chunk with an age (chunk loads passed since 
 * last visible) greater than the maximum is eligible to be replaced by the new 
 * chunk. When an evicted chunk occupied more slots than the new one, the buffers 
 * become fragmented. 
 * TODO: Implement staging buffer defragmentation above a certain threshold.
 */
static void stage_chunk(chunk_t *chunk)
{
    const uint8_t MAX_AGE = 3;

    uint32_t v_slot = state.cb.v_cnt;
    uint32_t i_slot = state.cb.i_cnt;

    chunk_t *evicted = NULL;
    for (uint16_t i = 0; i < state.chunk_cnt; i++)
    {
        chunk_t *c = state.chunks[i];
        if (c->age > MAX_AGE 
            && c->staged 
            && c->buf_data.num_slots >= chunk->buf_data.num_slots)
        {
            evicted = c;
            break;
        }
    }

    if (!evicted)
    {
        state.cb.v_cnt += chunk->mesh.v_rsrv;
        state.cb.i_cnt += chunk->mesh.i_rsrv;
    } 
    else 
    {
        v_slot = evicted->buf_data.v_ofst;
        i_slot = evicted->buf_data.i_ofst;

        evicted->staged = false;

        /* 
         * If the new chunk uses fewer slots than the evicted, fragmentation occurs
         * in the staging buffers. The excess space is assigned to the new chunk 
         * so it is easier to evict and reuse later.
         */
        chunk->buf_data.num_slots = evicted->buf_data.num_slots;
    }

    memcpy(&state.cb.v_stg[v_slot],
           chunk->mesh.v_buf, chunk->mesh.v_cnt * sizeof(vertex_t));

    memcpy(&state.cb.i_stg[i_slot],
           chunk->mesh.i_buf, chunk->mesh.i_cnt * sizeof(uint32_t));

    chunk->staged = true;
    chunk->age = 0;
    chunk->creation_frame = state.frame;
    chunk->buf_data.v_len = chunk->mesh.v_cnt;
    chunk->buf_data.i_len = chunk->mesh.i_cnt;
    chunk->buf_data.v_ofst = v_slot;
    chunk->buf_data.i_ofst = i_slot;
}

/* 
 * Loop over all chunks in memory. 
 *
 * If any of these chunks are required for this update then they are made 
 * visible and staged if not already.
 *
 * If any of the chunks are not needed for this update then they are made 
 * invisible and their age is incremented. 
 *
 * If there are any chunks required for the update that are not yet loaded into 
 * state memory then they are generated, loaded, and staged.
 *
 * NOTE: Old chunks will be removed fram staging ram but not from state memory.
 * TODO: Very old chunks could be removed from memory and saved as a file
 *          to be loaded in when the player gets closer.
 */
static void generate_chunks(void)
{
    size_t num_coords;
    ivec2 *coords = gen_get_required_coords_2(state.cam.pos, state.cam.rndr_dist, &num_coords);

    for (size_t i = 0; i < num_coords; i++)
    {
        chunk_t *c = state.chunk_map->get_or_default(state.chunk_map, coords[i], NULL);
        if (c) // chunk was found in the hashmap
        {
            c->visible = true;
            if (!c->staged)
                stage_chunk(c);
        }
        else // else
        {
            int32_t x = coords[i].x;
            int32_t z = coords[i].y;
            c = gen_new_chunk(x, z);

            state.chunk_map->put_ptr(state.chunk_map, (ivec2) {x, z}, c);
            stage_chunk(c);
        }
    }

    upload_stage();
}
// static void generate_chunks(void)
// {
//     size_t coords_len;
//     ivec3 *coords = gen_get_required_coords(state.cam.pos, state.cam.rndr_dist, &coords_len);
//     size_t new_chunk_cnt = coords_len;
//
//     bool *chunk_needed = calloc(state.chunk_cnt, sizeof(bool));
//
//     uint32_t *idx_of_coord = malloc(coords_len * sizeof(uint32_t));
//     memset(idx_of_coord, UINT32_MAX, coords_len * sizeof(uint32_t));
//
//     for (uint16_t i = 0; i < state.chunk_cnt; i++)
//     {
//         chunk_t *c = state.chunks[i];
//         ivec3 pos = {
//             c->x,
//             0.0,  // One chunk per column so Y not needed for world pos.
//             c->z
//         };
//         for (size_t j = 0; j < coords_len; j++)
//         {
//             /* If a chunk at this coord is already loaded it is prepared to render. */
//             if (em_equals_ivec3(pos, coords[j])) 
//             {
//                 new_chunk_cnt--;
//                 chunk_needed[i] = true;
//                 idx_of_coord[j] = i;
//
//                 c->visible = true;
//                 c->age = 0;
//                 if (!c->staged) 
//                     stage_chunk(state.chunks[i]);
//
//                 break;
//             }
//         }
//     }
//
//     /* 
//      * Increment age of all loaded, non-visible chunks before generating the new 
//      * ones to avoid looping over the new chunks here (they will all be visible).
//      */
//     for (uint16_t i = 0; i < state.chunk_cnt; i++) 
//     {
//         chunk_t *c = state.chunks[i];
//         c->visible = chunk_needed[i];
//         if (!chunk_needed[i]) 
//             c->age++;
//     }
//
//     if (new_chunk_cnt > 0) // Need to generate new chunks.
//     {
//         size_t new_size = (state.chunk_cnt + new_chunk_cnt) * sizeof(chunk_t);
//         state.chunks = realloc(state.chunks, new_size);
//         for (size_t i = 0; i < coords_len; i++)
//         {
//             /* UINT32_MAX means no chunk with the coordinates coords[i] is loaded. */
//             if (idx_of_coord[i] == UINT32_MAX)
//             {
//                 chunk_t *chunk = gen_new_chunk(coords[i].x, coords[i].y, coords[i].z, 
//                                                state.chunks, state.chunk_cnt);
//                 state.chunks[state.chunk_cnt] = chunk;
//                 stage_chunk(state.chunks[state.chunk_cnt++]);
//             }
//         }
//     }
//
//     free(chunk_needed);
//     free(idx_of_coord);
//
//     upload_stage();
// }

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

    generate_chunks();
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

    /* Apply uniforms and draw each chunk. */
    em_hashmap_iter_t *iter = state.chunk_map->iterator(state.chunk_map);
    while (iter->has_next)
    {
        chunk_t *c = iter->next(iter);
        if (!c->visible || !c->staged)
        {

            continue;
        }

        mat4 translation = em_translate_mat4((vec3) {c->x / 2.0, c->y / 2.0, c->z / 2.0});
        mat4 scale = em_scale_mat4((vec3) {0.5, 0.5, 0.5});
        vs_params_t vs_params = {
            .u_mvp = em_mul_mat4(em_mul_mat4(state.cam.vp, translation), scale),
            .u_chnk_pos = {
                (float) c->x,
                (float) c->y,
                (float) c->z
            }
        };

        sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));

        state.bind.index_buffer_offset = c->buf_data.i_ofst * sizeof(uint32_t);
        state.bind.vertex_buffer_offsets[0] = c->buf_data.v_ofst * sizeof(vertex_t);

        sg_apply_bindings(&state.bind);

        sg_draw(0, c->buf_data.i_len, 1);
    }
    //
    // TODO: What is a fast way to iterate over a hashmap??? (ALSO, CHUNK AGING BASED ON FRAME)
    //
    // for (size_t i = 0; i < state.chunk_cnt; i++)
    // {
    //     chunk_t *c = state.chunks[i];
    //     if (!c->visible || !c->staged)
    //         continue;
    //
    //     mat4 translation = em_translate_mat4((vec3) {c->x / 2.0, c->y / 2.0, c->z / 2.0});
    //     mat4 scale = em_scale_mat4((vec3) {0.5, 0.5, 0.5});
    //     vs_params_t vs_params = {
    //         .u_mvp = em_mul_mat4(em_mul_mat4(state.cam.vp, translation), scale),
    //         .u_chnk_pos = {
    //             (float) c->x,
    //             (float) c->y,
    //             (float) c->z
    //         }
    //     };
    //
    //     sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
    //
    //     state.bind.index_buffer_offset = c->buf_data.i_ofst * sizeof(uint32_t);
    //     state.bind.vertex_buffer_offsets[0] = c->buf_data.v_ofst * sizeof(vertex_t);
    //
    //     sg_apply_bindings(&state.bind);
    //
    //     sg_draw(0, c->buf_data.i_len, 1);
    // }

    sg_end_pass();
    sg_commit();
}

static void tick(void)
{
    // TODO: Do something here.
}

static void frame(void)
{
    state.frame++; // Don't worry, this lasts 9 billion years at 60fps before overflow.
    state.tick++;
    if (state.tick == 5) 
    {
        tick();
        state.tick = 0;
    }

    /* Check if player has entered new chunk and generate new chunks if they have. */
    ivec3 chunk_pos = {
        floor(state.cam.pos.x / (float) CHUNK_SIZE) * 8,
        floor(0.0),
        floor(state.cam.pos.z / (float) CHUNK_SIZE) * 8
    };
    if (!em_equals_ivec3(state.prev_chunk_pos, chunk_pos)) 
    {
        generate_chunks();
        state.prev_chunk_pos = chunk_pos;
    }

    /* Apply player inputs. */
    cam_frame(&(frame_desc_t) {
        .cam        = &state.cam,
        .mouse_dx   = &state.mouse_dx,
        .mouse_dy   = &state.mouse_dy,
        .curr_chunk = state.chunks[state.player_chunk_idx],
        .key_down   = state.key_down,
        .dt         = sapp_frame_duration()
    });

    render();
}

static void cleanup(void)
{
    for (size_t i = 0; i < state.chunk_cnt; i++) 
    {
        free(state.chunks[i]->blocks);
        free(state.chunks[i]->mesh.v_buf);
        free(state.chunks[i]->mesh.i_buf);
        free(state.chunks[i]);
    }

    free(state.chunks);
    free(state.cb.v_stg);
    free(state.cb.i_stg);

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
        {
            sapp_lock_mouse(false);
        }
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
