#include "state.h"

static bucket_t _new_bucket(size_t size, size_t upper)
{
    return (bucket_t) {
        .chunks = HASHMAP_NEW(ivec2_chunk, size, HASHMAP_CMP(ivec2), 
                              HASHMAP_HSH(ivec2), EM_NO_RESIZE),
        .upper = upper
    };
}

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
    const uint8_t rndr_dist = 12;
    // const uint8_t rndr_dist = 4;
    state->cam = cam_setup(&(camera_desc_t) {
        .rndr_dist = rndr_dist,
        .near      = 0.1,
        .far       = (rndr_dist - 4) * CHUNK_SIZE,
        // .far       = (rndr_dist + 5) * CHUNK_SIZE,
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
    state->cb = (chunk_buffer_t) {
        .vbo = sg_make_buffer(&(sg_buffer_desc) {
            .size = V_STG_SIZE,
            .usage = {
                .vertex_buffer = true,
                .dynamic_update = true
            }
        }),
        .ibo = sg_make_buffer(&(sg_buffer_desc) {
            .size = I_STG_SIZE,
            .usage = {
                .index_buffer = true,
                .dynamic_update = true
            }
        }),
        .v_stg = malloc(V_STG_SIZE),
        .i_stg = malloc(I_STG_SIZE),
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

    state->frame = 0;
    state->tick = 0;
    state->l_tick = 0;

    size_t chnk_max = (2 * em_sqr(state->cam.rndr_dist)) 
                    + (2 * state->cam.rndr_dist) + 1;
    size_t req_size = chnk_max * 1.35; // < 0.75 hmap load when all chunks loaded.

    state->buckets[BUCKET_HOT] = _new_bucket(req_size, 0);
    state->buckets[BUCKET_COLD] = _new_bucket(req_size, 240);

    state->free_list = DLL_NEW(offset);
    for (size_t i = 0; i < NUM_SLOTS; i++)
    {
        offset_t offset = {.v_ofst = i * V_MAX, .i_ofst = i * I_MAX};
        state->free_list->append(state->free_list, offset);
    }
}
