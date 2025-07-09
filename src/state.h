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
#include "geometry.h"

#include "shaders/chunk.glsl.h"

typedef struct state {
	/* Global state */
	uint8_t tick;

	/* Player */
	camera_t cam;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
	cube_uv_lookup_t uv_lookup;

	/* World */
	uint32_t v_cnt;
	uint32_t i_cnt;
	sg_buffer vbo;
	sg_buffer ibo;
	size_t chunk_count;
	chunk_t **chunks;

	/* Input */
	bool key_down[SAPP_KEYCODE_MENU + 1];
	float mouse_dx;
	float mouse_dy;
} state_t;

extern void state_init_pipeline(state_t *state);
extern void state_init_bindings(state_t *state);
extern void state_init_textures(state_t *state);
extern void state_init_cam(state_t *state);

#endif
