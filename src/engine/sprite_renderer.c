#include "sprite_renderer.h"

vec2 _uv_lookup(sprite_renderer_t *sr, char c)
{
    const texture_t t = sr->textures[SPRITETEX_FONT];

    const uint8_t START = ' ';
    const uint8_t UNKNOWN = 95;

    uint8_t idx = em_clamp(c - START, 0, UNKNOWN);
    float y = floorf((float) idx / (float) t.subimages_x);
    float x = idx - (y * t.subimages_x);
    vec2 d = texture_query_subimage_uv(&t);

    return em_mul_vec2(d, VEC2(x, t.subimages_y - y - 1));
}

vec2 _sprite_dimensions(sprite_renderer_t *sr, sprite_t *s)
{
    // Subtracting vertex positions of opposite corners.
    return em_sub_vec2(sr->vbo[s->offset->v_ofst + 2].pos, sr->vbo[s->offset->v_ofst].pos);
}

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
                .clear_value = { 0.0, 0.0, 0.0, 1.0 }
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
    ENGINE_TODO("Make an atlas for sprite textures such as crosshair");

    bool res;
    res = texture_load(&sr->textures[SPRITETEX_ATLAS], &(texture_desc_t) {
        .path = "res/tex/sprite/crosshair"
    });
    ENGINE_ASSERT(res, "Failed to load sprite atlas texture");

    res = texture_load(&sr->textures[SPRITETEX_FONT], &(texture_desc_t) {
        .path = "res/tex/sprite/font-atlas",
        .subimages_x = 10,
        .subimages_y = 10
    });
    ENGINE_ASSERT(res, "Failed to load font atlas texture");

    sr->base.bind.views[VIEW_u_tex_sprite_atlas] = sr->textures[SPRITETEX_ATLAS].as_view;
    sr->base.bind.views[VIEW_u_tex_font_atlas] = sr->textures[SPRITETEX_FONT].as_view;
    sr->base.bind.samplers[SMP_u_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .max_lod = 1.0
    });
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

        fs_params_sprite_t fs_params = {
            .u_is_char = s->is_char,
            .u_bg = s->bg_col
        };

        sg_apply_uniforms(UB_vs_params_sprite, &SG_RANGE(vs_params));
        sg_apply_uniforms(UB_fs_params_sprite, &SG_RANGE(fs_params));

        sr->base.bind.vertex_buffer_offsets[0] = s->offset->v_ofst * sizeof(sprite_vertex_t);
        sr->base.bind.index_buffer_offset = s->offset->i_ofst * sizeof(uint16_t);
        sg_apply_bindings(&sr->base.bind);

        sg_draw(0, SPRITE_INDEX_COUNT, 1);
    }

    sg_end_pass();
}

void sprite_renderer_move(sprite_renderer_t *sr, sprite_t *s, vec2 pos)
{
    vec2 d = _sprite_dimensions(sr, s);
    sr->vbo[s->offset->v_ofst].pos = pos;
    sr->vbo[s->offset->v_ofst + 1].pos = em_add_vec2(pos, VEC2(d.x, 0.0));
    sr->vbo[s->offset->v_ofst + 2].pos = em_add_vec2(pos, d);
    sr->vbo[s->offset->v_ofst + 3].pos = em_add_vec2(pos, VEC2(0.0, d.y));
    sr->needs_update = true;
}

void sprite_renderer_move_str(sprite_renderer_t *sr, sprite_t **sprites, size_t len, vec2 pos)
{
    vec2 d = _sprite_dimensions(sr, sprites[0]);
    vec2 spr_pos = pos;
    for (size_t i = 0; i < len; i++)
    {
        sprite_t *s = sprites[i];
        ENGINE_ASSERT(s->is_char, "Sprite must be a character to move it as a string");

        sr->vbo[s->offset->v_ofst].pos = spr_pos;
        sr->vbo[s->offset->v_ofst + 1].pos = em_add_vec2(spr_pos, VEC2(d.x, 0.0));
        sr->vbo[s->offset->v_ofst + 2].pos = em_add_vec2(spr_pos, d);
        sr->vbo[s->offset->v_ofst + 3].pos = em_add_vec2(spr_pos, VEC2(0.0, d.y));

        spr_pos.x += d.x;
    }
    sr->needs_update = true;
}

