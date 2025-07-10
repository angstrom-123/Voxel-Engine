#ifndef STATE_H
#define STATE_H

#if !defined(SOKOL_APP_INCLUDED) 
#include "sokol_app.h"
#endif

#if !defined(SOKOL_GFX_INCLUDED) 
#include "sokol_gfx.h"
#endif

#define MAX_INSTANCES 10000

#include <stdlib.h> // stderr
#include "camera.h"
#include "bmp.h"
#include "geometry.h"

#include "shaders/chunk.glsl.h"

typedef struct chunk_buffer {
	uint32_t v_cnt;
	uint32_t i_cnt;

	sg_buffer vbo;
	sg_buffer ibo;

	vertex_t *v_stg;
	uint16_t *i_stg;
} chunk_buffer_t;

typedef struct state {
	/* Global state */
	uint8_t tick;

	/* Player */
	camera_t cam;
	em_ivec3 prev_chunk_pos;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_act;

	/* World */
	uint16_t chunk_cnt;
	chunk_t **chunks;
	chunk_buffer_t cb;

	/* Input */
	bool key_down[SAPP_KEYCODE_MENU + 1];
	float mouse_dx;
	float mouse_dy;
} state_t;

extern void state_init_pipeline(state_t *state);
extern void state_init_bindings(state_t *state);
extern void state_init_textures(state_t *state);
extern void state_init_cam(state_t *state);
extern void state_init_chunk_buffer(state_t *state);

#endif
