#include "line_renderer.h"

void line_renderer_init(line_renderer_t *lr, const line_renderer_desc_t *desc)
{
    rbase_init(&lr->base, desc->base_desc);

    lr->base.pass = (sg_pass) {
        .action = {
            .colors[0] = {
                .load_action = SG_LOADACTION_LOAD,
                .clear_value = { 1.0, 0.0, 1.0, 1.0 }
            },
        },
        .label = "lines-pass"
    };

    lr->base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(line_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                [ATTR_line_a_pos].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_line_a_col].format = SG_VERTEXFORMAT_FLOAT3,
            }
        },
        .primitive_type = SG_PRIMITIVETYPE_LINES,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_NONE,
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
        .label = "line-pipeline"
    });

    lr->offset_pool = CIRCULAR_QUEUE_NEW(offset)(&(em_circular_queue_desc_t) {
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(offset),
        .capacity = desc->max_lines,
        .flags = EM_FLAG_NO_RESIZE
    });
    for (size_t i = 0; i < lr->offset_pool->size; i++)
    {
        lr->offset_pool->enqueue(lr->offset_pool, (offset_t) {
            .v_ofst = i * LINE_VERTEX_COUNT,
            .i_ofst = i * LINE_INDEX_COUNT
        });
    }

    lr->vbo = malloc(desc->max_lines * LINE_VERTEX_COUNT * sizeof(line_vertex_t));
    lr->ibo = malloc(desc->max_lines * LINE_INDEX_COUNT * sizeof(uint16_t));

    lr->v_buf = sg_make_buffer(&(sg_buffer_desc) {
        .size = desc->max_lines *  LINE_VERTEX_COUNT * sizeof(line_vertex_t),
        .usage = {
            .vertex_buffer = true,
            .dynamic_update = true
        }
    });
    lr->i_buf = sg_make_buffer(&(sg_buffer_desc) {
        .size = desc->max_lines *  LINE_INDEX_COUNT * sizeof(uint16_t),
        .usage = {
            .index_buffer = true,
            .dynamic_update = true
        }
    });

    lr->base.bind.vertex_buffers[0] = lr->v_buf;
    lr->base.bind.index_buffer = lr->i_buf;
    
    lr->origin = (vec3) {0.0, 0.0, 0.0};
    lr->needs_update = false;
    lr->max_lines = desc->max_lines;
    lr->lines = calloc(desc->max_lines, sizeof(line_t *));
}

void line_renderer_cleanup(line_renderer_t *lr)
{
    lr->offset_pool->destroy(lr->offset_pool);
    sg_destroy_buffer(lr->v_buf);
    sg_destroy_buffer(lr->i_buf);
    free(lr->vbo);
    free(lr->ibo);
    for (size_t i = 0; i < lr->max_lines; i++)
    {
        if (lr->lines[i]) 
        {
            free(lr->lines[i]->offset);
            free(lr->lines[i]);
        }
    }
    free(lr->lines);
}

void line_renderer_render_all(line_renderer_t *lr)
{
    sg_begin_pass(&(sg_pass) {
        .action = lr->base.pass.action,
        .swapchain = sglue_swapchain(),
        .label = "line-pass"
    });

    sg_apply_pipeline(lr->base.pip);

    if (lr->needs_update) 
    {
        sg_update_buffer(lr->v_buf, &(sg_range) {
            .ptr = lr->vbo,
            .size = lr->max_lines * LINE_VERTEX_COUNT * sizeof(line_vertex_t)
        });
        sg_update_buffer(lr->i_buf, &(sg_range) {
            .ptr = lr->ibo,
            .size = lr->max_lines * LINE_INDEX_COUNT * sizeof(uint16_t)
        });

        lr->needs_update = false;
    }

    for (size_t i = 0; i < lr->max_lines; i++)
    {
        line_t *l = lr->lines[i];
        if (!l)
            continue;

        if (l->removed)
        {
            free(lr->lines[i]);
            lr->lines[i] = NULL;
            continue;
        }

        vs_params_line_t vs_params = {
            .u_mvp = lr->base.cam->vp,
            .u_offset = lr->origin
        };

        sg_apply_uniforms(UB_vs_params_line, &SG_RANGE(vs_params));

        lr->base.bind.vertex_buffer_offsets[0] = l->offset->v_ofst * sizeof(line_vertex_t);
        lr->base.bind.index_buffer_offset = l->offset->i_ofst * sizeof(uint16_t);
        sg_apply_bindings(&lr->base.bind);

        sg_draw(0, LINE_INDEX_COUNT, 1);
    }

    sg_end_pass();
}

line_t *line_renderer_push(line_renderer_t *lr, const line_desc_t *desc)
{
    ENGINE_ASSERT(lr->line_count < lr->max_lines, "Maximum lines reached in line renderer");

    line_t *l = malloc(sizeof(line_t));
    l->offset = lr->offset_pool->dequeue_ptr(lr->offset_pool);
    l->removed = false;

    lr->vbo[l->offset->v_ofst] = (line_vertex_t) {
        .pos = desc->from,
        .col = desc->col
    };
    lr->vbo[l->offset->v_ofst + 1] = (line_vertex_t) {
        .pos = desc->to,
        .col = desc->col
    };

    lr->ibo[l->offset->i_ofst] = 0;
    lr->ibo[l->offset->i_ofst + 1] = 1;

    uint16_t idx = l->offset->v_ofst / LINE_VERTEX_COUNT;
    lr->lines[idx] = l;

    lr->needs_update = true;
    lr->line_count++;

    return l;
}

void line_renderer_push_all(line_renderer_t *lr, line_t **res, size_t count,
                            const line_desc_t descs[])
{
    for (size_t i = 0; i < count; i++)
    {
        line_t *l = line_renderer_push(lr, &descs[i]);
        if (res) res[i] = l;
    }
}

void line_renderer_pop(line_renderer_t *lr, line_t *line)
{
    lr->offset_pool->enqueue_ptr(lr->offset_pool, line->offset);
    line->removed = true;
}
