#include "camera.h"

static em_vec3 _get_forward(em_quaternion camera_rotation)
{
	return em_quaternion_rotate_vec3(em_new_vec3(0.0, 0.0, -1.0), camera_rotation);
}

static em_vec3 _get_right(em_quaternion camera_rotation)
{
	return em_quaternion_rotate_vec3(em_new_vec3(1.0, 0.0, 0.0), camera_rotation);
}

static em_vec3 _get_up(em_quaternion camera_rotation)
{
	return em_quaternion_rotate_vec3(em_new_vec3(0.0, 1.0, 0.0), camera_rotation);
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
	cam->rotation = em_mul_quaternion(pitch_q, yaw_q);
	cam->rotation = em_normalize_quaternion(cam->rotation);
}

void cam_handle_keyboard(camera_t *cam, bool *key_down)
{
	// movement
	em_vec3 move = {0};
	if (key_down[_SAPP_KEYCODE_W]) move = em_add_vec3(move, cam->forward);
	if (key_down[_SAPP_KEYCODE_S]) move = em_sub_vec3(move, cam->forward);
	if (key_down[_SAPP_KEYCODE_D]) move = em_add_vec3(move, cam->right);
	if (key_down[_SAPP_KEYCODE_A]) move = em_sub_vec3(move, cam->right);
	move = em_mul_vec3_f(em_normalize_vec3(move), cam->move_sens);

	move.y = 0.0; // TODO: implement more advanced movement, this is a quick fix

	cam->position = em_add_vec3(cam->position, move);
}

camera_t cam_setup(const camera_desc_t *desc) 
{
	camera_t cam = {
		.near_dist   = desc->near_dist,
		.far_dist    = desc->far_dist,
		.aspect	     = desc->aspect,
		.fov 	     = desc->fov,
		.turn_sens   = desc->turn_sens,
		.move_sens   = desc->move_sens,
		.rotation    = desc->rotation,
		.position    = desc->position,
	};

	cam_update(&cam);
	return cam;
}

void cam_update(camera_t *cam)
{
	// recalculate basis vectors
	cam->forward = _get_forward(cam->rotation);
	cam->right = _get_right(cam->rotation);
	cam->up = _get_up(cam->rotation);

	// recalculate matrices
	cam->proj = em_perspective(cam->fov, cam->aspect, cam->near_dist, cam->far_dist);
	cam->view = em_look_at(cam->position, em_add_vec3(cam->position, cam->forward), cam->up);
	cam->view_proj = em_mul_mat4(cam->proj, cam->view);
}
