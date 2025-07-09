#include "geometry.h"

static float* _unpack(uint32_t packed)
{
	const float a = ((float) (packed >> 16)) / 65535.0;
	const float b = ((float) (packed & 0xFFFF)) / 65535.0;

	return (float[]) {a, b};
}

static cube_type_e _get_neighbour(uint8_t x, uint8_t y, uint8_t z,
						   		  cube_face_idx_e face, chunk_data_t *blocks)
{
	switch (face) {
	case FACEIDX_BACK:
		if (z == 0) return CUBETYPE_AIR;
		return blocks->types[x][y][z - 1];

	case FACEIDX_FRONT:
		if (z == CHUNK_SIZE - 1) return CUBETYPE_AIR;
		return blocks->types[x][y][z + 1];

	case FACEIDX_RIGHT:
		if (x >= CHUNK_SIZE - 1) return CUBETYPE_AIR;
		return blocks->types[x + 1][y][z];

	case FACEIDX_LEFT:
		if (x == 0) return CUBETYPE_AIR;
		return blocks->types[x - 1][y][z];

	case FACEIDX_BOTTOM:
		if (y == 0) return CUBETYPE_AIR;
		return blocks->types[x][y - 1][z];

	case FACEIDX_TOP:
		if (y == CHUNK_HEIGHT - 1) return CUBETYPE_AIR;
		return blocks->types[x][y + 1][z];

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
	uint32_t max_v = 10240;
	uint32_t max_i = (max_v / 4) * 6;

	uint32_t v_cnt = 0;
	uint32_t i_cnt = 0;
	vertex_t *v_buf = malloc(max_v * sizeof(vertex_t));
	uint16_t *i_buf = malloc(max_i * sizeof(uint16_t));

	/* 
	 * Iterating over 3D array by treating it as 1D in row-major-order, this 
	 * avoids triple nesting for loops. XYZ coords can be calculated from index.
	 */
	int8_t *ptr = (int8_t *) chunk->blocks->types;
	for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++, ptr++)
	{
		const cube_type_e type = *ptr;
		if (type == CUBETYPE_AIR) continue; /* Ignore air blocks */

		/* A maximum of 24 verts can be added in each pass. */
		if (v_cnt + 24 >= max_v)
		{
			max_v += max_v / 2;
			max_i = (max_v / 4) * 6;

			v_buf = realloc(v_buf, max_v * sizeof(vertex_t));
			i_buf = realloc(i_buf, max_i * sizeof(uint16_t));
		}

		const uint8_t x = i / (CHUNK_HEIGHT * CHUNK_SIZE);
		const uint8_t y = (i / CHUNK_SIZE) % CHUNK_HEIGHT;
		const uint8_t z = i % CHUNK_SIZE;

		/* 
		 * Only block faces that are next to transparency (such as air or some 
		 * blocks) are meshed. Chunk borders are automatically meshed to avoid 
		 * looking up adjacent chunks.
		 */
		for (uint8_t face = 0; face < 6; face++)
		{
			const cube_type_e nhbr = _get_neighbour(x, y, z, face, chunk->blocks);
			if (!is_transparent(nhbr)) continue;

			_add_face_to_mesh(&(quad_desc_t) {
				.x = x,
				.y = y,
				.z = z,
				.cube_type = type,
				.face_idx = face
			}, &i_cnt, &v_cnt, i_buf, v_buf);
		}
	}

	/* Ignore empty chunks. */
	if (v_cnt == 0)
		chunk->mesh = (mesh_t) {
			.x = 0,
			.y = 0,
			.z = 0,
			.v_cnt = 0,
			.i_cnt = 0,
			.v_buf = NULL,
			.i_buf = NULL
		};
	else 
		chunk->mesh = (mesh_t) {
			.x = chunk->pos.x,
			.y = chunk->pos.y,
			.z = chunk->pos.z,
			.v_cnt = v_cnt,
			.i_cnt = i_cnt,
			.v_buf = v_buf,
			.i_buf = i_buf
		};
}
