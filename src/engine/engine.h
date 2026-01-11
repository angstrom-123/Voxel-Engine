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
#include "camera_controller.h"

#include "include_sokol.h"
#include "include_nuklear.h"

#include <stdatomic.h>

typedef struct engine_desc {
    uint8_t render_distance;
    uint32_t seed;
    uint8_t ticks_per_second;
    vec3 base_sun_dir;
    uint64_t max_time;
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
        vec3 base_sun_dir;
        atomic_uint_least64_t time;
        uint64_t max_time;
        uint32_t seed;
        const char *name;
    } world;
    struct {
        sprite_t *cur_sprites[17];
    } debug;
} engine_meta_t;

typedef enum block_action {
    BLOCK_ACTION_NONE,
    BLOCK_ACTION_PLACE,
    BLOCK_ACTION_BREAK
} block_action_e;

typedef struct player_collider_desc {
    vec3 pos;
    aabb_t collider;
} player_collider_desc_t;

typedef struct engine_run_desc {
    uint32_t seed;
    const char *world_name;
    uint64_t time;
    vec3 cam_pos;
    quat cam_rot;
} engine_run_desc_t;

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
        void (*edit_active_block)(struct engine *engine, block_action_e action, 
                                  const player_collider_desc_t *desc);
        void (*start_running)(struct engine *engine, const engine_run_desc_t *desc);
    } api;
    engine_meta_t meta;
} engine_t;

extern void engine_init(engine_t *engine, const engine_desc_t *desc);
extern void engine_cleanup(engine_t *engine);
extern void engine_event(engine_t *engine, const event_t *event);
extern void engine_frame(engine_t *engine, double dt);
extern void engine_tick(engine_t *engine);

#endif
