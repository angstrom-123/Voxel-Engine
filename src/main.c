#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

#include "triangle.glsl.h"

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

	float vertices[] = {
		 0.0,  0.5, 0.5,	1.0, 0.0, 0.0, 1.0,
		 0.5, -0.5, 0.5,	0.0, 1.0, 0.0, 1.0,
		-0.5, -0.5, 0.5, 	0.0, 0.0, 1.0, 1.0
	};

	state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
		.data = SG_RANGE(vertices),
		.label = "triangle-vertices",
	});

	sg_shader shdr = sg_make_shader(triangle_shader_desc(sg_query_backend()));

	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = shdr,
		.layout = {
			.attrs = {
				[ATTR_triangle_position].format = SG_VERTEXFORMAT_FLOAT3,
				[ATTR_triangle_color0].format = SG_VERTEXFORMAT_FLOAT4
			},
		}, 
		.label = "triangle-pipeline",
	});

	state.pass_action =(sg_pass_action) {
		.colors[0] = {
			.load_action = SG_LOADACTION_CLEAR,
			.clear_value = {0.0, 0.0, 0.0, 1.0}
		}
	};
}

static void frame(void)
{
	sg_begin_pass(&(sg_pass) {
		.action = state.pass_action,
		.swapchain = sglue_swapchain()
	});
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&state.bind);
	sg_draw(0, 3, 1);
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
