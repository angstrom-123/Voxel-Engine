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

void cam_init(camera_t *cam, camera_projection_e proj, const camera_desc_t *desc) 
{
    const quaternion identity = { 1, 0, 0, 0 };
    mat4 rot = em_quaternion_to_mat4(em_conjugate_quaternion(identity));
    mat4 trans = em_translate_mat4(em_mul_vec3_f(desc->pos, -1.0));

    cam->kind = proj;
    cam->near = desc->near;
    cam->far = desc->far;
    cam->rot = identity;
    cam->pos = desc->pos;
    cam->view = em_mul_mat4(rot, trans);

    switch (proj) {
    case PROJECTION_PERSPECTIVE:
        cam->aspect = desc->aspect;
        cam->fov = desc->fov;
        cam->proj = em_perspective(desc->fov, desc->aspect, desc->near, desc->far);
        break;
    case PROJECTION_ORTHOGRAPHIC:
        cam->width = desc->width;
        cam->height = desc->height;
        cam->proj = em_orthographic(desc->width, desc->height, desc->near, desc->far);
        cam->scale = 1.0;
        break;
    default:
        ENGINE_ASSERT(false, "Camera projection must be specified");
    };

    cam_update(cam);
}

void cam_update(camera_t *cam)
{
    cam->vp = em_mul_mat4(cam->proj, cam->view);
}

void cam_set_scale(camera_t *cam, float scale)
{
    ENGINE_ASSERT(cam->kind == PROJECTION_ORTHOGRAPHIC, "Cannot scale perspective camera");

    cam->scale = scale;
    cam->proj.elements[0][0] *= scale;
    cam->proj.elements[1][1] *= scale;
}
