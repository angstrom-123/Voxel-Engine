#include "object.h"

static float* _unpack(uint32_t packed)
{
	const float a = ((float) (packed >> 16)) / 65535.0;
	const float b = ((float) (packed & 0xFFFF)) / 65535.0;

	return (float[]) {a, b};
}

static cube_type_e _get_neighbour(uint8_t x, uint8_t y, uint8_t z,
						   cube_face_idx_e face, chunk_data_t *data)
{
	switch (face) {
	case FACEIDX_BACK:
		if (z == 0) return CUBETYPE_AIR;
		return data->block_types[x][y][z - 1];

	case FACEIDX_FRONT:
		if (z == CHUNK_SIZE - 1) return CUBETYPE_AIR;
		return data->block_types[x][y][z + 1];

	case FACEIDX_RIGHT:
		if (x >= CHUNK_SIZE - 1) return CUBETYPE_AIR;
		return data->block_types[x + 1][y][z];

	case FACEIDX_LEFT:
		if (x == 0) return CUBETYPE_AIR;
		return data->block_types[x - 1][y][z];

	case FACEIDX_BOTTOM:
		if (y == 0) return CUBETYPE_AIR;
		return data->block_types[x][y - 1][z];

	case FACEIDX_TOP:
		if (y == CHUNK_HEIGHT - 1) return CUBETYPE_AIR;
		return data->block_types[x][y + 1][z];

	};
}

static void _add_face_to_mesh(quad_desc_t *desc, 
					   uint32_t *i_cnt, uint32_t *v_cnt,
					   uint16_t *indices, vertex_t *vertices)
{
	vertex_t verts[4] = {
		face_vertices[desc->face_idx][0],
		face_vertices[desc->face_idx][1],
		face_vertices[desc->face_idx][2],
		face_vertices[desc->face_idx][3]
	};
	for (size_t i = 0; i < 4; i++)
	{
		float *uv = _unpack(verts[i].uv);
		uv[0] += uv_lookup[(desc->cube_type * 6) + desc->face_idx].x;
		uv[1] += uv_lookup[(desc->cube_type * 6) + desc->face_idx].y;

		verts[i].uv = PACK(uv[0], uv[1]);
		verts[i].x += desc->x;
		verts[i].y += desc->y;
		verts[i].z += desc->z;
	}

	const uint32_t v_ofst = *v_cnt;
	for (size_t i = 0; i < 4; i++) 
		vertices[v_ofst + i] = verts[i];
	*v_cnt += 4;

	/* 
	 * Verts are ordered counter-clockwise in the buffer, so this index 
	 * ordering has clockwise winding.
	 */
	const uint32_t i_ofst = *i_cnt;
	indices[i_ofst + 0] = v_ofst + 0;
	indices[i_ofst + 1] = v_ofst + 2;
	indices[i_ofst + 2] = v_ofst + 1;
	indices[i_ofst + 3] = v_ofst + 0;
	indices[i_ofst + 4] = v_ofst + 3;
	indices[i_ofst + 5] = v_ofst + 2;
	*i_cnt += 6;
}

static mesh_t _make_submesh(mesh_desc_t *desc)
{
	return (mesh_t) {
		.v_buf = sg_make_buffer(&(sg_buffer_desc) {
			.usage = {
				.vertex_buffer = true,
				.immutable = true 
			},
			.data = (sg_range) {
				.ptr = desc->vertices,
				.size = (size_t) (desc->v_cnt * sizeof(vertex_t))
			},
			.label = "submesh-vertices"
		}),

		.i_buf = sg_make_buffer(&(sg_buffer_desc) {
			.usage = {
				.index_buffer = true,
				.immutable = true 
			},
			.data = (sg_range) {
				.ptr = desc->indices,
				.size = (size_t) (desc->i_cnt * sizeof(uint16_t))
			},
			.label = "submesh-indices"
		}),

		.v_cnt = desc->v_cnt,
		.i_cnt = desc->i_cnt
	};
}

