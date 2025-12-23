#ifndef ENGINE_H
#define ENGINE_H

#include "chunk_system.h"
#include "event_system.h"
#include "geometry_types.h"
#include "load_system.h"
#include "render_system.h"
#include "tick_system.h"
#include "update_system.h"
#include "ui_system.h"
#include "instrumentor.h"
#include "console.h"
#include "raycast.h"

#include "include_sokol.h"
#include "include_nuklear.h"

typedef struct engine_desc {
    size_t render_distance;
    uint32_t seed;
    size_t ticks_per_second;
} engine_desc_t;

typedef struct engine_meta {
    struct {
        bool active;
        float range;
        ivec2 chunk;
        ivec3 cell;
        cube_face_idx_e face;
    } cursor;
    struct {
        uint64_t frame_ctr;
        uint64_t frame_start;
        sprite_t *fps_sprites[3];
        sprite_t *mspf_sprites[3];
    } fps;
} engine_meta_t;

typedef enum block_action {
    BLOCK_ACTION_NONE,
    BLOCK_ACTION_PLACE,
    BLOCK_ACTION_BREAK
} block_action_e;

typedef struct engine {
    load_system_t _load_sys;
    chunk_system_t _chunk_sys;
    render_system_t _render_sys;
    update_system_t _update_sys;
    tick_system_t _tick_sys;
    event_system_t _event_sys;
    ui_system_t _ui_sys;

    struct {
        void (*subscribe_to_event)(struct engine *engine, event_type_e type,
                                   const event_subscriber_desc_t *desc);
        void (*edit_active_block)(struct engine *engine, block_action_e action);
    } api;
    engine_meta_t meta;
} engine_t;

extern void engine_init(engine_t *engine, const engine_desc_t *desc);
extern void engine_cleanup(engine_t *engine);
extern void engine_record_frame_start(engine_t *engine);
extern void engine_record_frame_end(engine_t *engine);
extern void engine_event(engine_t *engine, const event_t *event);
extern void engine_render(engine_t *engine);
extern void engine_frame(engine_t *engine);
extern void engine_tick(engine_t *engine);

#endif
