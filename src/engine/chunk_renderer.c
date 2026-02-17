#include "chunk_renderer.h"

// #define DEBUG_SHADOWS

void chunk_renderer_resize(chunk_renderer_t *cr, const vec2 dim) 
{
    cr->offscreen_base.dimensions = dim;
    cr->composite_base.dimensions = dim;

    // Cleanup existing images and texture views.
    sg_destroy_image(cr->targets.albedo);
    sg_destroy_image(cr->targets.normal);
    sg_destroy_image(cr->targets.depth);
    sg_destroy_image(cr->targets.shadow);
    sg_destroy_image(cr->targets.shadowmap);
    sg_destroy_image(cr->targets.colour);
    sg_destroy_image(cr->targets.skybox);
    sg_destroy_image(cr->targets.zbuf);
    sg_destroy_image(cr->targets.zbuf_shadow);
    sg_destroy_image(cr->targets.zbuf_skybox);
    sg_destroy_image(cr->targets.zbuf_composite);

    sg_destroy_view(cr->shadowmap_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->shadowmap_base.pass.attachments.depth_stencil);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[2]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[3]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.depth_stencil);
    sg_destroy_view(cr->skybox_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->skybox_base.pass.attachments.depth_stencil);
    sg_destroy_view(cr->composite_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->composite_base.pass.attachments.depth_stencil);

    sg_destroy_view(cr->offscreen_base.bind.views[VIEW_u_shadow_map]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_galbedo]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_gnormal_composite]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_gdepth_composite]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_gshadow]);
    #ifndef DEBUG_SHADOWS
    sg_destroy_view(cr->effects_base.bind.views[VIEW_u_gnormal_effects]);
    sg_destroy_view(cr->effects_base.bind.views[VIEW_u_gdepth_effects]);
    #endif

    // Make new targets with the new dimensions.
    cr->targets = (struct targets) {
        .albedo = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = COLOUR_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
        .normal = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = NORMAL_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
        .depth = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = DEPTH_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
        .shadow = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = SHADOW_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
        .shadowmap = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = DEPTH_PIXELFORMAT,
            .width = cr->shadowmap_base.dimensions.x,  // Do not scale shadowmap.
            .height = cr->shadowmap_base.dimensions.y,
            .sample_count = 1,
        }),
        .skybox = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = COLOUR_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1
        }),
        .colour = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = COLOUR_PIXELFORMAT,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1
        }),
        .zbuf = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
        .zbuf_shadow = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .width = cr->shadowmap_base.dimensions.x,  // Do not scale shadowmap.
            .height = cr->shadowmap_base.dimensions.y,
            .sample_count = 1,
        }),
        .zbuf_skybox = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
        .zbuf_composite = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
        }),
    };

    cr->info.ssao_noise_image = sg_make_image(&(sg_image_desc) {
        .data = SG_RANGE(cr->info.ssao_noise_data),
        .sample_count = 1,
        .pixel_format = DATA_PIXELFORMAT,
        .width = SSAO_NOISE_SCALE,
        .height = SSAO_NOISE_SCALE,
        .type = SG_IMAGETYPE_2D
    });

    // Make new target views from the new images
    cr->shadowmap_base.pass.attachments = (sg_attachments) {
        .colors[0] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.shadowmap }
        }),
        .depth_stencil = sg_make_view(&(sg_view_desc) {
            .depth_stencil_attachment = { .image = cr->targets.zbuf_shadow }
        })
    };

    cr->skybox_base.pass.attachments = (sg_attachments) {
        .colors[0] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.skybox }
        }),
        .depth_stencil = sg_make_view(&(sg_view_desc) {
            .depth_stencil_attachment = { .image = cr->targets.zbuf_skybox }
        })
    };

    cr->offscreen_base.pass.attachments = (sg_attachments) {
        .colors[0] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.albedo }
        }),
        .colors[1] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.normal }
        }),
        .colors[2] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.depth }
        }),
        .colors[3] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.shadow }
        }),
        .depth_stencil = sg_make_view(&(sg_view_desc) {
            .depth_stencil_attachment = { .image = cr->targets.zbuf }
        })
    };

    cr->composite_base.pass.attachments = (sg_attachments) {
        .colors[0] = sg_make_view(&(sg_view_desc) {
            .color_attachment = { .image = cr->targets.colour }
        }),
        .depth_stencil = sg_make_view(&(sg_view_desc) {
            .depth_stencil_attachment = { .image = cr->targets.zbuf_composite }
        })
    };

    cr->offscreen_base.bind.views[VIEW_u_shadow_map] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.shadowmap },
    });

    cr->composite_base.bind.views[VIEW_u_galbedo] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.albedo },
    });
    cr->composite_base.bind.views[VIEW_u_gnormal_composite] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.normal },
    });
    cr->composite_base.bind.views[VIEW_u_gdepth_composite] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.depth },
    });
    cr->composite_base.bind.views[VIEW_u_gshadow] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.shadow },
    });

    #ifdef DEBUG_SHADOWS
    cr->effects_base.bind.views[VIEW_u_effects_shadow_map] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.shadowmap }
    });
    #else

    cr->effects_base.bind.views[VIEW_u_colour] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.colour }
    });
    cr->effects_base.bind.views[VIEW_u_gnormal_effects] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.normal }
    });
    cr->effects_base.bind.views[VIEW_u_gdepth_effects] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.depth }
    });
    cr->effects_base.bind.views[VIEW_u_skybox] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->targets.skybox }
    });
    cr->effects_base.bind.views[VIEW_u_ssao_noise] = sg_make_view(&(sg_view_desc) {
        .texture = { .image = cr->info.ssao_noise_image }
    });
    #endif
}

