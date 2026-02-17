#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H 

#include "geometry_types.h"
#include "renderer_base.h"
#include "data_structures.h"

#include "include_sokol.h"
#include <libem/em_math.h>

#include <threads.h>

#include "shaders/line.glsl.h"

#define LINE_VERTEX_COUNT 2
#define LINE_INDEX_COUNT LINE_VERTEX_COUNT

typedef struct line_vertex {
    vec3 pos;
    vec3 col;
} line_vertex_t;

typedef struct line {
    offset_t *offset;
    bool removed;
} line_t;

typedef struct line_renderer {
    CIRCULAR_QUEUE(offset) *offset_pool;
    bool needs_update;
    vec3 origin;
    line_t **lines;
    size_t max_lines;
    size_t line_count;
    renderer_base_t base;
    sg_buffer v_buf;
    sg_buffer i_buf;
    line_vertex_t *vbo;
    uint16_t *ibo;
    bool initialized;
} line_renderer_t;

typedef struct line_renderer_desc {
    size_t max_lines;
    const renderer_base_desc_t *base_desc;
} line_renderer_desc_t;

typedef struct line_desc {
    vec3 from;
    vec3 to;
    vec3 col;
} line_desc_t;

extern void line_renderer_init(line_renderer_t *lr, const line_renderer_desc_t *desc);
extern void line_renderer_cleanup(line_renderer_t *lr);
extern void line_renderer_render_all(line_renderer_t *lr);

extern line_t *line_renderer_push(line_renderer_t *lr, const line_desc_t *desc);
extern void line_renderer_push_all(line_renderer_t *lr, line_t **res, size_t count,
                                   const line_desc_t descs[]);
extern void line_renderer_pop(line_renderer_t *lr, line_t *line);

#endif
