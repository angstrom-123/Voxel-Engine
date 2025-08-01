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

    #define STATE_PIPELINE_INIT
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
        .images[0] = sg_alloc_image()
    };

    #define STATE_BINDINGS_INIT
}

void state_init_textures(state_t *state)
{
    /* Full-res atlas and mipmap levels */
    em_bmp_image_t *atlases[5] = {
        em_bmp_load("res/minecraft_remake_texture_atlas.bmp"),
        em_bmp_load("res/minecraft_remake_texture_atlas-mm1.bmp"),
        em_bmp_load("res/minecraft_remake_texture_atlas-mm2.bmp"),
        em_bmp_load("res/minecraft_remake_texture_atlas-mm3.bmp"),
        em_bmp_load("res/minecraft_remake_texture_atlas-mm4.bmp")
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

    sg_init_image(state->bind.images[0], &(sg_image_desc) {
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

    for (size_t i = 0; i < 5; i++)
    {
        free(atlases[i]->pixel_data);
        free(atlases[i]);
    }

    #define STATE_TEXTURES_INIT
}

void state_init_cam(state_t *state)
{
    state->cam = cam_setup(&(camera_desc_t) {
        .rndr_dist = 12, // NOTE: MAX = 16
        .near      = 0.1,
        .far       = 300.0,
        .aspect    = (sapp_widthf() / sapp_heightf()),
        .fov       = 60.0,
        .turn_sens = 0.04,
        .move_sens = 10.0,
        .rot       = {0.0, 0.0, 0.0, 1.0},
        .pos       = {0.0, 34.0, 0.0},
    });

    #define STATE_CAM_INIT
}

void state_init_chunk_buffer(state_t *state)
{
    const size_t MEGA_BUFFER_SIZE = 134217728; // 128 MebiBytes

    state->cb = (chunk_buffer_t) {
        .vbo = sg_make_buffer(&(sg_buffer_desc) {
            .size = MEGA_BUFFER_SIZE,
            .usage = {
                .vertex_buffer = true,
                .dynamic_update = true
            }
        }),
        .ibo = sg_make_buffer(&(sg_buffer_desc) {
            .size = MEGA_BUFFER_SIZE,
            .usage = {
                .index_buffer = true,
                .dynamic_update = true
            }
        }),
        .v_stg = malloc(MEGA_BUFFER_SIZE),
        .i_stg = malloc(MEGA_BUFFER_SIZE),
        .v_cnt = 0,
        .i_cnt = 0
    };

    if (!state->cb.v_stg || !state->cb.i_stg 
        || (state->cb.vbo.id == SG_INVALID_ID) 
        || (state->cb.ibo.id == SG_INVALID_ID))
    {
        fprintf(stderr, "Failed to allocate mega buffers.\n");
        exit(1);
    }
}

void state_init_data(state_t *state)
{
    #ifndef STATE_CAM_INIT
        fprintf(stderr, "Camera must be initialized before state data\n");
        exit(1);
    #endif

    state->prev_chunk_pos = (ivec3) {0, 0, 0};
    state->tick = 0;
    state->chunk_cnt = 0;
    state->chunks = NULL;

    size_t max_chunks = (2 * em_sqr(state->cam.rndr_dist)) 
                      + (2 * state->cam.rndr_dist) + 1;
    state->chunk_map = HASHMAP_NEW(ivec2_chunk, max_chunks * 1.5, 
                                   HASHMAP_CMP(ivec2),
                                   HASHMAP_HSH(ivec2));
}
