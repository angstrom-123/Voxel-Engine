#ifndef ENGINE_H
#define ENGINE_H

#include "chunk_system.h"
#include "event_system.h"
#include "load_system.h"
#include "render_system.h"
#include "update_system.h"
#include "instrumentor.h"

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

#ifndef SOKOL_TIME_INCLUDED
#undef SOKOL_IMPL 
#include <sokol/sokol_time.h>
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

typedef struct engine_desc {
    size_t render_distance;
    uint32_t seed;
    size_t num_chunk_slots;
    size_t ticks_per_second;
} engine_desc_t;

typedef struct engine {
    load_system_t _load_sys;
    chunk_system_t _chunk_sys;
    render_system_t _render_sys;
    update_system_t _update_sys;
    event_system_t _event_sys;
} engine_t;

extern void engine_init(engine_t *engine, const engine_desc_t *desc);
extern void engine_cleanup(engine_t *engine);
extern void engine_event(engine_t *engine, const event_t *event);
extern void engine_render(engine_t *engine);
extern void engine_frame_update(engine_t *engine);

#endif
