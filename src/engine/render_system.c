#include "render_system.h"

void _on_resize(const event_t *ev, void *args)
{
    render_system_t *rs = args;
    rs->dimensions = ev->window_size;
}

void render_sys_init(render_system_t *rs, const render_system_desc_t *desc)
{
    /* Pipeline. */
    rs->pip = sg_make_pipeline(&(sg_pipeline_desc) {
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
    });

    /* Bindings. */
    rs->bind = (sg_bindings) {
        .samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
            .min_filter = SG_FILTER_NEAREST,
            .mag_filter = SG_FILTER_NEAREST,
            .mipmap_filter = SG_FILTER_NEAREST,
            .max_lod = 5.0,
        }),
        .views[0] = sg_alloc_view()
    };

    em_bmp_image_t *atlases[5];
    atlases[0] = em_bmp_load("res/tex/minecraft_remake_texture_atlas.bmp");
    atlases[1] = em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm1.bmp");
    atlases[2] = em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm2.bmp");
    atlases[3] = em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm3.bmp");
    atlases[4] = em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm4.bmp");

    ENGINE_ASSERT(atlases[0] != NULL, "Failed to load texture atlas 0.\n");
    ENGINE_ASSERT(atlases[1] != NULL, "Failed to load texture atlas 1.\n");
    ENGINE_ASSERT(atlases[2] != NULL, "Failed to load texture atlas 2.\n");
    ENGINE_ASSERT(atlases[3] != NULL, "Failed to load texture atlas 3.\n");
    ENGINE_ASSERT(atlases[4] != NULL, "Failed to load texture atlas 4.\n");

    sg_image img = sg_alloc_image();
    sg_init_image(img, &(sg_image_desc) {
        .width = atlases[0]->ih.width,
        .height = atlases[0]->ih.height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .num_mipmaps = 5,
        .data.subimage[0] = {
            {.ptr = atlases[0]->pixel_data, .size = atlases[0]->ih.img_size},
            {.ptr = atlases[1]->pixel_data, .size = atlases[1]->ih.img_size},
            {.ptr = atlases[2]->pixel_data, .size = atlases[2]->ih.img_size},
            {.ptr = atlases[3]->pixel_data, .size = atlases[3]->ih.img_size},
            {.ptr = atlases[4]->pixel_data, .size = atlases[4]->ih.img_size},
        },
    });

    sg_init_view(rs->bind.views[0], &(sg_view_desc) {
        .texture = {.image = img}
    });

    for (size_t i = 0; i < 5; i++)
    {
        free(atlases[i]->pixel_data);
        free(atlases[i]);
    }

    /* Pass Action. */
    rs->pass_act = (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {0.35, 0.6, 0.85, 1.0}
        }
    };

    /* Camera. */
    cam_init(&rs->cam, &(camera_desc_t) {
        .near      = 0.1,
        .far       = desc->view_distance,
        .aspect    = desc->window_size.x / desc->window_size.y,
        .fov       = 60.0,
        .pos       = {0.0, 0.0, 0.0},
    });

    /* Data. */
    rs->dimensions = desc->window_size;
}

void render_sys_cleanup(render_system_t *rs)
{
    (void) rs;
}

void render_sys_render(render_system_t *rs, render_data_t r_data, render_coords_t r_crds)
{
    sg_apply_pipeline(rs->pip);

    INSTRUMENT_SCOPE_BEGIN(render_sys_iterate);
    for (size_t i = 0; i < r_crds.num; i++)
    {
        ivec2 crd = em_add_ivec2(r_crds.coords[i], r_crds.offset);
        chunk_render_info_t *cri = r_data.chunks->get_or_default(r_data.chunks, crd, NULL);

        if (!cri)
            continue;

        vs_params_t vs_params = {
            .u_mvp = rs->cam.vp,
            .u_view = rs->cam.view,
            .u_ccord = {cri->pos.x, 0, cri->pos.y}
        };

        fs_params_t fs_params = {
            .u_fog_data = {0.35, 0.6, 0.85, rs->cam.far}
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

        INSTRUMENT_SCOPE_BEGIN(chunk_bind);

        rs->bind.vertex_buffers[0] = cri->bufs.vertex;
        rs->bind.index_buffer = cri->bufs.index;
        sg_apply_bindings(&rs->bind);

        INSTRUMENT_SCOPE_END(chunk_bind);

        INSTRUMENT_SCOPE_BEGIN(chunk_uniform_apply);

        sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
        sg_apply_uniforms(UB_fs_params, &SG_RANGE(fs_params));

        INSTRUMENT_SCOPE_END(chunk_uniform_apply);

        INSTRUMENT_SCOPE_BEGIN(chunk_draw_call);

        sg_draw(0, cri->mesh->i_cnt, 1);

        INSTRUMENT_SCOPE_END(chunk_draw_call);
    }
    INSTRUMENT_SCOPE_END(render_sys_iterate);
}

void render_sys_render_ui(render_system_t *rs, ui_component_t **comps, size_t cnt)
{
    struct nk_context *ctx = snk_new_frame();

    for (size_t i = 0; i < cnt; i++)
    {
        ui_component_t *uic = comps[i];
        if (uic->visible(uic))
            uic->render(ctx, rs->dimensions, uic);
    }

    snk_render(rs->dimensions.x, rs->dimensions.y);
}

void render_sys_scene_start(render_system_t *rs)
{
    sg_begin_pass(&(sg_pass) {
        .action = rs->pass_act,
        .swapchain = sglue_swapchain(),
        .label = "Render system pass"
    });
}

void render_sys_scene_end(render_system_t *rs)
{
    (void) rs;
    sg_end_pass();
    sg_commit();
}