void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc)
{
    rbase_init(&cr->offscreen_base, desc->base_desc);
    rbase_init(&cr->composite_base, desc->base_desc);
    rbase_init(&cr->effects_base, desc->base_desc);
    rbase_init(&cr->skybox_base, desc->base_desc);
    rbase_init(&cr->shadowmap_base, desc->shadowmap_base_desc);

    cr->info.sun_dir = desc->sun_dir;
    cr->info.view_distance = desc->view_distance;

    // Inits the render targets and attachment views for the offscreen pass.
    chunk_renderer_resize(cr, desc->base_desc->dimensions);

    // SSAO kernel
    em_romu_duo_state_t s;
    em_romu_duo_init(&s, time(NULL));

    for (size_t i = 0; i < SSAO_SAMPLES; i++)
    {
        vec3 sample = em_normalize_vec3(VEC3(
            ((float) em_romu_duo_random(&s) / EM_ROMU_DUO_MAXF) * 2.0 - 1.0,
            ((float) em_romu_duo_random(&s) / EM_ROMU_DUO_MAXF) * 2.0 - 1.0,
            ((float) em_romu_duo_random(&s) / EM_ROMU_DUO_MAXF)
        ));
        float scale = 0.1 + em_sqr((float) i / SSAO_SAMPLES) * 0.9;
        sample = em_mul_vec3_f(sample, scale);
        cr->info.ssao_kernel[i] = VEC4(sample.x, sample.y, sample.z, 0.0);
    }

    // SSAO noise (for random kernel rotations)
    for (size_t i = 0; i < em_sqr(SSAO_NOISE_SCALE); i++)
    {
        vec2 noise = {
            ((float) em_romu_duo_random(&s) / EM_ROMU_DUO_MAXF) * 2.0 - 1.0,
            ((float) em_romu_duo_random(&s) / EM_ROMU_DUO_MAXF) * 2.0 - 1.0,
        };
        cr->info.ssao_noise_data[i] = noise;
    }

    // Shadowmap pass 
    cr->shadowmap_base.pass.action = (sg_pass_action) {
        .colors = { 
            [0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = 1.0
            }
        },
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
        .layout.attrs[ATTR_offscreen_a_vert].format = SG_VERTEXFORMAT_UBYTE4,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .sample_count = 1,
        .color_count = 1,
        .colors = { 
            [0].pixel_format = DEPTH_PIXELFORMAT,
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = false
        }
    });

    // Offscreen pass
    cr->offscreen_base.pass.action = (sg_pass_action) {
        .colors = {
            [0] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 0.0 },
            },
            [1] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 0.0 }
            },
            [2] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 0.0 }
            },
            [3] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 0.0, 0.0, 0.0, 0.0 }
            },
        },
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = 1.0
        }
    };

    // Offscreen pipeline
    cr->offscreen_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(offscreen_shader_desc(sg_query_backend())),
        .layout.attrs[ATTR_offscreen_a_vert].format = SG_VERTEXFORMAT_UBYTE4,
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

    // Skybox pass
    cr->skybox_base.pass.action = (sg_pass_action) {
        .colors = {
            [0] = { 
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = { 1.0, 1.0, 0.0, 1.0 },
            }
        },
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = 1.0
        }
    };

    // Skybox pipeline
    cr->skybox_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(skybox_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                [ATTR_skybox_a_pos] = { .format = SG_VERTEXFORMAT_FLOAT3 },
                [ATTR_skybox_a_nrm] = { .format = SG_VERTEXFORMAT_FLOAT3 },
                [ATTR_skybox_a_uv]  = { .format = SG_VERTEXFORMAT_FLOAT2 }
            }
        },
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .sample_count = 1,
        .color_count = 1,
        .colors = {
            [0].pixel_format = COLOUR_PIXELFORMAT
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    const float DIM = 400.0;
    skybox_vertex_t skybox_verts[] = { 
        { .pos = VEC3(-DIM, -DIM, -DIM), .nrm = VEC3( 0.0,  0.0,  1.0), .uv = VEC2(0.0, 0.0) },
        { .pos = VEC3( DIM, -DIM, -DIM), .nrm = VEC3( 0.0,  0.0,  1.0), .uv = VEC2(1.0, 0.0) },
        { .pos = VEC3( DIM,  DIM, -DIM), .nrm = VEC3( 0.0,  0.0,  1.0), .uv = VEC2(1.0, 1.0) },
        { .pos = VEC3(-DIM,  DIM, -DIM), .nrm = VEC3( 0.0,  0.0,  1.0), .uv = VEC2(0.0, 1.0) },
        { .pos = VEC3( DIM, -DIM,  DIM), .nrm = VEC3( 0.0,  0.0, -1.0), .uv = VEC2(0.0, 0.0) },
        { .pos = VEC3(-DIM, -DIM,  DIM), .nrm = VEC3( 0.0,  0.0, -1.0), .uv = VEC2(1.0, 0.0) },
        { .pos = VEC3(-DIM,  DIM,  DIM), .nrm = VEC3( 0.0,  0.0, -1.0), .uv = VEC2(1.0, 1.0) },
        { .pos = VEC3( DIM,  DIM,  DIM), .nrm = VEC3( 0.0,  0.0, -1.0), .uv = VEC2(0.0, 1.0) },
        { .pos = VEC3(-DIM, -DIM,  DIM), .nrm = VEC3( 1.0,  0.0,  0.0), .uv = VEC2(0.0, 0.0) },
        { .pos = VEC3(-DIM, -DIM, -DIM), .nrm = VEC3( 1.0,  0.0,  0.0), .uv = VEC2(1.0, 0.0) },
        { .pos = VEC3(-DIM,  DIM, -DIM), .nrm = VEC3( 1.0,  0.0,  0.0), .uv = VEC2(1.0, 1.0) },
        { .pos = VEC3(-DIM,  DIM,  DIM), .nrm = VEC3( 1.0,  0.0,  0.0), .uv = VEC2(0.0, 1.0) },
        { .pos = VEC3( DIM, -DIM, -DIM), .nrm = VEC3(-1.0,  0.0,  0.0), .uv = VEC2(0.0, 0.0) },
        { .pos = VEC3( DIM, -DIM,  DIM), .nrm = VEC3(-1.0,  0.0,  0.0), .uv = VEC2(1.0, 0.0) },
        { .pos = VEC3( DIM,  DIM,  DIM), .nrm = VEC3(-1.0,  0.0,  0.0), .uv = VEC2(1.0, 1.0) },
        { .pos = VEC3( DIM,  DIM, -DIM), .nrm = VEC3(-1.0,  0.0,  0.0), .uv = VEC2(0.0, 1.0) },
        { .pos = VEC3(-DIM, -DIM,  DIM), .nrm = VEC3( 0.0,  1.0,  0.0), .uv = VEC2(0.0, 0.0) },
        { .pos = VEC3( DIM, -DIM,  DIM), .nrm = VEC3( 0.0,  1.0,  0.0), .uv = VEC2(1.0, 0.0) },
        { .pos = VEC3( DIM, -DIM, -DIM), .nrm = VEC3( 0.0,  1.0,  0.0), .uv = VEC2(1.0, 1.0) },
        { .pos = VEC3(-DIM, -DIM, -DIM), .nrm = VEC3( 0.0,  1.0,  0.0), .uv = VEC2(0.0, 1.0) },
        { .pos = VEC3(-DIM,  DIM, -DIM), .nrm = VEC3( 0.0, -1.0,  0.0), .uv = VEC2(0.0, 0.0) },
        { .pos = VEC3( DIM,  DIM, -DIM), .nrm = VEC3( 0.0, -1.0,  0.0), .uv = VEC2(1.0, 0.0) },
        { .pos = VEC3( DIM,  DIM,  DIM), .nrm = VEC3( 0.0, -1.0,  0.0), .uv = VEC2(1.0, 1.0) },
        { .pos = VEC3(-DIM,  DIM,  DIM), .nrm = VEC3( 0.0, -1.0,  0.0), .uv = VEC2(0.0, 1.0) }
    };
    uint16_t skybox_indices[] = {
        0, 2, 1,
        0, 3, 2,
        4, 6, 5,
        4, 7, 6,
        8, 10, 9,
        8, 11, 10,
        12, 14, 13,
        12, 15, 14,
        16, 18, 17,
        16, 19, 18,
        20, 22, 21,
        20, 23, 22
    };
    cr->skybox_base.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
        .data = SG_RANGE(skybox_verts),
        .usage = {
            .vertex_buffer = true,
        }
    });
    cr->skybox_base.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
        .data = SG_RANGE(skybox_indices),
        .usage = {
            .index_buffer = true,
        }
    });

    // Composite pipeline 
    cr->composite_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(composite_shader_desc(sg_query_backend())),
        .sample_count = 1,
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
        .layout.attrs[ATTR_composite_a_pos].format = SG_VERTEXFORMAT_FLOAT2,
        .color_count = 1,
        .colors = {
            [0].pixel_format = COLOUR_PIXELFORMAT
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    float quad_verts[] = { 
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    cr->composite_base.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
        .data = SG_RANGE(quad_verts)
    });
    cr->composite_base.bind.samplers[SMP_u_composite_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE
    });

    // Effects pipeline 
    cr->effects_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(effects_shader_desc(sg_query_backend())),
        .sample_count = 1,
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
        .layout.attrs[ATTR_effects_a_pos].format = SG_VERTEXFORMAT_FLOAT2
    });

    cr->effects_base.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
        .data = SG_RANGE(quad_verts)
    });
    cr->effects_base.bind.samplers[SMP_u_effects_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_REPEAT,
        .wrap_v = SG_WRAP_REPEAT
    });

    cr->initialized = true;
}

