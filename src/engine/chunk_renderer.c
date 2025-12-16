#include "chunk_renderer.h"

void chunk_renderer_resize(chunk_renderer_t *cr, const vec2 dim) 
{
    cr->offscreen_base.dimensions = dim;
    cr->display_base.dimensions = dim;

    // Cleanup existing images and texture views.
    sg_destroy_image(cr->targets.colour);
    sg_destroy_image(cr->targets.normal);
    sg_destroy_image(cr->targets.depth);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.depth_stencil);

    // Make new targets with the new dimensions.
    cr->targets = (struct targets) {
        .colour = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "offscreen-chunk-colour-image"
        }),
        .normal = sg_make_image(&(sg_image_desc) {
            .usage.color_attachment = true,
            .pixel_format = SG_PIXELFORMAT_RGBA16F,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "offscreen-chunk-normal-image"
        }),
        .depth = sg_make_image(&(sg_image_desc) {
            .usage.depth_stencil_attachment = true,
            .pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL,
            .width = dim.x,
            .height = dim.y,
            .sample_count = 1,
            .label = "offscreen-chunk-depth-image"
        })
    };

    // Make new target views from the new images
    cr->offscreen_base.pass.attachments.colors[0] = sg_make_view(&(sg_view_desc) {
        .color_attachment.image = cr->targets.colour,
        .label = "offscreen-chunk-colour-attachment"
    });
    cr->offscreen_base.pass.attachments.colors[1] = sg_make_view(&(sg_view_desc) {
        .color_attachment.image = cr->targets.normal,
        .label = "offscreen-chunk-normal-attachment"
    });
    cr->offscreen_base.pass.attachments.depth_stencil = sg_make_view(&(sg_view_desc) {
        .depth_stencil_attachment = cr->targets.depth,
        .label = "offscreen-chunk-depth-attachment"
    });

    cr->display_base.bind.views[0] = sg_make_view(&(sg_view_desc) {
        .texture.image = cr->targets.colour,
        // .color_attachment.image = cr->targets.colour,
        .label = "display-chunk-colour-view"
    });
    cr->display_base.bind.views[1] = sg_make_view(&(sg_view_desc) {
        .texture.image = cr->targets.normal,
        // .color_attachment.image = cr->targets.normal,
        .label = "display-chunk-normal-view"
    });
    cr->display_base.bind.views[2] = sg_make_view(&(sg_view_desc) {
        .texture.image = cr->targets.depth,
        // .depth_stencil_attachment.image = cr->targets.depth,
        .label = "display-chunk-depth-view"
    });
}

void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc)
{
    rbase_init(&cr->offscreen_base, desc->base_desc);
    rbase_init(&cr->display_base, desc->base_desc);

    // Inits the render targets and attachment views for the offscreen pass.
    chunk_renderer_resize(cr, desc->base_desc->dimensions);

    // Offscreen pass
    cr->offscreen_base.pass.action = (sg_pass_action) {
        .colors[0].load_action = SG_LOADACTION_CLEAR,
        .colors[1].load_action = SG_LOADACTION_CLEAR,
        .depth.load_action = SG_LOADACTION_CLEAR,
    };
    cr->offscreen_base.pass.label = "offscreen-chunks-pass";

    // Offscreen pipeline
    cr->offscreen_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(chunk_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                // 1: xz, 2: y, 3: tex id, 4: normal
                [ATTR_chunk_a_vertex].format = SG_VERTEXFORMAT_UBYTE4,
            }
        },
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .sample_count = 1,
        .color_count = 2,
        .colors = {
            [0].pixel_format = SG_PIXELFORMAT_RGBA8,
            [1].pixel_format = SG_PIXELFORMAT_RGBA16F
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
        .label = "offscreen-chunks-pipeline"
        // .colors[0] = {
        //     .blend = {
        //         .enabled = true,
        //         .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
        //         .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        //         .op_rgb = SG_BLENDOP_ADD,
        //         .src_factor_alpha = SG_BLENDFACTOR_ONE,
        //         .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        //         .op_alpha = SG_BLENDOP_ADD
        //     }
        // }
    });

    // Display pipeline 
    cr->display_base.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(composite_shader_desc(sg_query_backend())),
        .layout.attrs[ATTR_composite_a_pos].format = SG_VERTEXFORMAT_FLOAT2,
        .cull_mode = SG_CULLMODE_BACK,
        // .blend_color = {1.0, 0.0, 0.0, 1.0},
        .colors[0].pixel_format = SG_PIXELFORMAT_RGBA8,
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
        .label = "display-chunks-pipeline",
    });

    // For sampling the render textures from the offscreen pass
    float quad_verts[] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
    cr->display_base.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
        .data = SG_RANGE(quad_verts),
        .label = "display-quad-verts"
    });
    cr->display_base.bind.samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "chunk-display-sampler"
    });
}

