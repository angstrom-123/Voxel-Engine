#ifndef STATE_H
#define STATE_H

#if !defined(SOKOL_GFX_INCLUDED) 
#include <sokol/sokol_gfx.h>
#endif

#include <stdint.h>

#include <libem/em_bmp.h>

#include "camera.h"
#include "geometry.h"
#include "constants.h"
#include "chunk_system.h"
#include "load_manager.h"

#include "chunk.glsl.h"

#define NUM_SLOTS 1024
#define V_STG_SIZE (V_MAX * NUM_SLOTS * sizeof(vertex_t))
#define I_STG_SIZE (I_MAX * NUM_SLOTS * sizeof(uint32_t))

typedef enum game_state {
    GAME_RUN,
    GAME_PAUSE,
    GAME_MENUMAIN,
    GAME_MENULOAD,
    GAME_MENUNEW,
    GAME_MENUOPT
} game_state_e;

typedef enum popup {
    POPUP_NONE,
    POPUP_CONFIRM,
    POPUP_EDIT_CONFIRM
} popup_e;

typedef struct game_desc {
    popup_e popup;
    game_state_e game_state;
    uvec2 window;
    char selected_str[MAX_WORLD_NAME_LEN];
    char world_str[MAX_WORLD_NAME_LEN];
    char seed_str[MAX_SEED_LEN];
    uint32_t seed;
} game_desc_t;

typedef struct state {
	/* Global state */
    game_desc_t game_desc;
    size_t render_dist;

    /* Frame counters */
	uint8_t tick;
    uint8_t l_tick;
    uint64_t frame;

	/* Player */
	camera_t cam;

	/* Render */
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_act;
    atomic_uint_least64_t render_generation;

	/* World */
    chunk_system_t cs;
    load_manager_t lm;

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
extern void state_init_systems(state_t *state);

#endif
