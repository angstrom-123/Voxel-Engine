#include "state.h"

void state_init_pipeline(state_t *state)
{
	state->pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = sg_make_shader(chunk_shader_desc(sg_query_backend())),
		.layout = {
			.attrs = {
				[ATTR_chunk_a_xyzn] = {
					.format = SG_VERTEXFORMAT_UBYTE4,
					.buffer_index = 0
				},
				[ATTR_chunk_a_uv] = {
					.format = SG_VERTEXFORMAT_UINT,
					.buffer_index = 0
				}
			}
		},
		.index_type = SG_INDEXTYPE_UINT16,
		.cull_mode = SG_CULLMODE_BACK,
		.depth = {
			.compare = SG_COMPAREFUNC_LESS_EQUAL,
			.write_enabled = true
		},
		.blend_color = {1.0, 0.0, 0.0, 1.0},
		.colors[0].blend = {
			.enabled = true,
			.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
			.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.op_rgb = SG_BLENDOP_ADD,
			.src_factor_alpha = SG_BLENDFACTOR_ONE,
			.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.op_alpha = SG_BLENDOP_ADD
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
			.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
			.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
			.max_lod = 5.0, /* NOTE: Adjust this to be number of mipmaps */
		}),
		.images[0] = sg_alloc_image()
	};
}

void state_init_textures(state_t *state)
{
	/* Full-res atlas and mipmap levels */
	bmp_image_t *atlas = bmp_load_file("res/minecraft_remake_texture_atlas.bmp");
	bmp_image_t *atlas_mm1 = bmp_load_file("res/minecraft_remake_texture_atlas-mm1.bmp");
	bmp_image_t *atlas_mm2 = bmp_load_file("res/minecraft_remake_texture_atlas-mm2.bmp");
	bmp_image_t *atlas_mm3 = bmp_load_file("res/minecraft_remake_texture_atlas-mm3.bmp");
	bmp_image_t *atlas_mm4 = bmp_load_file("res/minecraft_remake_texture_atlas-mm4.bmp");

	if (atlas && atlas_mm1 && atlas_mm2 && atlas_mm3 && atlas_mm4)
	{
		sg_init_image(state->bind.images[0], &(sg_image_desc) {
			.width = atlas->info_header.width,
			.height = atlas->info_header.height,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.num_mipmaps = 5,
			.data.subimage[0][0] = {
				.ptr = atlas->pixel_data,
				.size = (size_t) atlas->info_header.img_size
			},
			.data.subimage[0][1] = {
				.ptr = atlas_mm1->pixel_data,
				.size = (size_t) atlas_mm1->info_header.img_size
			},
			.data.subimage[0][2] = {
				.ptr = atlas_mm2->pixel_data,
				.size = (size_t) atlas_mm2->info_header.img_size
			},
			.data.subimage[0][3] = {
				.ptr = atlas_mm3->pixel_data,
				.size = (size_t) atlas_mm3->info_header.img_size
			},
			.data.subimage[0][4] = {
				.ptr = atlas_mm4->pixel_data,
				.size = (size_t) atlas_mm4->info_header.img_size
			},
		});

		free(atlas->pixel_data);
		free(atlas_mm1->pixel_data);
		free(atlas_mm2->pixel_data);
		free(atlas_mm3->pixel_data);
		free(atlas_mm4->pixel_data);
		free(atlas);
		free(atlas_mm1);
		free(atlas_mm2);
		free(atlas_mm3);
		free(atlas_mm4);
	} 
	else 
	{
		free(atlas);
		free(atlas_mm1);
		free(atlas_mm2);
		free(atlas_mm3);
		free(atlas_mm4);
		fprintf(stderr, "Failed to load texture atlas or mipmaps.");
		exit(1);
	}
}

void state_init_cam(state_t *state)
{
	state->cam = cam_setup(&(camera_desc_t) {
		.render_distance = 4,
		.near_dist = 0.1,
		.far_dist = 100.0,
		.aspect = (sapp_widthf() / sapp_heightf()),
		.fov = 60.0,
		.turn_sens = 0.04,
		.move_sens = 10.0,
		.rotation = {0.0, 0.0, 0.0, 1.0}, /* Identity quaternion */
		.position = {8.0, 34.0, 8.0},
	});
}
