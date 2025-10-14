#ifndef GAME_H
#define GAME_H

#include "chunk_system.h"
#include "event_system.h"
#include "load_system.h"
#include "render_system.h"
#include "update_system.h"

#ifndef SOKOL_GFX_INCLUDED
#undef SOKOL_IMPL
#include <sokol/sokol_gfx.h>
#endif

#ifndef SOKOL_APP_INCLUDED
#undef SOKOL_IMPL
#include <sokol/sokol_app.h>
#endif

#ifndef SOKOL_GLUE_INCLUDED
#undef SOKOL_IMPL
#include <sokol/sokol_glue.h>
#endif

#ifndef SOKOL_LOG_INCLUDED
#undef SOKOL_IMPL
#include <sokol/sokol_log.h>
#endif

#ifndef SOKOL_NUKLEAR_INCLUDED
#undef NK_IMPLEMENTATION
#undef SOKOL_IMPL
#include <nuklear/nuklear.h>
#include <sokol/sokol_nuklear.h>
#endif

/*
 * Engine:
 *      Windowing
 *      Updating es for polling
 *      World gen, meshing, rendering
 *      Camera (not actual movement code)
 *      Event System 
 *      Layer System 
 *
 * Common:
 *      Logger 
 *
 * App:
 *      Event handling, actually polling ev_sys
 *      Camera controller
 *      Game logic
 */

typedef struct game_desc {
    size_t render_distance;
    uint32_t seed;
    size_t num_chunk_slots;
    size_t ticks_per_second;
} game_desc_t;

typedef struct game {
    load_system_t load_sys;
    chunk_system_t chunk_sys;
    render_system_t render_sys;
    update_system_t update_sys;
    event_system_t event_sys;
} game_t;

extern void game_init(game_t *game, const game_desc_t *desc);
extern void game_cleanup(game_t *game);
extern void game_event(game_t *game, const event_t *event);
extern void game_render(game_t *game);

#endif
