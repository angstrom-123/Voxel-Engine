#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "camera.h"
#include "event_system.h"

#define CAM_HEIGHT 2.0
#define MAX_PITCH 89.0

typedef struct camera_controller {
    float move_speed;
    float turn_speed;
} camera_controller_t;

typedef struct camera_controller_desc {
    vec3 start_pos;
    float move_speed;
    float turn_speed;
} camera_controller_desc_t;

extern void camera_ctl_init(camera_controller_t *cc, camera_t *cam, const camera_controller_desc_t *desc);
extern void camera_ctl_cleanup(camera_controller_t *cc);
extern void camera_ctl_update(camera_controller_t *cc, camera_t *cam, event_system_t *es, double dt);

#endif
