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

static void frame(void)
{
    INSTRUMENT_FUNC_BEGIN();
    if (!atomic_load(&engine->_running)) 
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
        return;
    }

    app_frame(engine, app, sapp_frame_duration());
    engine_frame(engine, sapp_frame_duration());

    INSTRUMENT_FUNC_END();
}

static void tick(void)
{
    INSTRUMENT_FUNC_BEGIN();
    if (!atomic_load(&engine->_running)) 
    {
        thrd_sleep(&(struct timespec) {
            .tv_nsec = THREAD_AWAIT_NS
        }, NULL);
        return;
    }

    app_tick(engine, app);
    engine_tick(engine);

    INSTRUMENT_FUNC_END();
}

// This is a bit gross but it lets me call it from the tick thread.
void (*tick_func)(void) = tick;

static void cleanup(void *user_data)
{
    INSTRUMENTOR_SESSION_END();

    ENGINE_LOG_WARN("Cleaning up app.\n", NULL);
    app_cleanup(app);
    free(app);
    ENGINE_LOG_WARN("Cleaning up engine.\n", NULL);
    engine_cleanup(engine);
    free(engine);
    ENGINE_LOG_WARN("Cleaning up sokol.\n", NULL);
    sg_shutdown();

    struct user_data *ud = user_data;
    free(ud);

    ENGINE_LOG_OK("Cleanup done.\n", NULL);
}

static void event(const sapp_event *event)
{
    const event_t ev = event_sys_convert_event(event);
    engine_event(engine, &ev);
}

struct user_data {
    int argc;
    char **argv;
};

static void init(void *user_data)
{
    INSTRUMENTOR_SESSION_BEGIN(Minecraft_Session);

    engine = malloc(sizeof(engine_t));
    app = malloc(sizeof(app_t));
    memset(engine, 0, sizeof(engine_t));
    memset(app, 0, sizeof(app_t));

    struct user_data *ud = user_data;
    engine_init(engine);
    app_init(engine, app, &(app_desc_t) {
        .args = {
            .argc = ud->argc,
            .argv = ud->argv
        },
    });
}

sapp_desc sokol_main(int argc, char* argv[])
{
    #ifndef PLAT_LINUX
        RUNTIME_ASSERT(false, "Currently only linux is supported");
    #endif

    struct user_data *ud = malloc(sizeof(*ud));
    ud->argc = argc;
    ud->argv = argv;

    return (sapp_desc) {
        .user_data           = ud,
        .init_userdata_cb    = init,
        .frame_cb            = frame,
        .cleanup_userdata_cb = cleanup,
        .event_cb            = event,
        .logger.func         = slog_func,
        .width               = SCREEN_WIDTH,
        .height              = SCREEN_HEIGHT,
        .sample_count        = 1,
        .window_title        = "Minecraft Remake",
        .icon.sokol_default  = true
    };
}

#endif // TEST
