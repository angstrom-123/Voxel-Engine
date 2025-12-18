#include "sprite_renderer.h"

void sprite_renderer_init(sprite_renderer_t *sr, const sprite_renderer_desc_t *desc)
{
    sr->offset_pool = CIRCULAR_QUEUE_NEW(offset)(&(em_circular_queue_desc_t) {
        .cln_func = (void_cln_func) CIRCULAR_QUEUE_CLN(offset),
        .capacity = desc->max_sprites,
        .flags = EM_FLAG_NO_RESIZE
    });
    for (size_t i = 0; i < sr->offset_pool->size; i++)
    {
        sr->offset_pool->enqueue(sr->offset_pool, (offset_t) {
            .v_ofst = i * SPRITE_VERTEX_COUNT,
            .i_ofst = i * SPRITE_INDEX_COUNT
        });
    }

    sr->vbo = malloc(desc->max_sprites * SPRITE_VERTEX_COUNT * sizeof(sprite_vertex_t));
    sr->ibo = malloc(desc->max_sprites * SPRITE_INDEX_COUNT * sizeof(uint16_t));

    sr->v_buf = sg_make_buffer(&(sg_buffer_desc) {
        .size = desc->max_sprites *  SPRITE_VERTEX_COUNT * sizeof(sprite_vertex_t),
        .usage = {
            .vertex_buffer = true,
            .dynamic_update = true
        }
    });
    sr->i_buf = sg_make_buffer(&(sg_buffer_desc) {
        .size = desc->max_sprites *  SPRITE_INDEX_COUNT * sizeof(uint16_t),
        .usage = {
            .index_buffer = true,
            .dynamic_update = true
        }
    });

    rbase_init(&sr->base, desc->base_desc);

    sr->base.bind.vertex_buffers[0] = sr->v_buf;
    sr->base.bind.index_buffer = sr->i_buf;
    
    sr->needs_update = false;
    sr->max_sprites = desc->max_sprites;
    sr->sprites = calloc(desc->max_sprites, sizeof(sprite_t *));

    sr->base.pass = (sg_pass) {
        .action = {
            .colors[0] = {
                .load_action = SG_LOADACTION_LOAD,
                .clear_value = { 1.0, 0.0, 1.0, 1.0 }
            }
        },
        .label = "sprites-pass"
    };
    
    sr->base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(sprite_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                [ATTR_sprite_a_pos]     = { .format = SG_VERTEXFORMAT_FLOAT2 },
                [ATTR_sprite_a_uv]      = { .format = SG_VERTEXFORMAT_FLOAT2 },
                [ATTR_sprite_a_z_index] = { .format = SG_VERTEXFORMAT_FLOAT }
            }
        },
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_NONE,
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
        .blend_color = {1.0, 0.0, 0.0, 1.0},
        .colors[0] = {
            .blend = {
                .enabled = true,
                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .op_rgb = SG_BLENDOP_ADD,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .op_alpha = SG_BLENDOP_ADD
            }
        },
        .label = "sprite-pipeline"
    });

}

void sprite_renderer_load_textures(sprite_renderer_t *sr)
{
    /* Bindings. */
    sr->base.bind.samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .max_lod = 1.0
    });
    sr->base.bind.views[0] = sg_alloc_view();

    em_bmp_image_t crosshair;
    bool res = em_bmp_load(&crosshair, "res/tex/sprite/crosshair.bmp");
    ENGINE_ASSERT(res, "Failed to load texture for crosshair sprite");

    sg_image img = sg_alloc_image();
    sg_init_image(img, &(sg_image_desc) {
        .width = crosshair.ih.width,
        .height = crosshair.ih.height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .num_mipmaps = 1.0,
        .data.subimage[0] = {
            { .ptr = crosshair.pixel_data, .size = crosshair.ih.img_size }
        }
    });

    sg_init_view(sr->base.bind.views[0], &(sg_view_desc) {
        .texture.image = img
    });

    free(crosshair.pixel_data);
}

