#include "chunk_renderer.h"

void chunk_renderer_resize(chunk_renderer_t *cr, const vec2 dim) 
{
    cr->offscreen_base.dimensions = dim;
    cr->display_base.dimensions = dim;

    // Cleanup existing images and texture views.
    sg_destroy_image(cr->targets.colour);
    sg_destroy_image(cr->targets.normal);
    sg_destroy_image(cr->targets.depth);
    sg_destroy_image(cr->targets.shadow);
    sg_destroy_image(cr->targets.shadowmap);
    sg_destroy_image(cr->targets.zbuf);
    sg_destroy_image(cr->targets.zbuf_shadow);

    sg_destroy_view(cr->shadowmap_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->shadowmap_base.pass.attachments.depth_stencil);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[2]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[3]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.depth_stencil);

    sg_destroy_view(cr->offscreen_base.bind.views[1]);
    sg_destroy_view(cr->display_base.bind.views[0]);
    sg_destroy_view(cr->display_base.bind.views[1]);
    sg_destroy_view(cr->display_base.bind.views[2]);
    sg_destroy_view(cr->display_base.bind.views[3]);

    // Make new targets with the new dimensions.
    cr->targets = (struct targets) {
        .colour = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = COLOUR_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "offscreen-chunk-colour-image"
        }),
        .normal = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = NORMAL_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "offscreen-chunk-normal-image"
        }),
        .depth = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = DEPTH_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "offscreen-chunk-depth-image"
        }),
        .shadow = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = SHADOW_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "shadow-chunk-mask-image",
        }),
        .shadowmap = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = DEPTH_PIXELFORMAT,
            .width = cr->shadowmap_base.dimensions.x,  // Do not scale shadowmap.
            .height = cr->shadowmap_base.dimensions.y,
            .sample_count = 1,
            .label = "shadowmap-chunk-depth-image"
        }),
        .zbuf = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "chunk-zbuf-image"
        }),
        .zbuf_shadow = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .width = cr->shadowmap_base.dimensions.x,  // Do not scale shadowmap.
            .height = cr->shadowmap_base.dimensions.y,
            .sample_count = 1,
            .label = "chunk-shadowmap-image"
        })
    };


    // Make new target views from the new images
    cr->shadowmap_base.pass.attachments = (sg_attachments) {
        .colors[0] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.shadowmap },
            .label = "shadowmap-chunk-attachment",
        }),
        .depth_stencil = sg_make_view(&(sg_view_desc) {
            .depth_stencil_attachment = { .image = cr->targets.zbuf_shadow },
            .label = "shadowmap-chunk-zbuf-attachment",
        })
    };

    cr->offscreen_base.pass.attachments = (sg_attachments) {
        .colors[0] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.colour },
            .label = "offscreen-chunk-colour-attachment",
        }),
        .colors[1] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.normal },
            .label = "offscreen-chunk-normal-attachment"
        }),
        .colors[2] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.depth },
            .label = "offscreen-chunk-depth-attachment"
        }),
        .colors[3] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.shadow },
            .label = "offscreen-chunk-shadow-attachment"
        }),
        .depth_stencil = sg_make_view(&(sg_view_desc) {
            .depth_stencil_attachment = { .image = cr->targets.zbuf },
            .label = "offscreen-chunk-zbuf-attachment"
        })
    };

    // Binding at index 0 is reserved for the block textures.
    cr->offscreen_base.bind.views[1] = sg_make_view(&(sg_view_desc) {
        // .texture = { .image = cr->targets.zbuf_shadow },
        .texture = { .image = cr->targets.shadowmap },
        .label = "offscreen-chunk-shadowmap-view"
    });

    cr->display_base.bind.views[0] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.colour },
        .label = "display-chunk-colour-view",
    });
    cr->display_base.bind.views[1] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.normal },
        .label = "display-chunk-normal-view"
    });
    cr->display_base.bind.views[2] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.depth },
        .label = "display-chunk-depth-view"
    });
    cr->display_base.bind.views[3] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.shadow },
        .label = "display-chunk-shadow-view"
    });
}

