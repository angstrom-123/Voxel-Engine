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

#undef MY_DLL_IMPL
#include "list.h"

#include <libem/em_bmp.h>

#include "shaders/chunk.glsl.h"

#define NUM_SLOTS 512
#define V_STG_SIZE (V_MAX * NUM_SLOTS * sizeof(vertex_t))
#define I_STG_SIZE (I_MAX * NUM_SLOTS * sizeof(uint32_t))

typedef enum bucket_type {
    BUCKET_HOT = 0,
    BUCKET_COLD = 1,
    NUM_BUCKETS = 2
} bucket_type_e;

typedef struct bucket {
    HASHMAP(ivec2_chunk) *chunks;
    size_t upper;
} bucket_t;

typedef struct chunk_buffer {
	sg_buffer vbo;
	sg_buffer ibo;

	vertex_t *v_stg;
	uint32_t *i_stg;
} chunk_buffer_t;

typedef struct state {
	/* Global state */
	uint8_t tick;
    uint8_t l_tick;
    uint64_t frame;

    bool needs_update;

	/* Player */
	camera_t cam;
    chunk_t *curr_chunk;
    chunk_t *curr_north;
    chunk_t *curr_east;
    chunk_t *curr_south;
    chunk_t *curr_west;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_act;

	/* World */
	chunk_buffer_t cb;
    bucket_t buckets[NUM_BUCKETS];
    DLL(offset) *free_list;

	/* Input */
	bool key_down[SAPP_KEYCODE_MENU + 1];
    bool left_click;
    bool right_click;
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