void chunk_renderer_load_textures(chunk_renderer_t *cr)
{
    /* Bindings. */
    cr->offscreen_base.bind.samplers[SMP_u_atlas_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .mipmap_filter = SG_FILTER_NEAREST,
        .max_lod = MIP_LEVELS,
    });
    cr->offscreen_base.bind.samplers[SMP_u_shadow_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
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
    cr->offscreen_base.bind.views[VIEW_u_atlas] = atlas.as_view;
}

void chunk_renderer_cleanup(chunk_renderer_t *cr)
{
    sg_destroy_view(cr->shadowmap_base.pass.attachments.depth_stencil);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[2]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[3]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.depth_stencil);

    sg_destroy_view(cr->offscreen_base.bind.views[VIEW_u_shadow_map]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_galbedo]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_gnormal_composite]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_gdepth_composite]);
    sg_destroy_view(cr->composite_base.bind.views[VIEW_u_gshadow]);
    #ifndef DEBUG_SHADOWS
    sg_destroy_view(cr->effects_base.bind.views[VIEW_u_gnormal_effects]);
    sg_destroy_view(cr->effects_base.bind.views[VIEW_u_gdepth_effects]);
    sg_destroy_view(cr->effects_base.bind.views[VIEW_u_ssao_noise]);
    #endif

    sg_destroy_image(cr->targets.albedo);
    sg_destroy_image(cr->targets.normal);
    sg_destroy_image(cr->targets.depth);
    sg_destroy_image(cr->targets.shadow);
    sg_destroy_image(cr->targets.shadowmap);
    sg_destroy_image(cr->targets.colour);
    sg_destroy_image(cr->targets.zbuf);
    sg_destroy_image(cr->targets.zbuf_shadow);
    sg_destroy_image(cr->info.ssao_noise_image);

    sg_destroy_pipeline(cr->shadowmap_base.pip);
    sg_destroy_pipeline(cr->offscreen_base.pip);
    sg_destroy_pipeline(cr->composite_base.pip);
    sg_destroy_pipeline(cr->effects_base.pip);
}

