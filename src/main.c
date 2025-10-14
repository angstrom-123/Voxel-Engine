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

static game_t game_instance;

static void init(void)
{
    game_init(&game_instance, &(game_desc_t) {
        .render_distance = 3,
        .num_chunk_slots = 512,
        .ticks_per_second = 5,
        .seed = 0
    });
}

static void frame(void)
{
    game_render(&game_instance);
}

static void cleanup(void)
{
    game_cleanup(&game_instance);
    sg_shutdown();
}

static void event(const sapp_event *event)
{
    const event_t ev = event_sys_convert_event(event);
    game_event(&game_instance, &ev);
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
