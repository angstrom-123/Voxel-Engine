#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

#include "math/extra_math.h"

#include "shaders/triangle.glsl.h"
#include "shaders/cube.glsl.h"

struct cube_bufs {
	float vertices[168];
	uint16_t indices[36];
};

static struct cube_bufs _get_cube_bufs(void)
{
	return (struct cube_bufs) {
		.vertices = {
			-1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
			 1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
			 1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
			-1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

			-1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
			 1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
			 1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
			-1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

			-1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
			-1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
			-1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
			-1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

			 1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
			 1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
			 1.0,  1.0,  1.0,   1.0, 0.5, 0.0, 1.0,
			 1.0, -1.0,  1.0,   1.0, 0.5, 0.0, 1.0,

			-1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
			-1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
			 1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
			 1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

			-1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
			-1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
			 1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
			 1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0
		},
		.indices = {
			 0,  1,  2,    0,  2,  3,
			 6,  5,  4,	   7,  6,  4,
			 8,  9, 10,	   8, 10, 11,
			14, 13, 12,	  15, 14, 12,
			16, 17, 18,	  16, 18, 19,
			22, 21, 20,	  23, 22, 20
		},
	};
}

static struct {
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
} state;

static void init(void)
{
	sg_setup(&(sg_desc) {
		.environment = sglue_environment(),
	});

	struct cube_bufs cube_bufs = _get_cube_bufs();

	sg_buffer v_buf = sg_make_buffer(&(sg_buffer_desc) {
		.data = SG_RANGE(cube_bufs.vertices),
		.label = "cube-vertices"
	});

	sg_buffer i_buf = sg_make_buffer(&(sg_buffer_desc) {
		.usage.index_buffer = true,
		.data = SG_RANGE(cube_bufs.indices),
		.label = "cube-indices"
	});

	sg_shader shdr = sg_make_shader(cube_cube_shader_desc(sg_query_backend()));

	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.layout = {
			.buffers[0].stride = 28,
			.attrs = {
				[ATTR_cube_cube_position].format = SG_VERTEXFORMAT_FLOAT3,
				[ATTR_cube_cube_color0].format = SG_VERTEXFORMAT_FLOAT4
			}
		},
		.shader = shdr,
		.index_type = SG_INDEXTYPE_UINT16,
		.cull_mode = SG_CULLMODE_BACK,
		.depth = {
			.write_enabled = true,
			.compare = SG_COMPAREFUNC_LESS_EQUAL
		},
		.label = "cube-pipeline"
	});

	state.bind = (sg_bindings) {
		.vertex_buffers[0] = v_buf,
		.index_buffer = i_buf
	};

	state.pass_action = (sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.25, 0.5, 0.75, 1.0}
		}
	};
}

static void frame(void)
{
	const float w = sapp_width();
	const float h = sapp_height();
	const float t = (float) (sapp_frame_duration() * 60);

	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&state.bind);
	sg_draw(0, 36, 1);
	sg_end_pass();
	sg_commit();
}

static void cleanup(void)
{
	sg_shutdown();
}

static void event(const sapp_event* event)
{
	(void) event;
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
		.window_title = "Minecraft Remake",
		.icon.sokol_default = true,
	};
}