static void _render_shadowmap_pass(chunk_renderer_t *cr) 
{
    INSTRUMENT_FUNC_BEGIN();
    renderer_base_t *rb = &cr->shadowmap_base;

    sg_begin_pass(&rb->pass);

    sg_apply_pipeline(rb->pip);

    for (size_t i = 0; i < cr->info.chunk_coords.num; i++)
    {
        ivec2 crd = em_add_ivec2(cr->info.chunk_coords.coords[i], cr->info.chunk_coords.offset);
        chunk_render_info_t *cri = GET_OR_NULL(cr->info.chunk_data.chunks, crd);
        if (!cri)
            continue;

        vec3 pos = { cri->pos.x, 0.0, cri->pos.y };
        vs_params_shadowmap_t vs_params = {
            .u_vp = rb->cam->vp,
            .u_chunk = pos,
        };

        // Render opaque geometry for shadowcasting
        if (cri->mesh_o)
        {
            if (cri->needs_update_o && cri->mesh_o->v_cnt > 0)
            {
                sg_update_buffer(cri->bufs_o.vertex, &(sg_range) {
                    .ptr = cri->mesh_o->v_buf,
                    .size = cri->mesh_o->v_cnt * sizeof(packed_vertex_t)
                });
                sg_update_buffer(cri->bufs_o.index, &(sg_range) {
                    .ptr = cri->mesh_o->i_buf,
                    .size = cri->mesh_o->i_cnt * sizeof(uint16_t)
                });

                cri->needs_update_o = false;
            }

            rb->bind.vertex_buffers[0] = cri->bufs_o.vertex;
            rb->bind.index_buffer = cri->bufs_o.index;
            sg_apply_bindings(&rb->bind);

            sg_apply_uniforms(UB_vs_params_shadowmap, &SG_RANGE(vs_params));

            sg_draw(0, cri->mesh_o->i_cnt, 1);
        }
    }

    sg_end_pass();
    INSTRUMENT_FUNC_END();
}

