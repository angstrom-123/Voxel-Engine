#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "renderer_base.h"
#include "data_structures.h"
#include "texture_handler.h"

#include "shaders/sprite.glsl.h"

#include <libem/em_bmp.h>

#define SPRITE_VERTEX_COUNT 4
#define SPRITE_INDEX_COUNT 6

typedef enum sprite_texture {
    SPRITETEX_ATLAS,
    SPRITETEX_FONT,
    SPRITETEX_NUM
} sprite_texture_e;

typedef struct sprite_vertex {
    vec2 pos;
    vec2 uv;
    float z;
} sprite_vertex_t;

typedef struct sprite {
    offset_t *offset;
    vec4 bg_col;
    bool visible;
    bool removed;
    bool is_char;
} sprite_t;

typedef struct sprite_renderer {
    CIRCULAR_QUEUE(offset) *offset_pool;
    bool needs_update;
    sprite_t **sprites;
    size_t max_sprites;
    size_t sprite_count;
    renderer_base_t base;
    sg_buffer v_buf;
    sg_buffer i_buf;
    sprite_vertex_t *vbo;
    uint16_t *ibo;
    texture_t textures[SPRITETEX_NUM];
} sprite_renderer_t;

typedef struct sprite_renderer_desc {
    size_t max_sprites;
    renderer_base_desc_t *base_desc;
} sprite_renderer_desc_t;

typedef struct sprite_desc {
    vec4 bg_col;
    vec2 pos;
    vec2 size;
    float z_index;
    vec2 uv_offset;
    bool is_char;
    bool visible;
} sprite_desc_t;

typedef enum icon_id {
    IID_BLOCK_NONE,
    IID_BLOCK_GRASS,
    IID_BLOCK_DIRT,
    IID_BLOCK_STONE,
    IID_BLOCK_SAND,
    IID_BLOCK_LOG,
    IID_BLOCK_LEAF,
    IID_BLOCK_LOG_P,
    IID_BLOCK_LEAF_P,

    IID_CROSSHAIR = 128,
    IID_SLOT,
    IID_SLOT_SELECTED,
    IID_CORNER_TL_HARD,
    IID_CORNER_TR_HARD,
    IID_CORNER_BL_HARD,
    IID_CORNER_BR_HARD,
    IID_EDGE_L,
    IID_EDGE_R,
    IID_EDGE_B,
    IID_EDGE_T,
    IID_CORNER_TL_SOFT,
    IID_CORNER_TR_SOFT,
    IID_CORNER_BL_SOFT,
    IID_CORNER_BR_SOFT,
    IID_BG_TINT,
} icon_id_e;

typedef struct ui_sprites_desc {
    vec2 pos;
    vec2 size;
    uvec2 dim;
    float z_index;
    bool visible;
    vec4 bg_col;
    bool rounded;
    bool mini;
} ui_sprites_desc_t;

// TODO: Make the background colour of sprites a sentinel value to be replaced during 
//       rendering by the background colour.

extern void sprite_renderer_init(sprite_renderer_t *sr, const sprite_renderer_desc_t *desc);
extern void sprite_renderer_load_textures(sprite_renderer_t *sr);
extern void sprite_renderer_cleanup(sprite_renderer_t *sr);
extern void sprite_renderer_render_all(sprite_renderer_t *sr);
extern void sprite_renderer_move(sprite_renderer_t *sr, sprite_t *s, vec2 pos);
extern void sprite_renderer_move_str(sprite_renderer_t *sr, sprite_t **sprites, 
                                     size_t len, vec2 pos);
extern void sprite_renderer_change_str(sprite_renderer_t *sr, sprite_t **sprites, 
                                       const char *str);
extern void sprite_renderer_change_char(sprite_renderer_t *sr, sprite_t *s, char c);
extern vec2 sprite_icon_uv_offset(sprite_renderer_t *sr, icon_id_e id);
extern sprite_t **sprite_renderer_push_str(sprite_renderer_t *sr, const char *str, 
                                           const sprite_desc_t *desc);
extern sprite_t **sprite_renderer_push_ui(sprite_renderer_t *sr, const ui_sprites_desc_t *desc);
extern sprite_t *sprite_renderer_push(sprite_renderer_t *sr, const sprite_desc_t *desc);
extern void sprite_renderer_pop(sprite_renderer_t *sr, sprite_t *sprite);

#endif
