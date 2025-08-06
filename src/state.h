#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#include <stdlib.h> // fprintf, stderr, exit

#include "camera.h"
#include "geometry_types.h"

#undef MY_HASHMAP_IMPL
#include "hashmap.h"

#include <libem/em_bmp.h>

#include "shaders/chunk.glsl.h"

typedef struct chunk_buffer {
	uint32_t v_cnt;
	uint32_t i_cnt;

	sg_buffer vbo;
	sg_buffer ibo;

	vertex_t *v_stg;
	uint32_t *i_stg;
} chunk_buffer_t;

typedef struct state {
	/* Global state */
	uint8_t tick;
    uint64_t frame;

	/* Player */
	camera_t cam;
	ivec3 prev_chunk_pos;
	uint16_t player_chunk_idx;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_act;

	/* World */
	uint16_t chunk_cnt;
	chunk_t **chunks;
	chunk_buffer_t cb;
    HASHMAP(ivec2_chunk) *chunk_map;

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
extern void state_init_data(state_t *state);

#endif