static void _render_offscreen_pass(chunk_renderer_t *cr) 
{
    INSTRUMENT_FUNC_BEGIN();
    renderer_base_t *rb = &cr->offscreen_base;

    sg_begin_pass(&rb->pass);

    sg_apply_pipeline(rb->pip);

    for (size_t i = 0; i < cr->info.chunk_coords.num; i++)
    {
        ivec2 crd = em_add_ivec2(cr->info.chunk_coords.coords[i], cr->info.chunk_coords.offset);
        chunk_render_info_t *cri = GET_OR_NULL(cr->info.chunk_data.chunks, crd);
        if (!cri)
            continue;

        vs_params_offscreen_t vs_params = {
            .u_vp = rb->cam->vp,
            .u_chunk = VEC3(cri->pos.x, 0.0, cri->pos.y),
            .u_sun_vp = cr->shadowmap_base.cam->vp,
        };

        fs_params_offscreen_t fs_params = {
            .u_sun_dir = cr->info.sun_dir
        };

        // Render opaque geometry 
        if (cri->mesh_o)
        {
            if (cri->needs_update_o && cri->mesh_o->v_cnt > 0)
            {
                sg_update_buffer(cri->bufs_o.vertex, &(sg_range) {
                    .ptr = cri->mesh_o->v_buf,
                    .size = cri->mesh_o->v_cnt * sizeof(packed_vertex_t)
                });
                sg_update_buffer(cri->bufs_o.index, &(sg_range) {
                    .ptr = cri->mesh_o->i_buf,
                    .size = cri->mesh_o->i_cnt * sizeof(uint16_t)
                });

                cri->needs_update_o = false;
            }

            rb->bind.vertex_buffers[0] = cri->bufs_o.vertex;
            rb->bind.index_buffer = cri->bufs_o.index;
            sg_apply_bindings(&rb->bind);

            sg_apply_uniforms(UB_vs_params_offscreen, &SG_RANGE(vs_params));
            sg_apply_uniforms(UB_fs_params_offscreen, &SG_RANGE(fs_params));

            sg_draw(0, cri->mesh_o->i_cnt, 1);
        }
    }

    sg_end_pass();
    INSTRUMENT_FUNC_END();
}