void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc)
{
    rbase_init(&cr->offscreen_base, desc->base_desc);
    rbase_init(&cr->display_base, desc->base_desc);
    rbase_init(&cr->shadowmap_base, desc->shadowmap_base_desc);

    cr->inv_sun_dir = desc-> inv_sun_dir;

    // Inits the render targets and attachment views for the offscreen pass.
    chunk_renderer_resize(cr, desc->base_desc->dimensions);

    // Shadowmap pass 
    cr->shadowmap_base.pass.action = (sg_pass_action) {
        .colors = { 
            [0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = 1.0,
            }
        },
        // .depth = {
        //     .load_action = SG_LOADACTION_CLEAR,
        //     .store_action = SG_STOREACTION_STORE,
        //     .clear_value = 1.0
        // }
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = 1.0
        }
    };

    // Shadowmap pipeline
    cr->shadowmap_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(shadowmap_shader_desc(sg_query_backend())),
        // |Byte 1 |Byte 2 |Byte 3 |Byte 4 |
        // | 5 | 3 |       | 5 | 3 | 4 | 4 |
        // | x | o |   y   | z | n | u | v |
        .layout.attrs[ATTR_chunk_a_vertex].format = SG_VERTEXFORMAT_UBYTE4,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .sample_count = 1,
        .color_count = 1,
        .colors = { 
            [0].pixel_format = DEPTH_PIXELFORMAT,
        },
        // .colors[0].pixel_format = SG_PIXELFORMAT_NONE,
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            // .write_enabled = true
            .write_enabled = false
        }
    });

    // Offscreen pass
    cr->offscreen_base.pass.action = (sg_pass_action) {
        .colors = {
            [0] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 1.0 }
            },
            [1] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 1.0 }
            },
            [2] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 1.0 }
            },
            [3] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 1.0 }
            },
        },
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = 1.0
        }
    };

    // Offscreen pipeline
    cr->offscreen_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(chunk_shader_desc(sg_query_backend())),
        // |Byte 1 |Byte 2 |Byte 3 |Byte 4 |
        // |5  | 3 |       | 5 | 3 | 4 | 4 |
        // |x  | o |  y    | z | n | u | v |
        .layout.attrs[ATTR_chunk_a_vertex].format = SG_VERTEXFORMAT_UBYTE4,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .sample_count = 1,
        .color_count = 4,
        .colors = {
            [0].pixel_format = COLOUR_PIXELFORMAT,
            [1].pixel_format = NORMAL_PIXELFORMAT,
            [2].pixel_format = DEPTH_PIXELFORMAT,
            [3].pixel_format = SHADOW_PIXELFORMAT,
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    // Display pipeline 
    cr->display_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(composite_shader_desc(sg_query_backend())),
        .sample_count = 1,
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
        .layout.attrs[ATTR_composite_a_pos].format = SG_VERTEXFORMAT_FLOAT2
    });

    // For sampling the render textures from the offscreen pass
    float quad_verts[] = { 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
    cr->display_base.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
        .data = SG_RANGE(quad_verts)
    });
    cr->display_base.bind.samplers[SMP_u_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE
    });
}

void chunk_renderer_load_textures(chunk_renderer_t *cr)
{
    /* Bindings. */
    cr->offscreen_base.bind.samplers[SMP_u_smp_col] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .mipmap_filter = SG_FILTER_NEAREST,
        .max_lod = MIP_LEVELS,
    });
    cr->offscreen_base.bind.samplers[SMP_u_smp_sha] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        // .compare = SG_COMPAREFUNC_LESS,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE
    });

    texture_t atlas;
    bool res = texture_mip_load(&atlas, &(texture_desc_t) {
        .path = "res/tex/block/atlas-mipmap-",
        .mip_levels = MIP_LEVELS,
        .subimages_x = 16,
        .subimages_y = 16,
    });
    ENGINE_ASSERT(res, "Failed to load block texture atlas mipmaps");

    ENGINE_ASSERT(sg_query_view_state(atlas.as_view) == SG_RESOURCESTATE_VALID, 
                  "Mipmap texture view state must be valid");
    cr->offscreen_base.bind.views[0] = atlas.as_view;
}

void chunk_renderer_cleanup(chunk_renderer_t *cr)
{
    sg_destroy_view(cr->shadowmap_base.pass.attachments.depth_stencil);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[2]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[3]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.depth_stencil);

    sg_destroy_view(cr->offscreen_base.bind.views[1]);
    sg_destroy_view(cr->display_base.bind.views[0]);
    sg_destroy_view(cr->display_base.bind.views[1]);
    sg_destroy_view(cr->display_base.bind.views[2]);
    sg_destroy_view(cr->display_base.bind.views[3]);

    sg_destroy_image(cr->targets.colour);
    sg_destroy_image(cr->targets.normal);
    sg_destroy_image(cr->targets.depth);
    sg_destroy_image(cr->targets.shadow);
    sg_destroy_image(cr->targets.zbuf);
    sg_destroy_image(cr->targets.zbuf_shadow);

    sg_destroy_pipeline(cr->offscreen_base.pip);
    sg_destroy_pipeline(cr->display_base.pip);
}

