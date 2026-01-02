#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "aabb.h"
#include "camera.h"
#include "event_system.h"

#define MAX_PITCH 89.9

typedef struct camera_controller {
    float floor_friction;
    float air_friction;
    float jump_impulse;
    float acceleration;
    float turn_speed;
    float gravity;
    bool on_ground;
    vec3 velocity;
    vec3 max_velocity;
    aabb_t collider;
} camera_controller_t;

typedef struct camera_controller_desc {
    vec3 start_pos;
    float floor_friction;
    float air_friction;
    float jump_impulse;
    float acceleration;
    float turn_speed;
    float gravity;
    vec3 max_velocity;
    vec3 collider_size;
} camera_controller_desc_t;

extern void camera_ctl_init(camera_controller_t *cc, camera_t *cam, const camera_controller_desc_t *desc);
extern void camera_ctl_cleanup(camera_controller_t *cc);
extern void camera_ctl_update(camera_controller_t *cc, camera_t *cam, event_system_t *es, double dt);

#endif
