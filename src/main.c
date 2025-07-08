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
		.logger.func = slog_func
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

	state.chunks = realloc(state.chunks, sizeof(chunk_t *));
	state.chunk_count = 1;

	state.chunks[0] = gen_new_chunk((em_vec3) {0.0, 0.0, 0.0});
}

/* 
 * NOTE: With current techniques, it should be possible to draw up to 8 chunks 
 * render distance in around 2000 draw calls (assuming large, complex chunks).
 */
static void render(void)
{
	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});

	sg_apply_pipeline(state.pip);

	for (size_t i = 0; i < state.chunk_count; i++)
	{
		chunk_t *chunk = state.chunks[i];

		vs_params_t vs_params;
		vs_params.u_mvp = em_mul_mat4(state.cam.view_proj, em_translate_mat4(chunk->pos));
		vs_params.u_chnk_pos[0] = (float) chunk->mesh.x;
		vs_params.u_chnk_pos[1] = (float) chunk->mesh.y;
		vs_params.u_chnk_pos[2] = (float) chunk->mesh.z;

		sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));

		/* Draw each submesh making up the chunk */
		for (size_t j = 0; j < chunk->mesh.submesh_cnt; j++)
		{
			mesh_t mesh = chunk->mesh.submeshes[j];
			sg_apply_bindings(&(sg_bindings) {
				.vertex_buffers[0] = mesh.v_buf,
				.index_buffer = mesh.i_buf,
				.samplers[0] = state.bind.samplers[0],
				.images[0] = state.bind.images[0],
			});
			sg_draw(0, mesh.i_cnt, 1);
		}
	}

	sg_end_pass();
	sg_commit();
}

static void tick(void)
{
}

#define MAX_TIME 240

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
		free(state.chunks[i]->data);
		free(state.chunks[i]->mesh.submeshes);
		free(state.chunks[i]);
	}
	free(state.chunks);
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
