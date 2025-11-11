#include "chunk_renderer.h"

void chunk_renderer_init(chunk_renderer_t *cr, const chunk_renderer_desc_t *desc)
{
    renderer_init_base(&cr->base, &(renderer_base_desc_t) {
        .dummy = false,
        .cam = desc->cam,
        .dimensions = desc->dimensions,
        .pass_act = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = {0.35, 0.6, 0.85, 1.0}
            }
        },
        .pip_desc = &(sg_pipeline_desc) {
            .shader = sg_make_shader(chunk_shader_desc(sg_query_backend())),
            .layout = {
                .attrs = {
                    [ATTR_chunk_a_vertex] = {
                        .format = SG_VERTEXFORMAT_UBYTE4 // xz = 1, y = 2, tex = 3, 4 = normal
                    },
                }
            },
            .index_type = SG_INDEXTYPE_UINT16,
            .cull_mode = SG_CULLMODE_BACK,
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
            .label = "chunk-pipeline"
        }
    });
}

void chunk_renderer_load_textures(chunk_renderer_t *cr)
{
    /* Bindings. */
    cr->base.bind = (sg_bindings) {
        .samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
            .min_filter = SG_FILTER_NEAREST,
            .mag_filter = SG_FILTER_NEAREST,
            .mipmap_filter = SG_FILTER_NEAREST,
            .max_lod = (float) MIP_LEVELS,
        }),
        .views[0] = sg_alloc_view()
    };

    em_bmp_image_t atlases[MIP_LEVELS];
    const size_t MAX_PATH_LEN = 128;
    for (size_t i = 0; i < MIP_LEVELS; i++)
    {
        char path[MAX_PATH_LEN];
        snprintf(path, MAX_PATH_LEN, "res/tex/minecraft_remake_texture_atlas-mm%zu.bmp", i);
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
            {.ptr = atlases[0].pixel_data, .size = atlases[0].ih.img_size},
            {.ptr = atlases[1].pixel_data, .size = atlases[1].ih.img_size},
            {.ptr = atlases[2].pixel_data, .size = atlases[2].ih.img_size},
            {.ptr = atlases[3].pixel_data, .size = atlases[3].ih.img_size},
            {.ptr = atlases[4].pixel_data, .size = atlases[4].ih.img_size},
        },
    });

    sg_init_view(cr->base.bind.views[0], &(sg_view_desc) {
        .texture = {.image = img}
    });

    for (size_t i = 0; i < 5; i++)
        free(atlases[i].pixel_data);
}

void chunk_renderer_cleanup(chunk_renderer_t *cr)
{
    (void) cr;
}

void chunk_renderer_render_all(chunk_renderer_t *cr)
{
    sg_begin_pass(&(sg_pass) {
        .action = cr->base.pass_act,
        .swapchain = sglue_swapchain(),
        .label = "Chunk renderer pass"
    });

    sg_apply_pipeline(cr->base.pip);

    for (size_t i = 0; i < cr->coords.num; i++)
    {
        ivec2 crd = em_add_ivec2(cr->coords.coords[i], cr->coords.offset);
        chunk_render_info_t *cri = cr->data.chunks->get_or_default(cr->data.chunks, crd, NULL);

        if (!cri)
            continue;

        vs_params_chunk_t vs_params = {
            .u_mvp = cr->base.cam->vp,
            .u_view = cr->base.cam->view,
            .u_ccord = {cri->pos.x, 0, cri->pos.y}
        };

        fs_params_chunk_t fs_params = {
            .u_fog_data = {0.35, 0.6, 0.85, cr->base.cam->far}
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

        cr->base.bind.vertex_buffers[0] = cri->bufs.vertex;
        cr->base.bind.index_buffer = cri->bufs.index;
        sg_apply_bindings(&cr->base.bind);

        sg_apply_uniforms(UB_vs_params_chunk, &SG_RANGE(vs_params));
        sg_apply_uniforms(UB_fs_params_chunk, &SG_RANGE(fs_params));

        sg_draw(0, cri->mesh->i_cnt, 1);
    }

    sg_end_pass();
}
