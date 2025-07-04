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
#include "cube.h"

#include "shaders/cube.glsl.h"

typedef struct state {
	/* Player */
	camera_t cam;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
	cube_uv_lookup_t uv_lookup;

	/* World */
	uint16_t instance_count;
	cube_instance_t *instances;
	uint16_t deferred_count;
	cube_instance_t *deferred;

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
