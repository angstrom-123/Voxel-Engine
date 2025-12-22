#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "renderer_base.h"
#include "data_structures.h"
#include "texture_handler.h"

#include "shaders/sprite.glsl.h"

#include <libem/em_bmp.h>

#define SPRITE_VERTEX_COUNT 4
#define SPRITE_INDEX_COUNT 6

typedef struct sprite_vertex {
    vec2 pos;
    vec2 uv;
    float z;
} sprite_vertex_t;

typedef struct sprite {
    offset_t *offset;
    bool removed;
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
} sprite_renderer_t;

typedef struct sprite_renderer_desc {
    size_t max_sprites;
    renderer_base_desc_t *base_desc;
} sprite_renderer_desc_t;

typedef struct sprite_desc {
    vec2 pos;
    vec2 size;
    float z_index;
    vec2 uv_offset;
} sprite_desc_t;

extern void sprite_renderer_init(sprite_renderer_t *sr, const sprite_renderer_desc_t *desc);
extern void sprite_renderer_load_textures(sprite_renderer_t *sr);
extern void sprite_renderer_cleanup(sprite_renderer_t *sr);
extern void sprite_renderer_render_all(sprite_renderer_t *sr);
extern sprite_t *sprite_renderer_push(sprite_renderer_t *sr, const sprite_desc_t *desc);
extern void sprite_renderer_pop(sprite_renderer_t *sr, sprite_t *sprite);

#endif
