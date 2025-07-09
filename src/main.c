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

static void init(void)
{
	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
		.logger.func = slog_func,
	});

	state.v_cnt = 0;
	state.i_cnt = 0;

	/* Vertex buffer. */
	state.vbo = sg_make_buffer(&(sg_buffer_desc) {
		.size = 256 * 256 * 1024,
		.usage = {
			.vertex_buffer = true,
			.dynamic_update = true
		}
	});

	/* Index buffer. */
	state.ibo = sg_make_buffer(&(sg_buffer_desc) {
		.size = 256 * 256 * 1024,
		.usage = {
			.index_buffer = true,
			.dynamic_update = true
		}
	});

	state.tick = 0;
	state.chunk_count = 0;
	state.chunks = NULL;

	state_init_pipeline(&state);
	state_init_bindings(&state);
	state_init_textures(&state);
	state_init_cam(&state);

	state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.25, 0.5, 0.75, 1.0}
		}
	};

	chunk_set_t tmp = gen_chunks_around(state.cam.position, state.cam.render_distance);
	state.chunks = tmp.chunks;
	state.chunk_count = tmp.count;
}

static void render(void)
{
	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});

	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&(sg_bindings) {
		.vertex_buffers[0] = state.vbo,
		.index_buffer = state.ibo,
		.samplers[0] = state.bind.samplers[0],
		.images[0] = state.bind.images[0],
	});

	/* Apply per-chunk uniforms. */
	for (size_t i = 0; i < state.chunk_count; i++)
	{
		chunk_t *chunk = state.chunks[i];

		vs_params_t vs_params;

		vs_params.u_mvp = em_mul_mat4(state.cam.view_proj, em_translate_mat4(chunk->pos));
		vs_params.u_chnk_pos[0] = (float) chunk->mesh.x;
		vs_params.u_chnk_pos[1] = (float) chunk->mesh.y;
		vs_params.u_chnk_pos[2] = (float) chunk->mesh.z;

		sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
		sg_draw(chunk->vbo_offsets.i_offset, chunk->vbo_offsets.i_len, 1);
	}

	sg_end_pass();
	sg_commit();
}

static void tick(void)
{
	/* Update state's buffers to include newly loaded chunks. */
	for (size_t i = 0; i < state.chunk_count; i++)
	{
		chunk_t *chunk = state.chunks[i];
		if (!chunk->loaded)
		{
			sg_append_buffer(state.vbo, &(sg_range) {
				.ptr = chunk->mesh.v_buf,
				.size = (size_t) (chunk->mesh.v_cnt * sizeof(vertex_t))
			});
			sg_append_buffer(state.ibo, &(sg_range) {
				.ptr = chunk->mesh.i_buf,
				.size = (size_t) (chunk->mesh.i_cnt * sizeof(uint16_t))
			});

			chunk->vbo_offsets = (vbo_data_t) {
				.v_len = chunk->mesh.v_cnt,
				.i_len = chunk->mesh.i_cnt,
				.v_offset = state.v_cnt,
				.i_offset = state.i_cnt
			};

			state.v_cnt += chunk->mesh.v_cnt;
			state.i_cnt += chunk->mesh.i_cnt;

			chunk->loaded = true;
		}
	}
}

static void frame(void)
{
	state.tick++;
	if (state.tick == 5) tick();

	/* Apply user inputs. */
	double dt = sapp_frame_duration();
	cam_frame(&state.cam, state.key_down, &state.mouse_dx, &state.mouse_dy, dt);

	render();
}

static void cleanup(void)
{
	for (size_t i = 0; i < state.chunk_count; i++) 
	{
		free(state.chunks[i]->blocks);
		free(state.chunks[i]->mesh.v_buf);
		free(state.chunks[i]->mesh.i_buf);
		free(state.chunks[i]);
	}
	free(state.chunks);

	sg_destroy_buffer(state.vbo);
	sg_destroy_buffer(state.ibo);

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
