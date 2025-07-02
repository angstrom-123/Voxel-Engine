#ifndef STATE_H
#define STATE_H

#if !defined(SOKOL_APP_INCLUDED) 
#include "sokol_app.h"
#endif

#if !defined(SOKOL_GFX_INCLUDED) 
#include "sokol_gfx.h"
#endif

#define MAX_INSTANCES 10000

#include "camera.h"
#include "bmp.h"
#include "atlas.h"
#include "mesh.h"

#include "shaders/cube.glsl.h"

typedef struct state {
	/* Player */
	camera_t cam;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;

	/* World */
	uint16_t instance_count;
	cube_instance_t *instances;

	/* Input */
	bool key_down[SAPP_KEYCODE_MENU + 1];
	float mouse_dx;
	float mouse_dy;
} state_t;

extern void state_init_pipeline(state_t *state);
extern void state_init_bindings(state_t *state);
extern void state_init_textures(state_t *state, char* tex_path);
extern void state_init_cam(state_t *state);

#endif
