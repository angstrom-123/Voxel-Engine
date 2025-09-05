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

#define MAX_NAME_CHARS 24
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

typedef enum menu_state {
    MENU_MAIN,
    MENU_LOAD,
    MENU_NEW,
    MENU_PLAY,
    MENU_OPT
} menu_state_e;

typedef struct menu_desc {
    size_t win_width;
    size_t win_height;
    menu_state_e state;
    char name_buf[24];
    char seed_buf[32];
    char selected_name[24];
} menu_desc_t;

typedef enum game_state {
    GAME_MENU,
    GAME_RUN,
    GAME_PAUSE
} game_state_e;

typedef struct state {
    /* Main Menu. */
    menu_desc_t menu_desc;

	/* Global state */
    game_state_e game_state;
    char *dir_name;
    uint32_t seed;
    bool needs_update;

    /* Frame counters */
	uint8_t tick;
    uint8_t l_tick;
    uint64_t frame;

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
