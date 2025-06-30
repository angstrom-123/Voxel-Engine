#include <stdlib.h>
#define SOKOL_IMPL

#define TEXTURE_PATH "res/minecraft_remake_texture_atlas.bmp"

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

#include "bmp.h"
#include "camera.h"
#include "mesh.h"
#include "atlas.h"
#include "state.h"
#include "world_gen.h"

#include "shaders/cube.glsl.h"

// static struct {
// 	camera_t cam;
// 	sg_pipeline pip;
// 	sg_bindings bind;
// 	sg_pass_action pass_action;
//
// 	uint16_t instance_count;
// 	cube_instance_t *instances;
//
// 	bool key_down[SAPP_KEYCODE_MENU + 1];
// 	float mouse_dx;
// 	float mouse_dy;
// } state;

static state_t state = {0};

static void init(void)
{
	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
	});

	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = sg_make_shader(cube_shader_desc(sg_query_backend())),
		.layout = {
			.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE,
			.attrs = {
				[ATTR_cube_base_pos] = {
					.format = SG_VERTEXFORMAT_FLOAT3, 
					.buffer_index = 0
				},
				[ATTR_cube_inst_pos] = {
					.format = SG_VERTEXFORMAT_FLOAT3,
					.buffer_index = 1 /* NOTE: buffer_index = 1, not 0 */
				},
				[ATTR_cube_tex_uv] = {
					.format = SG_VERTEXFORMAT_SHORT2N, 
					.buffer_index = 0
				}
			}
		},
		.index_type = SG_INDEXTYPE_UINT16,
		.cull_mode = SG_CULLMODE_BACK,
		.depth = {
			.compare = SG_COMPAREFUNC_LESS_EQUAL,
			.write_enabled = true
		},
		.label = "tex-cube-pipeline"
	});

	state.instance_count = 0;
	state.instances = malloc(MAX_INSTANCES * sizeof(cube_instance_t));
	if (!state.instances)
	{
		fprintf(stderr, "Failed to malloc instance array\n");
		exit(1);
	}

	cube_t *cube = mesh_generate_cube();
	atlas_set_texture(cube, TEX_GRASS_SIDE);
	atlas_set_top(cube, TEX_GRASS_TOP);
	atlas_set_bottom(cube, TEX_DIRT);

	state.bind = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.data = SG_RANGE(cube->vertices),
			.label = "vertices"
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.usage.index_buffer = true,
			.data = SG_RANGE(cube->indices),
			.label = "indices"
		}),
		.samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
			.min_filter = SG_FILTER_NEAREST,
			.mag_filter = SG_FILTER_NEAREST
		}),
		.images[0] = sg_alloc_image()
	};

	state.instance_buf = sg_make_buffer(&(sg_buffer_desc) {
		.size = MAX_INSTANCES * sizeof(cube_instance_t),
		.usage.stream_update = true,
		.label = "instance-data"
	});
	state.bind.vertex_buffers[1] = state.instance_buf;

	state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.25, 0.5, 0.75, 1.0}
		}
	};

	state.cam = cam_setup(&(camera_desc_t) {
		.near_dist = 0.1,
		.far_dist = 100.0,
		.aspect = (sapp_widthf() / sapp_heightf()),
		.fov = 60.0,
		.turn_sens = 0.04,
		.move_sens = 0.2,
		.rotation = {.x = 0.0, .y = 0.0, .z = 0.0, .w = 1.0},
		.position = {.x = 0.0, .y = 1.5, .z = 6.0},
	});

	bmp_image_t *atlas = bmp_load_file(TEXTURE_PATH);
	if (atlas)
	{
		sg_init_image(state.bind.images[0], &(sg_image_desc) {
			.width = atlas->info_header.width,
			.height = atlas->info_header.height,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.data.subimage[0][0] = {
				.ptr = atlas->pixel_data,
				.size = (size_t) atlas->info_header.img_size
			}
		});
	} 
	else fprintf(stderr, "Failed to load texture atlas at: %s\n", TEXTURE_PATH);

	free(atlas);
	free(cube);

	gen_add_cube_instance(&state, (em_vec3) {.x = 3.0, .y = 0.0, .z = 0.0});
	gen_add_cube_instance(&state, (em_vec3) {.x = 1.0, .y = 0.0, .z = 0.0});
}

static void frame(void)
{
	cam_handle_mouse(&state.cam, state.mouse_dx, state.mouse_dy);
	cam_handle_keyboard(&state.cam, state.key_down);
	cam_update(&state.cam);

	state.mouse_dx = 0.0;
	state.mouse_dy = 0.0;

	vs_params_t vs_params;
	em_mat4 model = em_new_mat4_diagonal(1.0);
	vs_params.mvp = em_mul_mat4(state.cam.view_proj, model);

	if (state.instance_count > 0)
	{
		sg_update_buffer(state.instance_buf, &(sg_range) {
			.ptr = state.instances,
			.size = MAX_INSTANCES * sizeof(cube_instance_t)
		});
	}

	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&state.bind);
	sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
	if (state.instance_count > 0)
		sg_draw(0, 36, state.instance_count);
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
		.width = 1280,
		.height = 720,
		.sample_count = 1,
		.window_title = "Minecraft Remake",
		.icon.sokol_default = true
	};
}
