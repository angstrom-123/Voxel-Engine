#ifndef CAMERA_H
#define CAMERA_H

#include "geometry_types.h"

#include "include_sokol.h"
#include <libem/em_math.h>

#include <stdbool.h>
#include <math.h>

#define WORLD_X ((vec3) {1.0, 0.0, 0.0})
#define WORLD_Y ((vec3) {0.0, 1.0, 0.0})

typedef struct camera_desc {
    float near;
    float far;
    float aspect;
    float fov;
    vec3 pos;
} camera_desc_t;

typedef struct camera {
    // parameters
    float near;          // near clipping distance
    float far;           // far clipping distance
    float aspect;        // width / height
    float fov;           // field of view (degrees)
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

void cam_init(camera_t *cam, const camera_desc_t *desc);
void cam_update(camera_t *cam);
vec3 cam_get_fwd(camera_t *cam);
vec3 cam_get_right(camera_t *cam);
vec3 cam_get_up(camera_t *cam);

#endif
