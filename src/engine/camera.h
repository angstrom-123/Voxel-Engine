#ifndef CAMERA_H
#define CAMERA_H

#include "include_sokol.h"
#include <libem/em_math.h>

#include <stdbool.h>
#include <math.h>

#include "geometry_types.h"
#include "logger.h"

#define WORLD_X ((vec3) {1.0, 0.0, 0.0})
#define WORLD_Y ((vec3) {0.0, 1.0, 0.0})

typedef enum camera_projection {
    PROJECTION_NONE,
    PROJECTION_PERSPECTIVE,
    PROJECTION_ORTHOGRAPHIC
} camera_projection_e;

typedef struct camera_desc {
    float near;
    float far;
    vec3 pos;
    /* Perspective only. */
    float aspect;
    float fov;
    /* Orthographic only. */
    float left;
    float right;
    float bottom;
    float top;
} camera_desc_t;

typedef struct camera {
    camera_projection_e kind;
    float near;
    float far;
    float pitch;
    float yaw;
    quat rot;
    vec3 pos;
    vec3 fwd;
    vec3 right;
    vec3 up;
    mat4 view;
    mat4 proj;
    mat4 vp;
    union {
        struct perspective {
            float aspect;
            float fov;
        } perspective;
        struct orthographic {
            float left;
            float right;
            float bottom;
            float top;
            float scale;
        } orthographic;
    };
} camera_t;

typedef struct frame_desc {
    camera_t *cam;
    bool *lmb;
    bool *rmb;
    float *mouse_dx;
    float *mouse_dy;
    bool *key_down;
    double dt;
} frame_desc_t;

void cam_init(camera_t *cam, camera_projection_e projection, const camera_desc_t *desc);
void cam_update(camera_t *cam);
void cam_set_scale(camera_t *cam, float scale);
vec3 cam_get_fwd(camera_t *cam);
vec3 cam_get_right(camera_t *cam);
vec3 cam_get_up(camera_t *cam);

#endif
