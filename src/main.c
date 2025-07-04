#define SOKOL_IMPL

#define TEXTURE_PATH "res/minecraft_remake_texture_atlas.bmp"

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "state.h"
#include "extra_math.h"
#include "world_gen.h"

#include "shaders/cube.glsl.h"

static state_t state = {0};

static void init(void)
{
	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
		.logger.func = slog_func
	});

	cube_init_lookup(&(state.uv_lookup));

	state.instance_count = 0;
	state.instances = NULL; /* instances are dynamically allocated */

	state_init_pipeline(&state);
	state_init_bindings(&state);
	state_init_textures(&state, TEXTURE_PATH);
	state_init_cam(&state);

	state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.25, 0.5, 0.75, 1.0}
		}
	};

	gen_instantiate_cube(&state,  (em_vec3) { 0.0,  4.0,  0.0}, CUBETYPE_LEAF);
	gen_instantiate_chunk(&state, (em_vec3) { 0.0,  0.0,  0.0});
	gen_instantiate_chunk(&state, (em_vec3) { 16.0, 0.0,  0.0});
	gen_instantiate_chunk(&state, (em_vec3) {-16.0, 0.0,  0.0});
	gen_instantiate_chunk(&state, (em_vec3) { 0.0,  0.0,  16.0});
	gen_instantiate_chunk(&state, (em_vec3) { 0.0,  0.0, -16.0});
	gen_instantiate_chunk(&state, (em_vec3) { 16.0, 0.0,  16.0});
	gen_instantiate_chunk(&state, (em_vec3) {-16.0, 0.0,  16.0});
	gen_instantiate_chunk(&state, (em_vec3) { 16.0, 0.0, -16.0});
	gen_instantiate_chunk(&state, (em_vec3) {-16.0, 0.0, -16.0});
}

static void draw_instances(size_t count, cube_instance_t *instances)
{
	const size_t BATCH_SIZE = 256;
	size_t offset = 0;
	size_t remaining = count;
	while (remaining > 0)
	{
		size_t batch_size = (remaining >= BATCH_SIZE)
						  ? BATCH_SIZE
						  : remaining;

		/* set the uniforms for this batch */
		vs_params_t vs_params;
		vs_params.u_mvp = em_mul_mat4(state.cam.view_proj, em_new_mat4_diagonal(1.0));
		vs_params.u_cnt = batch_size;
		for (size_t i = 0; i < batch_size; i++)
		{
			const cube_instance_t *tmp = &instances[offset + i];
			vs_params.u_data[i][0] = tmp->pos.x;
			vs_params.u_data[i][1] = tmp->pos.y;
			vs_params.u_data[i][2] = tmp->pos.z;
			vs_params.u_data[i][3] = tmp->type;
		}

		sg_apply_bindings(&state.bind);
		sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
		sg_draw(0, 36, batch_size);

		remaining -= batch_size;
		offset += batch_size;
	}
}

static void render_cubes(void)
{
	fs_params_t fs_params;
	memcpy(fs_params.u_uv_rects, state.uv_lookup.uv_rects, sizeof(state.uv_lookup.uv_rects));

	sg_apply_pipeline(state.pip);
	sg_apply_uniforms(UB_fs_params, &SG_RANGE(fs_params));

	/* Draw blocks with transparency (deferred instances) last */
	draw_instances(state.instance_count, state.instances);
	draw_instances(state.deferred_count, state.deferred);
}

static void frame(void)
{
	cam_frame(&state.cam, state.key_down, &state.mouse_dx, &state.mouse_dy);

	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});
	render_cubes();

	sg_end_pass();
	sg_commit();
}

static void cleanup(void)
{
	free(state.instances);
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
