#ifndef CAMERA_H
#define CAMERA_H

#if !defined(SOKOL_APP_INCLUDED) 
#include "sokol_app.h"
#endif

#include "extra_math.h"

#define MAX_PITCH 89.0
#define WORLD_X (em_vec3) {.x = 1.0, .y = 0.0, .z = 0.0}
#define WORLD_Y (em_vec3) {.x = 0.0, .y = 1.0, .z = 0.0}

typedef struct camera_desc {
	uint8_t render_distance;
	float near_dist;
	float far_dist;
	float aspect;
	float fov;
	float turn_sens;
	float move_sens;
	em_quaternion rotation;
	em_vec3 position;
} camera_desc_t;

typedef struct camera {
	// parameters
	uint8_t render_distance;		// maximum rendering distance in chunks
	float near_dist;				// near clipping distance
	float far_dist;					// far clipping distance
	float aspect;					// width / height
	float fov;						// field of view (degrees)
	float turn_sens;				// turn speed multiplier
	float move_sens;				// movement speed multiplier
	// transform
	float pitch;					// current pitch (up / down) angle (degrees)
	float yaw;						// current yaw (left / right) angle (degrees)
	em_quaternion rotation;			// current rotation
	em_vec3 position;				// current position
	// basis vectors
	em_vec3 forward;				// basis forward vector
	em_vec3 right;					// basis right vector
	em_vec3 up;						// basis up vector
	// matrices
	em_mat4 view;					// view matrix
	em_mat4 proj;					// projection matrix
	em_mat4 view_proj;				// projection matrix * view matrix
} camera_t;

camera_t cam_setup(const camera_desc_t *desc);
void cam_update(camera_t *cam);
void cam_handle_mouse(camera_t *cam, float mouse_dx, float mouse_dy);
void cam_handle_keyboard(camera_t *cam, bool *key_down, double dt);
void cam_frame(camera_t *cam, bool *key_down, float *mouse_dx, float *mouse_dy, double dt);

#endif
