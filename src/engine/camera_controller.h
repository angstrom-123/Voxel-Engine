#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "physics.h"
#include "camera.h"
#include "chunk_system.h"
#include "event_system.h"
#include "instrumentor.h"
#include <stdatomic.h>

#define MAX_PITCH 89.9

typedef struct ctl {
    float floor_friction;
    float air_friction;
    float jump_accel;
    float run_accel;
    float walk_accel;
    float air_accel;
    float turn_speed;
    float gravity;
    size_t time_grounded;
    size_t time_since_grounded;
    size_t jump_cooldown;
    vec3 velocity;
    vec3 collider_size;
    aabb_t collider;
    chunk_data_t *surrounding[3][3];
    bool surrounding_loaded;
    ivec2 surrounding_pos;
    float max_fall_velo;
} ctl_t;

typedef struct ctl_desc {
    vec3 start_pos;
    float floor_friction;
    float air_friction;
    float jump_accel;
    float run_accel;
    float walk_accel;
    float air_accel;
    float turn_speed;
    float gravity;
    vec3 collider_size;
    float max_fall_velo;
} ctl_desc_t;

extern void ctl_init(ctl_t *cc, camera_t *cam, const ctl_desc_t *desc);
extern void ctl_cleanup(ctl_t *cc);
extern void ctl_update_view(ctl_t *cc, camera_t *cam, event_system_t *es);
extern void ctl_update_pos(ctl_t *cc, camera_t *cam, event_system_t *es, double dt);
extern void ctl_update_surrounding(ctl_t *cc, camera_t *cam, chunk_system_t *cs);

#endif
