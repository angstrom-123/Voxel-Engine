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
    float width;
    float height;
} camera_desc_t;

typedef struct camera {
    camera_projection_e kind;
    // parameters
    float near;          // near clipping distance
    float far;           // far clipping distance
    /* Perspective only. */
    float aspect;        // width / height
    float fov;           // field of view (degrees)
    /* Orthographic only. */
    float width;
    float height;
    float scale;
    // transform
    float pitch;         // current pitch (up / down) angle (degrees)
    float yaw;           // current yaw (left / right) angle (degrees)
    quaternion rot;      // current rotation
    vec3 pos;            // current position
    // basis vectors
    vec3 fwd;            // basis forward vector
    vec3 right;          // basis right vector
    vec3 up;             // basis up vector
    // matrices
    mat4 view;           // view matrix
    mat4 proj;           // projection matrix
    mat4 vp;             // projection matrix * view matrix
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
