#if defined(TEST)

#include <libem/em_impl.h>

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

#include "main.h"
#include "app/app_main.h"

static engine_t engine_instance;
static app_t app_instance;

static void init(void)
{
    // const size_t RENDER_DISTANCE = 3;
    const size_t RENDER_DISTANCE = 8;
    engine_init(&engine_instance, &(engine_desc_t) {
        .render_distance = RENDER_DISTANCE,
        .num_chunk_slots = 512,
        .ticks_per_second = 5.0,
        .seed = 0
    });
    app_init(&app_instance, &(app_desc_t) {
    });
}

static void frame(void)
{
    app_frame(&engine_instance, &app_instance, sapp_frame_duration());
    engine_render(&engine_instance);
    engine_frame_update(&engine_instance);
}

static void cleanup(void)
{
    engine_cleanup(&engine_instance);
    app_cleanup(&app_instance);
    sg_shutdown();
}

static void event(const sapp_event *event)
{
    const event_t ev = event_sys_convert_event(event);
    engine_event(&engine_instance, &ev);
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
