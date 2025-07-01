#include "state.h"

void state_init_pipeline(state_t *state)
{
	state->pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = sg_make_shader(cube_shader_desc(sg_query_backend())),
		.layout = {
			// .buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE, /* IMPORTANT */
			.attrs = {
				[ATTR_cube_base_pos] = {
					.format = SG_VERTEXFORMAT_FLOAT3,
					.buffer_index = 0
				},
				// [ATTR_cube_inst_pos] = {
				// 	.format = SG_VERTEXFORMAT_FLOAT3,
				// 	.buffer_index = 1 /* NOTE: buffer_index = 1, not 0 */
				// },
				[ATTR_cube_tex_uv] = {
					.format = SG_VERTEXFORMAT_SHORT2N, 
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
		.label = "pipeline"
	});
}

void state_init_bindings(state_t *state)
{
	cube_t *cube = mesh_generate_cube();
	if (!cube)
	{
		free(cube);
		fprintf(stderr, "Failed to generate base cube mesh\n");
		exit(1);
	}

	atlas_set_texture(cube, TEX_GRASS_SIDE);
	atlas_set_top(cube, TEX_GRASS_TOP);
	atlas_set_bottom(cube, TEX_DIRT);

	state->bind = (sg_bindings) {
		.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
			.data = SG_RANGE(cube->vertices),
			.label = "vertices"
		}),
		.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
			.usage.index_buffer = true,
			.data = SG_RANGE(cube->indices),
			.label = "indices"
		}),
		.samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
			.min_filter = SG_FILTER_NEAREST,
			.mag_filter = SG_FILTER_NEAREST
		}),
		.images[0] = sg_alloc_image()
	};

	free(cube);
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
	} 
	else fprintf(stderr, "Failed to load texture atlas at: %s\n", tex_path);

	free(atlas);
}

void state_init_cam(state_t *state)
{
	state->cam = cam_setup(&(camera_desc_t) {
		.near_dist = 0.1,
		.far_dist = 100.0,
		.aspect = (sapp_widthf() / sapp_heightf()),
		.fov = 60.0,
		.turn_sens = 0.04,
		.move_sens = 0.2,
		.rotation = {.x = 0.0, .y = 0.0, .z = 0.0, .w = 1.0},
		.position = {.x = 0.0, .y = 1.5, .z = 6.0},
	});
}