void sprite_renderer_cleanup(sprite_renderer_t *sr)
{
    sr->offset_pool->destroy(sr->offset_pool);
    sg_destroy_buffer(sr->v_buf);
    sg_destroy_buffer(sr->i_buf);
    free(sr->vbo);
    free(sr->ibo);
    for (size_t i = 0; i < sr->max_sprites; i++)
    {
        if (sr->sprites[i]) 
        {
            free(sr->sprites[i]->offset);
            free(sr->sprites[i]);
        }
    }
    free(sr->sprites);
}

void sprite_renderer_render_all(sprite_renderer_t *sr)
{
    sg_begin_pass(&(sg_pass) {
        .action = sr->base.pass.action,
        .swapchain = sglue_swapchain()
    });

    sg_apply_pipeline(sr->base.pip);

    if (sr->needs_update) 
    {
        sg_update_buffer(sr->v_buf, &(sg_range) {
            .ptr = sr->vbo,
            .size = sr->max_sprites * SPRITE_VERTEX_COUNT * sizeof(sprite_vertex_t)
        });
        sg_update_buffer(sr->i_buf, &(sg_range) {
            .ptr = sr->ibo,
            .size = sr->max_sprites * SPRITE_INDEX_COUNT * sizeof(uint16_t)
        });

        sr->needs_update = false;
    }

    for (size_t i = 0; i < sr->max_sprites; i++)
    {
        sprite_t *s = sr->sprites[i];
        if (!s)
            continue;

        if (s->removed)
        {
            free(sr->sprites[i]);
            sr->sprites[i] = NULL;
            continue;
        }

        vs_params_sprite_t vs_params = {
            .u_vp = sr->base.cam->vp
        };

        sg_apply_uniforms(UB_vs_params_sprite, &SG_RANGE(vs_params));

        sr->base.bind.vertex_buffer_offsets[0] = s->offset->v_ofst * sizeof(sprite_vertex_t);
        sr->base.bind.index_buffer_offset = s->offset->i_ofst * sizeof(uint16_t);
        sg_apply_bindings(&sr->base.bind);

        sg_draw(0, SPRITE_INDEX_COUNT, 1);
    }

    sg_end_pass();
}

sprite_t *sprite_renderer_push(sprite_renderer_t *sr, const sprite_desc_t *desc)
{
    ENGINE_ASSERT(sr->sprite_count < sr->max_sprites, "Maximum sprites reached in sprite renderer");

    sprite_t *s = malloc(sizeof(sprite_t));
    s->offset = sr->offset_pool->dequeue_ptr(sr->offset_pool);
    s->removed = false;

    /* Counter clockwise winding. */
    sr->vbo[s->offset->v_ofst] = (sprite_vertex_t) {
        .pos = desc->pos,
        .uv = {0.0, 0.0},
        .z = desc->z_index
    };
    sr->vbo[s->offset->v_ofst + 1] = (sprite_vertex_t) {
        .pos = em_add_vec2(desc->pos, (vec2) {desc->size.x, 0.0}),
        .uv = {1.0, 0.0},
        .z = desc->z_index
    };
    sr->vbo[s->offset->v_ofst + 2] = (sprite_vertex_t) {
        .pos = em_add_vec2(desc->pos, desc->size),
        .uv = {1.0, 1.0},
        .z = desc->z_index
    };
    sr->vbo[s->offset->v_ofst + 3] = (sprite_vertex_t) {
        .pos = em_add_vec2(desc->pos, (vec2) {0.0, desc->size.y}),
        .uv = {0.0, 1.0},
        .z = desc->z_index
    };

    sr->ibo[s->offset->i_ofst]     = 0;
    sr->ibo[s->offset->i_ofst + 1] = 2;
    sr->ibo[s->offset->i_ofst + 2] = 1;
    sr->ibo[s->offset->i_ofst + 3] = 0;
    sr->ibo[s->offset->i_ofst + 4] = 3;
    sr->ibo[s->offset->i_ofst + 5] = 2;

    uint16_t idx = s->offset->v_ofst / SPRITE_VERTEX_COUNT;
    sr->sprites[idx] = s;

    sr->needs_update = true;
    sr->sprite_count++;

    return s;
}

void sprite_renderer_pop(sprite_renderer_t *sr, sprite_t *s)
{
    sr->offset_pool->enqueue_ptr(sr->offset_pool, s->offset);
    s->removed = true;
}
