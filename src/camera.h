#ifndef CAMERA_H
#define CAMERA_H

#if !defined(SOKOL_APP_INCLUDED) 
#include "sokol/sokol_app.h"
#endif

#include <libem/em_math.h>

#include "geometry_types.h"

#define CAM_HEIGHT 2.0
#define MAX_PITCH 89.0
#define WORLD_X ((vec3) {1.0, 0.0, 0.0})
#define WORLD_Y ((vec3) {0.0, 1.0, 0.0})

typedef struct camera_desc {
    uint8_t rndr_dist;
    float near;
    float far;
    float aspect;
    float fov;
    float turn_sens;
    float move_sens;
    quaternion rot;
    vec3 pos;
} camera_desc_t;

typedef struct camera {
    // parameters
    uint8_t rndr_dist; // maximum rendering distance in chunks
    float near;        // near clipping distance
    float far;         // far clipping distance
    float aspect;      // width / height
    float fov;         // field of view (degrees)
    float turn_sens;   // turn speed multiplier
    float move_sens;   // movement speed multiplier
    // transform
    float pitch;       // current pitch (up / down) angle (degrees)
    float yaw;         // current yaw (left / right) angle (degrees)
    quaternion rot;    // current rotation
    vec3 pos;          // current position
    // basis vectors
    vec3 fwd;          // basis forward vector
    vec3 right;        // basis right vector
    vec3 up;           // basis up vector
    // matrices
    mat4 view;         // view matrix
    mat4 proj;         // projection matrix
    mat4 vp;           // projection matrix * view matrix
} camera_t;

typedef struct frame_desc {
    camera_t *cam;
    float *mouse_dx;
    float *mouse_dy;
    bool *key_down;
    double dt;
    chunk_t *curr_chunk;
} frame_desc_t;

camera_t cam_setup(const camera_desc_t *desc);
void cam_update(camera_t *cam);
void cam_handle_mouse(camera_t *cam, float mouse_dx, float mouse_dy);
void cam_handle_keyboard(camera_t *cam, bool *key_down, double dt);
void cam_frame(const frame_desc_t *desc);

#endif
