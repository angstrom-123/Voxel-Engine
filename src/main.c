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

static engine_t *engine;
static app_t *app;

static void init(void)
{
    INSTRUMENTOR_SESSION_BEGIN("Minecraft_Session");

    // TODO: Make this editable by the user at runtime.
#if defined(RELEASE) || defined(PROFILING)
    const size_t RENDER_DISTANCE = 32;
#elif defined(DEBUG)
    const size_t RENDER_DISTANCE = 16;
#endif

    ENGINE_ASSERT(RENDER_DISTANCE >= 3, "Render distance too low.\n");

    engine = malloc(sizeof(engine_t));
    memset(engine, 0, sizeof(engine_t));
    engine_init(engine, &(engine_desc_t) {
        .render_distance = RENDER_DISTANCE,
        .ticks_per_second = 5.0,
        .seed = 0
    });

    app = malloc(sizeof(app_t));
    memset(app, 0, sizeof(app_t));
    app_init(engine, app, &(app_desc_t) {
        .engine = engine
    });
}

static void frame(void)
{
    INSTRUMENT_FUNC_BEGIN();

    app_frame(engine, app, sapp_frame_duration());
    engine_frame_update(engine);
    engine_render(engine);

    engine_update_events(engine);

    INSTRUMENT_FUNC_END();
}

static void cleanup(void)
{
    INSTRUMENTOR_SESSION_END();

    ENGINE_LOG_WARN("Cleaning up engine.\n", NULL);
    engine_cleanup(engine);
    free(engine);
    ENGINE_LOG_WARN("Cleaning up app.\n", NULL);
    app_cleanup(app);
    free(app);
    ENGINE_LOG_WARN("Cleaning up sokol.\n", NULL);
    sg_shutdown();

    ENGINE_LOG_OK("Cleanup done.\n", NULL);
}

static void event(const sapp_event *event)
{
    const event_t ev = event_sys_convert_event(event);
    engine_event(engine, &ev);
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
