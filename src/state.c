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
			// .mipmap_filter = SG_FILTER_NEAREST,
			// .max_lod = 4.0
		}),
		.images[0] = sg_alloc_image()
	};
}

void state_init_textures(state_t *state, char* tex_path)
{
	bmp_image_t *atlas = bmp_load_file(tex_path);
	if (atlas)
	{
		sg_init_image(state->bind.images[0], &(sg_image_desc) {
			.width = atlas->info_header.width,
			.height = atlas->info_header.height,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.data.subimage[0][0] = {
				.ptr = atlas->pixel_data,
				.size = (size_t) atlas->info_header.img_size
			}
		});

		free(atlas->pixel_data);
		free(atlas);
	} 
	else 
	{
		free(atlas);
		fprintf(stderr, "Failed to load texture atlas at: %s\n", tex_path);
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
		.position = {0.0, 1.5, 6.0},
	});
}