void chunk_renderer_load_textures(chunk_renderer_t *cr)
{
    /* Bindings. */
    cr->offscreen_base.bind.samplers[SMP_u_smp] = sg_make_sampler(&(sg_sampler_desc) {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .mipmap_filter = SG_FILTER_NEAREST,
        .max_lod = (float) MIP_LEVELS,
    });
    cr->offscreen_base.bind.views[0] = sg_alloc_view();

    em_bmp_image_t atlases[MIP_LEVELS];
    const size_t MAX_PATH_LEN = 128;
    for (size_t i = 0; i < MIP_LEVELS; i++)
    {
        char path[MAX_PATH_LEN];
        snprintf(path, MAX_PATH_LEN, "res/tex/block/atlas-mipmap-%zu.bmp", i);
        bool res = em_bmp_load(&atlases[i], path);
        ENGINE_ASSERT(res, "Failed to load texture atlas mip level");
    }

    sg_image img = sg_alloc_image();
    sg_init_image(img, &(sg_image_desc) {
        .width = atlases[0].ih.width,
        .height = atlases[0].ih.height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .num_mipmaps = MIP_LEVELS,
        .data.subimage[0] = {
            { .ptr = atlases[0].pixel_data, .size = atlases[0].ih.img_size },
            { .ptr = atlases[1].pixel_data, .size = atlases[1].ih.img_size },
            { .ptr = atlases[2].pixel_data, .size = atlases[2].ih.img_size },
            { .ptr = atlases[3].pixel_data, .size = atlases[3].ih.img_size },
            { .ptr = atlases[4].pixel_data, .size = atlases[4].ih.img_size }
        }
    });

    sg_init_view(cr->offscreen_base.bind.views[0], &(sg_view_desc) { .texture.image = img });

    for (size_t i = 0; i < 5; i++)
        free(atlases[i].pixel_data);
}

void chunk_renderer_cleanup(chunk_renderer_t *cr)
{
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->offscreen_base.pass.attachments.depth_stencil);
    sg_destroy_pipeline(cr->offscreen_base.pip);

    sg_destroy_view(cr->display_base.pass.attachments.colors[0]);
    sg_destroy_view(cr->display_base.pass.attachments.colors[1]);
    sg_destroy_view(cr->display_base.pass.attachments.depth_stencil);
    sg_destroy_pipeline(cr->display_base.pip);

    sg_destroy_image(cr->targets.colour);
    sg_destroy_image(cr->targets.normal);
    sg_destroy_image(cr->targets.depth);
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

        vs_params_chunk_t vs_params = {
            .u_mvp = rb->cam->vp,
            .u_view = rb->cam->view,
            .u_ccord = { cri->pos.x, 0, cri->pos.y }
        };

        fs_params_chunk_t fs_params = {
            .u_fog_data = { 0.35, 0.6, 0.85, rb->cam->far }
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
        .label = "chunk-display-pass"
    });

    sg_apply_pipeline(rb->pip);
    sg_apply_bindings(&rb->bind);

    // Render the fullscreen quad
    sg_draw(0, 4, 1);

    sg_end_pass();
}

void chunk_renderer_render_all(chunk_renderer_t *cr)
{
    _render_offscreen_pass(cr);
    _render_display_pass(cr);
}
