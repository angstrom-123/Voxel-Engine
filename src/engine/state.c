#include "state.h"

void state_init_pipeline(state_t *state)
{
    state->pip = sg_make_pipeline(&(sg_pipeline_desc) {
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
}

void state_init_bindings(state_t *state)
{
    state->bind = (sg_bindings) {
        .samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
            .min_filter = SG_FILTER_NEAREST,
            .mag_filter = SG_FILTER_NEAREST,
            .mipmap_filter = SG_FILTER_NEAREST,
            .max_lod = 5.0,
        }),
        .views[0] = sg_alloc_view()
    };
}

void state_init_textures(state_t *state)
{
    /* Full-res atlas and mipmap levels */
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

            fprintf(stderr, "Failed to load texture atlas or mipmaps.\n");
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

    sg_init_view(state->bind.views[0], &(sg_view_desc) {
        .texture = {.image = img}
    });

    for (size_t i = 0; i < 5; i++)
    {
        free(atlases[i]->pixel_data);
        free(atlases[i]);
    }
}

void state_init_cam(state_t *state)
{
    state->cam = cam_setup(&(camera_desc_t) {
        .near      = 0.1,
        .far       = (state->render_dist + 2) * CHUNK_SIZE,
        .aspect    = (sapp_widthf() / sapp_heightf()),
        .fov       = 60.0,
        .turn_sens = 0.04,
        .move_sens = 10.0,
        .rot       = {0.0, 0.0, 0.0, 1.0},
        .pos       = {0.0, 34.0, 0.0},
    });
}

void state_init_systems(state_t *state)
{
    cs_init(&state->cs, &(chunk_system_desc_t) {
        .free_capacity = NUM_SLOTS,
        .chunk_capacity = NUM_SLOTS * 1.4,
        .request_capacity = 128,
        .accumulator_capacity = 128,
        .seed = state->game_desc.seed
    });
}