static void _render_shadowmap_pass(chunk_renderer_t *cr) 
{
    renderer_base_t *rb = &cr->shadowmap_base;

    sg_begin_pass(&rb->pass);

    sg_apply_pipeline(rb->pip);

    for (size_t i = 0; i < cr->coords.num; i++)
    {
        ivec2 crd = em_add_ivec2(cr->coords.coords[i], cr->coords.offset);
        chunk_render_info_t *cri = GET_OR_NULL(cr->data.chunks, crd);
        if (!cri)
            continue;

        vec3 pos = { cri->pos.x, 0.0, cri->pos.y };
        vs_params_shadowmap_t vs_params = {
            .u_vp = rb->cam->vp,
            .u_ccord = pos,
        };

        if (cri->needs_update)
        {
            sg_update_buffer(cri->bufs.vertex, &(sg_range) {
                .ptr = cri->mesh->v_buf,
                .size = cri->mesh->v_cnt * sizeof(packed_vertex_t)
            });
            sg_update_buffer(cri->bufs.index, &(sg_range) {
                .ptr = cri->mesh->i_buf,
                .size = cri->mesh->i_cnt * sizeof(uint32_t)
            });

            cri->needs_update = false;
        }

        rb->bind.vertex_buffers[0] = cri->bufs.vertex;
        rb->bind.index_buffer = cri->bufs.index;
        sg_apply_bindings(&rb->bind);

        sg_apply_uniforms(UB_vs_params_shadowmap, &SG_RANGE(vs_params));

        sg_draw(0, cri->mesh->i_cnt, 1);
    }

    sg_end_pass();
}

static void _render_offscreen_pass(chunk_renderer_t *cr) 
{
    renderer_base_t *rb = &cr->offscreen_base;

    sg_begin_pass(&rb->pass);

    sg_apply_pipeline(rb->pip);

    for (size_t i = 0; i < cr->coords.num; i++)
    {
        ivec2 crd = em_add_ivec2(cr->coords.coords[i], cr->coords.offset);
        chunk_render_info_t *cri = GET_OR_NULL(cr->data.chunks, crd);
        if (!cri)
            continue;

        vec3 pos = { cri->pos.x, 0.0, cri->pos.y };
        vs_params_chunk_t vs_params = {
            .u_vp = rb->cam->vp,
            .u_ccord = pos,
            .u_lightspace = cr->shadowmap_base.cam->vp,
        };

        fs_params_chunk_t fs_params = {
            .u_sun_dir = em_mul_vec3_f(cr->inv_sun_dir, -1.0)
        };

        if (cri->needs_update)
        {
            sg_update_buffer(cri->bufs.vertex, &(sg_range) {
                .ptr = cri->mesh->v_buf,
                .size = cri->mesh->v_cnt * sizeof(packed_vertex_t)
            });
            sg_update_buffer(cri->bufs.index, &(sg_range) {
                .ptr = cri->mesh->i_buf,
                .size = cri->mesh->i_cnt * sizeof(uint32_t)
            });

            cri->needs_update = false;
        }

        rb->bind.vertex_buffers[0] = cri->bufs.vertex;
        rb->bind.index_buffer = cri->bufs.index;
        sg_apply_bindings(&rb->bind);

        sg_apply_uniforms(UB_vs_params_chunk, &SG_RANGE(vs_params));
        sg_apply_uniforms(UB_fs_params_chunk, &SG_RANGE(fs_params));

        sg_draw(0, cri->mesh->i_cnt, 1);
    }

    sg_end_pass();
}

static void _render_display_pass(chunk_renderer_t *cr)
{
    renderer_base_t *rb = &cr->display_base;

    sg_begin_pass(&(sg_pass) {
        .action = rb->pass.action,
        .swapchain = sglue_swapchain(),
    });

    fs_params_composite_t fs_params = {
        .u_inv_vp = em_inverse_mat4(cr->offscreen_base.cam->vp),
        .u_light_dir = VEC3(1.0, 5.0, 3.0),
        .u_eye_pos = cr->offscreen_base.cam->pos
    };

    sg_apply_pipeline(rb->pip);
    sg_apply_bindings(&rb->bind);

    sg_apply_uniforms(UB_fs_params_composite, &SG_RANGE(fs_params));

    // Render the fullscreen quad
    sg_draw(0, 4, 1);

    sg_end_pass();
}

void chunk_renderer_render_all(chunk_renderer_t *cr)
{
    // Synchronize main camera and shadow camera.
    camera_t *oc = cr->offscreen_base.cam;
    camera_t *sc = cr->shadowmap_base.cam;

    ivec3 tmp = em_floor_vec3(em_div_vec3_f(oc->pos, CHUNK_SIZE));
    vec3 target = em_mul_vec3_f(AS_VEC3(tmp), CHUNK_SIZE);
    target.y = sc->pos.y;

    mat4 transform = em_translate_mat4(em_mul_vec3_f(em_sub_vec3(target, sc->pos), -1.0));
    sc->view = em_mul_mat4(sc->view, transform);
    sc->pos = target;

    cam_update(sc);

    INSTRUMENT_SCOPE_BEGIN(render_shadowmap);
    _render_shadowmap_pass(cr);
    INSTRUMENT_SCOPE_END(render_shadowmap);
    INSTRUMENT_SCOPE_BEGIN(render_offscreen);
    _render_offscreen_pass(cr);
    INSTRUMENT_SCOPE_END(render_offscreen);
    INSTRUMENT_SCOPE_BEGIN(render_display);
    _render_display_pass(cr);
    INSTRUMENT_SCOPE_END(render_display);
}
