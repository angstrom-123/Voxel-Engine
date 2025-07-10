#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "state.h"
#include "extra_math.h"
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
		.size = state.cb.i_cnt * sizeof(uint16_t)
	});
}

static void stage_chunk(chunk_t *chunk)
{
	for (size_t i = 0; i < state.chunk_cnt; i++)
	{
		// TODO: Implement
		if (state.chunks[i]->age > 3) printf("evict\n");
	}
	memcpy(&state.cb.v_stg[state.cb.v_cnt],
		   chunk->mesh.v_buf, chunk->mesh.v_cnt * sizeof(vertex_t));

	memcpy(&state.cb.i_stg[state.cb.i_cnt],
		   chunk->mesh.i_buf, chunk->mesh.i_cnt * sizeof(uint16_t));

	chunk->buf_data = (buf_offsets_t) {
		.v_len = chunk->mesh.v_cnt,
		.i_len = chunk->mesh.i_cnt,
		.v_ofst = state.cb.v_cnt,
		.i_ofst = state.cb.i_cnt
	};

	state.cb.v_cnt += chunk->mesh.v_cnt;
	state.cb.i_cnt += chunk->mesh.i_cnt;

	chunk->staged = true;
	chunk->age = 0;
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
 * 		 to be loaded in when the player gets closer.
 */
static void dynamic_gen(void)
{
	size_t cnt;
	em_ivec3 *coords = gen_get_required_coords(state.cam.pos, state.cam.rndr_dist, &cnt);
	size_t num_to_gen = cnt;

	uint16_t *in_state = malloc(cnt * sizeof(uint32_t));
	memset(in_state, UINT16_MAX, cnt * sizeof(uint32_t)); /* Sentinel value. */

	bool *visible = calloc(state.chunk_cnt, sizeof(bool));
	for (size_t i = 0; i < state.chunk_cnt; i++)
	{
		em_ivec3 pos = {
			state.chunks[i]->x,
			0.0,
			state.chunks[i]->z
		};
		for (size_t j = 0; j < cnt; j++)
		{
			if (em_equals_ivec3(pos, coords[j]))
			{
				num_to_gen--;

				in_state[j] = i;
				visible[i] = true;

				if (!state.chunks[i]->staged) stage_chunk(state.chunks[i]);
				state.chunks[i]->visible = true;
				state.chunks[i]->age = 0;
				
				break;
			}
		}
	}

	for (size_t i = 0; i < state.chunk_cnt; i++) 
	{
		state.chunks[i]->visible = visible[i];
		if (!visible[i]) state.chunks[i]->age++;
	}

	if (num_to_gen > 0)
	{
		chunk_t **tmp = malloc(num_to_gen * sizeof(chunk_t));
		size_t tmp_cnt = 0;
		for (size_t i = 0; i < cnt; i++)
		{
			if (in_state[i] == UINT16_MAX)
			{
				tmp[tmp_cnt] = gen_new_chunk(coords[i].x, coords[i].y, coords[i].z);
				stage_chunk(tmp[tmp_cnt]);
				tmp_cnt++;
			}
		}
		state.chunks = realloc(state.chunks, (state.chunk_cnt + num_to_gen) * sizeof(chunk_t));
		memcpy(&state.chunks[state.chunk_cnt], tmp, tmp_cnt * sizeof(chunk_t));
		state.chunk_cnt += tmp_cnt;
	}

}

static void init(void)
{
	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
		.logger.func = slog_func,
	});

	state.tick = 0;
	state.chunk_cnt = 0;
	state.chunks = NULL;

	state_init_pipeline(&state);
	state_init_bindings(&state);
	state_init_textures(&state);
	state_init_cam(&state);
	state_init_chunk_buffer(&state);

	/* Initialize to sentinel value to force initial chunk load when entering the game. */
	state.prev_chunk_pos = (em_ivec3) {UINT_MAX, UINT_MAX, UINT_MAX};

	state.pass_act = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.25, 0.5, 0.75, 1.0}
		}
	};
}

static void render(void)
{
	sg_begin_pass(&(sg_pass) {
		.action = state.pass_act,
		.swapchain = sglue_swapchain()
	});
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&(sg_bindings) {
		.vertex_buffers[0] = state.cb.vbo,
		.index_buffer = state.cb.ibo,
		.samplers[0] = state.bind.samplers[0],
		.images[0] = state.bind.images[0],
	});

	/* Apply per-chunk uniforms. */
	for (size_t i = 0; i < state.chunk_cnt; i++)
	{
		chunk_t *chunk = state.chunks[i];

		if (!chunk->visible || !chunk->staged) continue;

		vs_params_t vs_params;

		em_vec3 pos = {chunk->x, chunk->y, chunk->z};
		vs_params.u_mvp = em_mul_mat4(state.cam.vp, em_translate_mat4(pos));
		vs_params.u_chnk_pos[0] = (float) chunk->x;
		vs_params.u_chnk_pos[1] = (float) chunk->y;
		vs_params.u_chnk_pos[2] = (float) chunk->z;

		sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));

		sg_draw(chunk->buf_data.i_ofst, chunk->buf_data.i_len, 1);
	}

	sg_end_pass();
	sg_commit();
}

static void tick(void)
{
	upload_stage();
}

static void frame(void)
{
	state.tick++;
	if (state.tick == 5) 
	{
		tick();
		state.tick = 0;
	}

	/* Check if player has entered new chunk and generate new chunks if they have. */
	em_ivec3 curr_chunk_pos = {
		floor(state.cam.pos.x / (float) CHUNK_SIZE) * 8,
		floor(0.0),
		floor(state.cam.pos.z / (float) CHUNK_SIZE) * 8,
	};
	if (!em_equals_ivec3(state.prev_chunk_pos, curr_chunk_pos)) dynamic_gen();

	state.prev_chunk_pos = curr_chunk_pos;

	/* Apply player inputs. */
	double dt = sapp_frame_duration();
	cam_frame(&state.cam, state.key_down, &state.mouse_dx, &state.mouse_dy, dt);

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
		if (event->key_code == SAPP_KEYCODE_CAPS_LOCK)
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
	(void) argc;
	(void) argv;

	return (sapp_desc) {
		.init_cb = init,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.event_cb = event,
		.logger.func = slog_func,
		.width = 1280,
		.height = 720,
		.sample_count = 1,
		.window_title = "Minecraft Remake",
		.icon.sokol_default = true
	};
}
