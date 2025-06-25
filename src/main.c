#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

#include "bmp.h"
#include "camera.h"

#include "shaders/tex_cube.glsl.h"

typedef struct vertex {
	float x;
	float y;
	float z;

	int16_t u;
	int16_t v;
} vertex_t;

static struct {
	camera_t cam;
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;

	bool key_down[SAPP_KEYCODE_MENU + 1];
	float mouse_dx;
	float mouse_dy;
} state;

static void init(void)
{
	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
	});

	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = sg_make_shader(tex_cube_shader_desc(sg_query_backend())),
		.layout = {
			.attrs = {
				[ATTR_tex_cube_position].format = SG_VERTEXFORMAT_FLOAT3,
				[ATTR_tex_cube_texcoord0].format = SG_VERTEXFORMAT_SHORT2N
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

	const vertex_t vertices[] = {
		// pos 				// uv
		{-1.0, -1.0, -1.0, 		0, 	   0},
		{ 1.0, -1.0, -1.0,  32767, 	   0},
		{ 1.0,  1.0, -1.0,  32767, 32767},
		{-1.0,  1.0, -1.0,  	0, 32767},

		{-1.0, -1.0,  1.0,  	0, 	   0},
		{ 1.0, -1.0,  1.0,  32767, 	   0},
		{ 1.0,  1.0,  1.0,  32767, 32767},
		{-1.0,  1.0,  1.0,  	0, 32767},

		{-1.0, -1.0, -1.0,  	0, 	   0},
		{-1.0,  1.0, -1.0,  32767, 	   0},
		{-1.0,  1.0,  1.0,  32767, 32767},
		{-1.0, -1.0,  1.0,  	0, 32767},

		{ 1.0, -1.0, -1.0,  	0, 	   0},
		{ 1.0,  1.0, -1.0,  32767, 	   0},
		{ 1.0,  1.0,  1.0,  32767, 32767},
		{ 1.0, -1.0,  1.0,  	0, 32767},

		{-1.0, -1.0, -1.0,  	0, 	   0},
		{-1.0, -1.0,  1.0,  32767, 	   0},
		{ 1.0, -1.0,  1.0,  32767, 32767},
		{ 1.0, -1.0, -1.0,  	0, 32767},

		{-1.0,  1.0, -1.0,  0	 , 0	},
		{-1.0,  1.0,  1.0,  32767, 0	},
		{ 1.0,  1.0,  1.0,  32767, 32767},
		{ 1.0,  1.0, -1.0,  0	 , 32767}
	};

	const int16_t indices[] = {
		 0,  1,  2,    0,  2,  3,
		 6,  5,  4,	   7,  6,  4,
		 8,  9, 10,	   8, 10, 11,
		14, 13, 12,	  15, 14, 12,
		16, 17, 18,	  16, 18, 19,
		22, 21, 20,	  23, 22, 20
	};

	state.bind = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.data = SG_RANGE(vertices),
			.label = "tex-cube-vertices"
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.usage.index_buffer = true,
			.data = SG_RANGE(indices),
			.label = "cube-indices"
		}),
		.samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
			.min_filter = SG_FILTER_NEAREST,
			.mag_filter = SG_FILTER_NEAREST
		}),
		.images[0] = sg_alloc_image()
	};

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

	bmp_image_t *atlas = bmp_load_file("res/minecraft_remake_texture_atlas.bmp");
	if (atlas)
	{
		bmp_sub_image_t *sub_tex = bmp_create_sub_image(atlas, &(bmp_sub_image_desc_t) {
			.width = 16,
			.height = 16,
			.x_offset = 48,
			.y_offset = 0,
		});
		if (sub_tex)
		{
			sg_init_image(state.bind.images[0], &(sg_image_desc) {
				.width = (*sub_tex).width,
				.height = (*sub_tex).height,
				.pixel_format = SG_PIXELFORMAT_RGBA8,
				.data.subimage[0][0] = {
					.ptr = (*sub_tex).pixel_data,
					.size = (size_t) ((*sub_tex).img_size)
				}
			});
		}
		free(sub_tex);
	} 
	free(atlas);
}

static void frame(void)
{
	// update camera position / rotation
	cam_handle_mouse(&state.cam, state.mouse_dx, state.mouse_dy);
	cam_handle_keyboard(&state.cam, state.key_down);
	cam_update(&state.cam);

	state.mouse_dx = 0.0;
	state.mouse_dy = 0.0;

	vs_params_t vs_params;
	em_mat4 model = em_new_mat4_diagonal(1.0);
	vs_params.mvp = em_mul_mat4(state.cam.view_proj, model);

	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&state.bind);
	sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
	sg_draw(0, 36, 1);
	sg_end_pass();
	sg_commit();
}

static void cleanup(void)
{
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
		.sample_count = 4,
		.window_title = "Minecraft Remake",
		.icon.sokol_default = true
	};
}