static void _push_submesh(const mesh_t *submesh, chunk_t *chunk)
{
	const size_t new_size = (chunk->mesh.submesh_cnt + 1) * sizeof(mesh_t);
	chunk->mesh.submeshes = realloc(chunk->mesh.submeshes, new_size);
	chunk->mesh.submeshes[chunk->mesh.submesh_cnt] = *submesh;
	chunk->mesh.submesh_cnt++;
}

bool is_transparent(cube_type_e type)
{
	switch (type) {
		case CUBETYPE_AIR: return true;
		case CUBETYPE_LEAF: return true;
		default: return false;
	};
}

void chunk_generate_mesh(chunk_t *chunk)
{
	uint32_t v_cnt = 0;
	uint32_t i_cnt = 0;
	vertex_t *v_buf = malloc(SUBMESH_VERTEX_COUNT * sizeof(vertex_t));
	uint16_t *i_buf = malloc(SUBMESH_INDEX_COUNT * sizeof(uint16_t));

	/* 
	 * Iterating over 3D array by treating it as 1D in row-major-order, this 
	 * avoids triple nesting for loops. XYZ coords can be calculated from index.
	 */
	int8_t *ptr = (int8_t *) chunk->data->block_types;
	for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++, ptr++)
	{
		const cube_type_e type = *ptr;
		if (type == CUBETYPE_AIR) continue; /* Ignore air blocks */

		/* 
		 * Up to 24 vertices can be pushed to the buffers on each iteration 
		 * (4 vertices * 6 cube faces). If this exceeds the maximum for the 
		 * submesh buffer, then the buffers are handed to the submesh and 
		 * new ones are allocated to continue chunk construction.
		 */
		if (v_cnt + 24 >= SUBMESH_VERTEX_COUNT)
		{
			/* Pass existing buffers to the submesh. */
			const mesh_t submesh = _make_submesh(&(mesh_desc_t) {
				.v_cnt = v_cnt,
				.i_cnt = i_cnt,
				.vertices = v_buf,
				.indices = i_buf
			});
			_push_submesh(&submesh, chunk);

			/* Allocate new buffers to continue construction. */
			v_cnt = 0;
			i_cnt = 0;

			free(v_buf);
			free(i_buf);

			v_buf = malloc(SUBMESH_VERTEX_COUNT * sizeof(vertex_t));
			i_buf = malloc(SUBMESH_INDEX_COUNT * sizeof(uint16_t));
		}

		/* 
		 * If a non-air block in the chunk is neighboured by a transparent block 
		 * such as air, or a block with transparency in the texture such as leaves,
		 * then a quad is added to the chunk's mesh.
		 *
		 * The edges of the chunk are automatically meshed to avoid looking up 
		 * neighbouring chunks.
		 */
		const uint8_t x = i / (CHUNK_HEIGHT * CHUNK_SIZE);
		const uint8_t y = (i / CHUNK_SIZE) % CHUNK_HEIGHT;
		const uint8_t z = i % CHUNK_SIZE;

		for (uint8_t face = 0; face < 6; face++)
		{
			const cube_type_e nhbr = _get_neighbour(x, y, z, face, chunk->data);
			if (is_transparent(nhbr))
			{
				_add_face_to_mesh(&(quad_desc_t) {
					.x = x,
					.y = y,
					.z = z,
					.cube_type = type,
					.face_idx = face
				}, &i_cnt, &v_cnt, i_buf, v_buf);
			}
		}
	}

	/* Final buffers are not guaranteed to be full. Partial buffers with the 
	 * remainder of the mesh information for the chunk are pushed here.
	 */
	if (v_cnt > 0)
	{
		const mesh_t submesh = _make_submesh(&(mesh_desc_t) {
			.v_cnt = v_cnt,
			.i_cnt = i_cnt,
			.vertices = v_buf,
			.indices = i_buf
		});
		_push_submesh(&submesh, chunk);
	}

	/*
	 * chunk.pos is an em_vec3 so the components are float, chunk.mesh.(x, y, z) 
	 * are uint8_t to match the packed uniform data passed to the shader.
	 */
	chunk->mesh.x = chunk->pos.x;
	chunk->mesh.y = chunk->pos.y;
	chunk->mesh.z = chunk->pos.z;

	free(v_buf);
	free(i_buf);
}
