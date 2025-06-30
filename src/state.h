#ifndef STATE_H
#define STATE_H

#if !defined(SOKOL_APP_INCLUDED) 
#include "sokol_app.h"
#endif

#if !defined(SOKOL_GFX_INCLUDED) 
#include "sokol_gfx.h"
#endif

#define MAX_INSTANCES 20

#include "camera.h"
#include "mesh.h"

typedef struct state {
	camera_t cam;
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;

	sg_buffer instance_buf;
	uint16_t instance_count;
	cube_instance_t *instances;

	bool key_down[SAPP_KEYCODE_MENU + 1];
	float mouse_dx;
	float mouse_dy;
} state_t;

#endif
