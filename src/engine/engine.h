#ifndef ENGINE_H
#define ENGINE_H

#include "chunk_system.h"
#include "event_system.h"
#include "geometry_types.h"
#include "load_system.h"
#include "render_system.h"
#include "tick_system.h"
#include "ui_system.h"
#include "update_system.h"
#include "instrumentor.h"
#include "raycast.h"
#include "camera_controller.h"

#include "include_sokol.h"

#include <stdatomic.h>

#define MAX_RENDER_DISTANCE 32
#define MIN_RENDER_DISTANCE 3

typedef struct engine_desc {
    uint8_t render_distance;
    uint8_t ticks_per_second;
    vec3 base_sun_dir;
    uint64_t max_time;
    chunk_data_t *( *gen_func)(ivec2, uint32_t);
    bool init_chunk_renderer;
    bool init_sprite_renderer;
    bool init_cursor_line_renderer;
} engine_desc_t;

typedef struct engine_meta {
    struct {
        size_t max_active_chunks;
        size_t request_queue_size;
        size_t chunk_buffer_pool_size;
    } data;
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
        uint8_t render_dist;
        char name[STD_BUFLEN];
    } world;
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
    char world_name[STD_BUFLEN];
    uint64_t time;
    vec3 cam_pos;
    quat cam_rot;
    float cam_pitch;
    float cam_yaw;
    enum {
        ENGINE_RUN_NEW,
        ENGINE_RUN_LOAD
    } run_mode;
} engine_run_desc_t;

typedef struct engine {
    atomic_bool _running;
    atomic_bool _do_render;
    load_system_t _load_sys;
    chunk_system_t _chunk_sys;
    render_system_t _render_sys;
    update_system_t _update_sys;
    tick_system_t _tick_sys;
    event_system_t _event_sys;
    ui_system_t _ui_sys;
    engine_meta_t meta;
} engine_t;

extern void engine_edit_active_block(engine_t *engine, cube_type_e type, block_action_e action, 
                                     aabb_t *player_collider);
extern void engine_init(engine_t *engine, const engine_desc_t *desc);
extern void engine_do_render(engine_t *engine);
extern void engine_run(engine_t *engine, const engine_run_desc_t *desc);
extern void engine_set_render_distance(engine_t *engine, size_t render_distance);
extern void engine_cleanup(engine_t *engine);
extern void engine_event(engine_t *engine, const event_t *event);
extern void engine_frame(engine_t *engine, double dt);
extern void engine_tick(engine_t *engine);

#endif