void sprite_renderer_change_char(sprite_renderer_t *sr, sprite_t *s, char c)
{
    ENGINE_ASSERT(s->is_char, "Sprite must be a character to change its char");
    vec2 uv_scale = texture_query_subimage_uv(&sr->textures[SPRITETEX_FONT]);
    vec2 uv = _uv_lookup(sr, c);
    sr->vbo[s->offset->v_ofst].uv = em_add_vec2(VEC2(0.0, 0.0), uv);
    sr->vbo[s->offset->v_ofst + 1].uv = em_add_vec2(VEC2(uv_scale.x, 0.0), uv);
    sr->vbo[s->offset->v_ofst + 2].uv = em_add_vec2(uv_scale, uv);
    sr->vbo[s->offset->v_ofst + 3].uv = em_add_vec2(VEC2(0.0, uv_scale.y), uv);
    sr->needs_update = true;
}

void sprite_renderer_change_str(sprite_renderer_t *sr, sprite_t **sprites, const char *str)
{
    vec2 uv_scale = texture_query_subimage_uv(&sr->textures[SPRITETEX_FONT]);

    char c;
    for (size_t i = 0; (c = str[i]); i++)
    {
        sprite_t *s = sprites[i];
        ENGINE_ASSERT(s->is_char, "Sprite must be a character to change its char");

        vec2 uv = _uv_lookup(sr, c);
        sr->vbo[s->offset->v_ofst].uv = em_add_vec2(VEC2(0.0, 0.0), uv);
        sr->vbo[s->offset->v_ofst + 1].uv = em_add_vec2(VEC2(uv_scale.x, 0.0), uv);
        sr->vbo[s->offset->v_ofst + 2].uv = em_add_vec2(uv_scale, uv);
        sr->vbo[s->offset->v_ofst + 3].uv = em_add_vec2(VEC2(0.0, uv_scale.y), uv);
    }

    sr->needs_update = true;
}

sprite_t **sprite_renderer_push_str(sprite_renderer_t *sr, const char *str,
                                    const sprite_desc_t *desc)
{
    const size_t len = strlen(str);
    ENGINE_ASSERT(sr->sprite_count + len <= sr->max_sprites,
                  "String length exceeds maximum sprite count");

    sprite_t **res = malloc(len * sizeof(sprite_t *));
    sprite_desc_t d = *desc;
    d.is_char = true;

    for (size_t i = 0; i < len; i++)
    {
        d.uv_offset = _uv_lookup(sr, str[i]);
        res[i] = sprite_renderer_push(sr, &d);
        d.pos.x += d.size.x;
    }

    return res;
}

sprite_t *sprite_renderer_push(sprite_renderer_t *sr, const sprite_desc_t *desc)
{
    ENGINE_ASSERT(sr->sprite_count < sr->max_sprites, "Maximum sprites reached in sprite renderer");

    sprite_t *s = malloc(sizeof(sprite_t));
    s->bg_col = desc->bg_col;
    s->offset = sr->offset_pool->dequeue_ptr(sr->offset_pool);
    s->removed = false;
    s->is_char = desc->is_char;

    vec2 uv_scale = (desc->is_char)
                  ? texture_query_subimage_uv(&sr->textures[SPRITETEX_FONT])
                  : texture_query_subimage_uv(&sr->textures[SPRITETEX_ATLAS]);

    /* Counter clockwise winding. */
    sr->vbo[s->offset->v_ofst] = (sprite_vertex_t) {
        .pos = desc->pos,
        .uv = em_add_vec2(VEC2(0.0, 0.0), desc->uv_offset),
        .z = desc->z_index
    };
    sr->vbo[s->offset->v_ofst + 1] = (sprite_vertex_t) {
        .pos = em_add_vec2(desc->pos, VEC2(desc->size.x, 0.0)),
        .uv = em_add_vec2(VEC2(uv_scale.x, 0.0), desc->uv_offset),
        .z = desc->z_index
    };
    sr->vbo[s->offset->v_ofst + 2] = (sprite_vertex_t) {
        .pos = em_add_vec2(desc->pos, desc->size),
        .uv = em_add_vec2(uv_scale, desc->uv_offset),
        .z = desc->z_index
    };
    sr->vbo[s->offset->v_ofst + 3] = (sprite_vertex_t) {
        .pos = em_add_vec2(desc->pos, VEC2(0.0, desc->size.y)),
        .uv = em_add_vec2(VEC2(0.0, uv_scale.y), desc->uv_offset),
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