static void _render_composite_pass(chunk_renderer_t *cr)
{
    INSTRUMENT_FUNC_BEGIN();
    renderer_base_t *rb = &cr->composite_base;

    sg_begin_pass(&rb->pass);

    fs_params_composite_t fs_params = {
        .u_view_distance = cr->info.view_distance,
        .u_inv_vp = em_inverse_mat4(cr->offscreen_base.cam->vp),
        .u_sun_dir = cr->info.sun_dir,
        .u_eye_pos = cr->offscreen_base.cam->pos
    };

    sg_apply_pipeline(rb->pip);
    sg_apply_bindings(&rb->bind);

    sg_apply_uniforms(UB_fs_params_composite, &SG_RANGE(fs_params));

    // Render the fullscreen quad
    sg_draw(0, 4, 1);

    sg_end_pass();
    INSTRUMENT_FUNC_END();
}

static void _render_skybox_pass(chunk_renderer_t *cr)
{
    INSTRUMENT_FUNC_BEGIN();
    renderer_base_t *rb = &cr->skybox_base;

    sg_begin_pass(&rb->pass);

    vs_params_skybox_t vs_params = {
        .u_pos = em_add_vec3(cr->skybox_base.cam->pos, VEC3(0.0, 0.0, 0.0)),
        .u_vp = cr->skybox_base.cam->vp
    };

    fs_params_skybox_t fs_params = {
        .u_sun_dir = cr->info.sun_dir,
    };

    sg_apply_pipeline(rb->pip);
    sg_apply_bindings(&rb->bind);

    sg_apply_uniforms(UB_vs_params_skybox, &SG_RANGE(vs_params));
    sg_apply_uniforms(UB_fs_params_skybox, &SG_RANGE(fs_params));

    // Render the skybox cube
    sg_draw(0, 6 * 6, 1);

    sg_end_pass();
    INSTRUMENT_FUNC_END();
}

static void _render_effects_pass(chunk_renderer_t *cr) 
{
    INSTRUMENT_FUNC_BEGIN();
    renderer_base_t *rb = &cr->effects_base;

    sg_begin_pass(&(sg_pass) {
        .action = rb->pass.action,
        .swapchain = sglue_swapchain(),
    });

    ENGINE_ASSERT(cr->offscreen_base.cam->kind == PROJECTION_PERSPECTIVE,
                  "offscreen camera should be perspective");

    sg_apply_pipeline(rb->pip);
    sg_apply_bindings(&rb->bind);

    #ifndef DEBUG_SHADOWS
    fs_params_effects_t fs_params = {
        .u_inv_vp = em_inverse_mat4(cr->offscreen_base.cam->vp),
        .u_proj = cr->offscreen_base.cam->proj,
        .u_view = cr->offscreen_base.cam->view
    };
    memcpy(fs_params.u_ssao_samples, cr->info.ssao_kernel, sizeof(cr->info.ssao_kernel));
    sg_apply_uniforms(UB_fs_params_effects, &SG_RANGE(fs_params));
    #endif

    // Render the fullscreen quad
    sg_draw(0, 4, 1);

    sg_end_pass();
    INSTRUMENT_FUNC_END();
}

void chunk_renderer_render_all(chunk_renderer_t *cr)
{
    // Synchronize main camera and shadow camera.
    camera_t *oc = cr->offscreen_base.cam;
    camera_t *sc = cr->shadowmap_base.cam;

    ivec3 tmp = em_floor_vec3(em_div_vec3_f(oc->pos, CHUNK_SIZE));
    vec3 target = em_mul_vec3_f(AS_VEC3(tmp), CHUNK_SIZE);
    target.y = sc->pos.y;

    sc->view = em_look_at(em_sub_vec3(target, cr->info.sun_dir), target, WORLD_Y);
    sc->pos = target;

    cam_update(sc);

    _render_shadowmap_pass(cr);
    _render_offscreen_pass(cr);
    _render_composite_pass(cr);
    _render_skybox_pass(cr);
    _render_effects_pass(cr);
}
