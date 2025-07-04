#include "camera.h"

static em_vec3 _get_forward(em_quaternion camera_rotation)
{
	return em_quaternion_rotate_vec3((em_vec3) {0.0, 0.0, -1.0}, camera_rotation);
}

static em_vec3 _get_right(em_quaternion camera_rotation)
{
	return em_quaternion_rotate_vec3((em_vec3) {1.0, 0.0, 0.0}, camera_rotation);
}

static em_vec3 _get_up(em_quaternion camera_rotation)
{
	return em_quaternion_rotate_vec3((em_vec3) {0.0, 1.0, 0.0}, camera_rotation);
}

void cam_handle_mouse(camera_t *cam, float mouse_dx, float mouse_dy)
{
	cam->pitch += -mouse_dy * cam->turn_sens;
	cam->yaw += -mouse_dx * cam->turn_sens;

	if (cam->pitch > MAX_PITCH) cam->pitch = MAX_PITCH;
	if (cam->pitch < -MAX_PITCH) cam->pitch = -MAX_PITCH;

	// yaw around global y axis
	em_quaternion yaw_q = em_quaternion_from_axis_angle(WORLD_Y, cam->yaw);

	// pitch around relative x axis
	em_vec3 right = em_quaternion_rotate_vec3(WORLD_X, yaw_q);
	em_quaternion pitch_q = em_quaternion_from_axis_angle(right, cam->pitch);

	// apply quaternions
	cam->rotation = em_normalize_quaternion(em_mul_quaternion(pitch_q, yaw_q));

	// recalculate basis vectors
	cam->forward = _get_forward(cam->rotation);
	cam->right = _get_right(cam->rotation);
	cam->up = _get_up(cam->rotation);

	// recalculate view matrix
	em_mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(cam->rotation));
	em_mat4 trans = em_translate_mat4(em_mul_vec3_f(cam->position, -1.0));
	cam->view = em_mul_mat4(rot, trans);
}

void cam_handle_keyboard(camera_t *cam, bool *key_down)
{
	// movement
	em_vec3 move = {0};

	em_vec3 forward = cam->forward;
	em_vec3 right = cam->right;

	forward.y = 0.0;
	right.y = 0.0;

	forward = em_normalize_vec3(forward);
	right = em_normalize_vec3(right);

	if (key_down[SAPP_KEYCODE_W]) 			 move = em_add_vec3(move, forward);
	if (key_down[SAPP_KEYCODE_S]) 			 move = em_sub_vec3(move, forward);
	if (key_down[SAPP_KEYCODE_D]) 			 move = em_add_vec3(move, right);
	if (key_down[SAPP_KEYCODE_A]) 			 move = em_sub_vec3(move, right);
	if (key_down[SAPP_KEYCODE_SPACE]) 		 move = em_add_vec3(move, WORLD_Y);
	if (key_down[SAPP_KEYCODE_LEFT_CONTROL]) move = em_sub_vec3(move, WORLD_Y);

	if (em_length_squared_vec3(move) > 0.0)
	{
		move = em_mul_vec3_f(move, cam->move_sens);
		cam->position = em_add_vec3(cam->position, move);
	}
}

camera_t cam_setup(const camera_desc_t *desc) 
{
	em_mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(desc->rotation));
	em_mat4 trans = em_translate_mat4(em_mul_vec3_f(desc->position, -1.0));
	camera_t cam = {
		.near_dist   = desc->near_dist,
		.far_dist    = desc->far_dist,
		.aspect	     = desc->aspect,
		.fov 	     = desc->fov,
		.turn_sens   = desc->turn_sens,
		.move_sens   = desc->move_sens,
		.rotation    = em_normalize_quaternion(desc->rotation),
		.position    = desc->position,
		.view 		 = em_mul_mat4(rot, trans),
		.proj 		 = em_perspective(desc->fov, desc->aspect, desc->near_dist, desc->far_dist)
	};

	cam_update(&cam);
	return cam;
}

void cam_update(camera_t *cam)
{
	cam->view_proj = em_mul_mat4(cam->proj, cam->view);
}

void cam_frame(camera_t *cam, bool *key_down, float *mouse_dx, float *mouse_dy)
{
	cam_handle_mouse(cam, *mouse_dx, *mouse_dy);
	cam_handle_keyboard(cam, key_down);
	cam_update(cam);

	*mouse_dx = 0.0;
	*mouse_dy = 0.0;
}
