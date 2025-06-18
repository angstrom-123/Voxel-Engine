#define SOKOL_IMPL

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

#include <stdio.h>

static void init(void)
{
	sg_desc desc = {
		.environment = sglue_environment(),
	};
	sg_setup(&desc);
}

static void frame(void)
{
	sg_pass pass = {
		.action = {
			.colors[0] = {
				.load_action = SG_LOADACTION_CLEAR,
				.clear_value = {0.0, 0.5, 1.0, 1.0},
			},
		},
		.swapchain = sglue_swapchain(),
	};
	sg_begin_pass(&pass);
	sg_end_pass();
	sg_commit();
}

static void cleanup(void)
{
	sg_shutdown();
}

static void event(const sapp_event* event)
{
	printf("event: %u\n", event->char_code);
}

sapp_desc sokol_main(int argc, char* argv[])
{
	printf("Running\n");

	(void) argc;
	(void) argv;

	return (sapp_desc) {
		.init_cb = init,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.event_cb = event,
		.width = 800,
		.height = 600,
		.window_title = "Minecraft Remake",
		.icon.sokol_default = true,
	};
}
