#include "camera.h"

vec3 cam_get_fwd(camera_t *cam)
{
    return em_quaternion_rotate_vec3((vec3) {0.0, 0.0, -1.0}, cam->rot);
}

vec3 cam_get_right(camera_t *cam)
{
    return em_quaternion_rotate_vec3((vec3) {1.0, 0.0, 0.0}, cam->rot);
}

vec3 cam_get_up(camera_t *cam)
{
    return em_quaternion_rotate_vec3((vec3) {0.0, 1.0, 0.0}, cam->rot);
}

void cam_init(camera_t *cam, const camera_desc_t *desc) 
{
    const quaternion identity = {1, 0, 0, 0};
    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(identity));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(desc->pos, -1.0));

    cam->near   = desc->near;
    cam->far    = desc->far;
    cam->aspect = desc->aspect;
    cam->fov    = desc->fov;
    cam->rot    = identity;
    cam->pos    = desc->pos;
    cam->view   = em_mul_mat4(rot, trans);
    cam->proj   = em_perspective(desc->fov, desc->aspect, desc->near, desc->far);

    cam_update(cam);
}

void cam_update(camera_t *cam)
{
    cam->vp = em_mul_mat4(cam->proj, cam->view);
}
