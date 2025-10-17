#include "render_system.h"
#include "update_system.h"
#include <GLES3/gl31.h>

static void _check_buffer_state(sg_buffer buf, const char *bufname)
{
    switch (sg_query_buffer_state(buf)) {
    case SG_RESOURCESTATE_INITIAL:
        ENGINE_LOG_WARN("%s State INITIAL.\n", bufname);
        break;
    case SG_RESOURCESTATE_ALLOC:
        ENGINE_LOG_WARN("%s State ALLOC.\n", bufname);
        break;
    case SG_RESOURCESTATE_VALID:
        ENGINE_LOG_WARN("%s State VALID.\n", bufname);
        break;
    case SG_RESOURCESTATE_FAILED:
        ENGINE_LOG_WARN("%s State FAILED.\n", bufname);
        break;
    case SG_RESOURCESTATE_INVALID:
        ENGINE_LOG_WARN("%s State INVALID.\n", bufname);
        break;
    default:
        break;
    };

}

void render_sys_init(render_system_t *rs, const render_system_desc_t *desc)
{
    /* Pipeline. */
    rs->pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = sg_make_shader(chunk_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                [ATTR_chunk_a_xyzn] = { // Position within chunk and packed normals.
                    .format = SG_VERTEXFORMAT_UBYTE4,
                },
                [ATTR_chunk_a_uv] = { // Packed texture coords.
                    .format = SG_VERTEXFORMAT_UINT,
                }
            },
        },
        .index_type = SG_INDEXTYPE_UINT32,
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
        .views[0] = sg_alloc_view(),
        .vertex_buffers[0] = desc->vbo,
        .index_buffer = desc->ibo
    };

    _check_buffer_state(rs->bind.vertex_buffers[0], "vbo");
    _check_buffer_state(rs->bind.index_buffer, "ibo");

    em_bmp_image_t *atlases[5] = {
        em_bmp_load("res/tex/minecraft_remake_texture_atlas.bmp"),
        em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm1.bmp"),
        em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm2.bmp"),
        em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm3.bmp"),
        em_bmp_load("res/tex/minecraft_remake_texture_atlas-mm4.bmp")
    };

    for (size_t i = 0; i < 5; i++)
    {
        if (!atlases[i]) 
        {
            for (size_t i = 0; i < 5; i++)
            {
                free(atlases[i]->pixel_data);
                free(atlases[i]);
            }

            ENGINE_LOG_ERROR("Failed to load texture atlas. Aborting.\n", NULL);
            exit(1);
        }
    }

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
        .far       = desc->view_distance * CHUNK_SIZE,
        .aspect    = desc->window_size.x / desc->window_size.y,
        .fov       = 60.0,
        .pos       = {0.0, 34.0, 0.0},
    });
}

void render_sys_cleanup(render_system_t *rs)
{
    (void) rs;
}

void render_sys_render(render_system_t *rs, render_data_t r_data, render_coords_t r_crds)
{
    sg_begin_pass(&(sg_pass) {
        .action = rs->pass_act,
        .swapchain = sglue_swapchain(),
        .label = "Render system pass"
    });

    sg_apply_pipeline(rs->pip);

    // ENGINE_LOG_WARN("Rendering VBO handle %u\n", rs->bind.vertex_buffers[0].id);
    // ENGINE_LOG_WARN("Rendering IBO handle %u\n", rs->bind.index_buffer.id);

    for (size_t i = 0; i < r_crds.num; i++)
    {
        ivec2 crd = em_add_ivec2(r_crds.coords[i], r_crds.offset);
        chunk_render_info_t *cri = r_data.chunks->get_or_default(r_data.chunks, crd, NULL);

        // if (!cri || cri->generation > r_data.generation)
        //     continue; // This chunk has not been loaded yet.

        // if (!cri || r_data.do_not_render->contains_key(r_data.do_not_render, cri->pos))
        //     continue;

        if (!cri)
            continue;

        // ENGINE_LOG_OK("Rendering cri: POS: %i %i, OFFSET: %zu %zu, ICNT: %hu.\n", 
        //               cri->pos.x, cri->pos.y, cri->offset.v_ofst, cri->offset.i_ofst,
        //               cri->index_cnt);

        vs_params_t vs_params = {
            .u_mvp = rs->cam.vp,
            .u_v = rs->cam.view,
            .u_chnk_pos = {
                (float) cri->pos.x,
                0.0,
                (float) cri->pos.y
            }
        };

        fs_params_t fs_params = {
            .u_fog_data = {0.35, 0.6, 0.85, rs->cam.far}
        };

        rs->bind.vertex_buffer_offsets[0] = cri->offset.v_ofst * sizeof(vertex_t);
        rs->bind.index_buffer_offset = cri->offset.i_ofst * sizeof(uint32_t);

        // _check_buffer_state(rs->bind.vertex_buffers[0], "vbo");
        // _check_buffer_state(rs->bind.index_buffer, "ibo");

        // ENGINE_LOG_OK("Applying Bindings.\n", NULL);
        sg_apply_bindings(&rs->bind);
        // ENGINE_LOG_OK("Applied Bindings.\n", NULL);

        sg_apply_uniforms(UB_vs_params, &SG_RANGE(vs_params));
        sg_apply_uniforms(UB_fs_params, &SG_RANGE(fs_params));

        sg_draw(0, cri->index_cnt, 1);
    }

    sg_end_pass();
    sg_commit();
}
