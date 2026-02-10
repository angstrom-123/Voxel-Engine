#include "geometry.h"

static cube_type_e _get_adj_neighbour(uint8_t x, uint8_t y, uint8_t z,
                                      cube_face_idx_e face, chunk_set_t cs)
{
    switch (face) {
    case FACEIDX_BACK:
        if (z == 0) return cs.sd ? cs.sd->types[x][y][CHUNK_SIZE - 1] : CUBETYPE_NUM;
        return cs.cd->types[x][y][z - 1];

    case FACEIDX_FRONT:
        if (z == CHUNK_SIZE - 1) return cs.nd ? cs.nd->types[x][y][0] : CUBETYPE_NUM;
        return cs.cd->types[x][y][z + 1];

    case FACEIDX_RIGHT:
        if (x >= CHUNK_SIZE - 1) return cs.ed ? cs.ed->types[0][y][z] : CUBETYPE_NUM;
        return cs.cd->types[x + 1][y][z];

    case FACEIDX_LEFT:
        if (x == 0) return cs.wd ? cs.wd->types[CHUNK_SIZE - 1][y][z] : CUBETYPE_NUM;
        return cs.cd->types[x - 1][y][z];

    case FACEIDX_BOTTOM:
        if (y == 0) return CUBETYPE_NUM;
        return cs.cd->types[x][y - 1][z];

    case FACEIDX_TOP:
        if (y == CHUNK_HEIGHT - 1) return CUBETYPE_AIR;
        return cs.cd->types[x][y + 1][z];

    };
}

bool geom_cube_is_transparent(cube_type_e type)
{
    switch (type) {
    case CUBETYPE_AIR: 
        return true;
    default: 
        return false;
    };
}

static inline packed_vertex_t _packed_vertex(uint8_t vert_idx, const quad_desc_t *desc)
{
    uint8_t x = (face_offsets[desc->face][vert_idx].x + desc->x) << 3;
    uint8_t y = (face_offsets[desc->face][vert_idx].y + desc->y);
    uint8_t z = (face_offsets[desc->face][vert_idx].z + desc->z) << 3;

    uint8_t tex = tex_lookup[desc->type][desc->face];
    uint8_t u = (tex % (CHUNK_SIZE - 1)) << 4;
    uint8_t v = (CHUNK_SIZE - 1 - (uint8_t) floorf((float) tex / (CHUNK_SIZE - 1))) & 0x0F;

    uint8_t o = 0;
    if (vert_idx == 1 || vert_idx == 2) o |= (1 << 2);
    if (vert_idx == 2 || vert_idx == 3) o |= (1 << 1);

    uint8_t n = 0;
    switch (desc->face) {
    case FACEIDX_RIGHT : n = 1; break;
    case FACEIDX_LEFT  : n = 2; break;
    case FACEIDX_TOP   : n = 3; break;
    case FACEIDX_BOTTOM: n = 4; break;
    case FACEIDX_FRONT : n = 5; break;
    case FACEIDX_BACK  : n = 6; break;
    }

    return (packed_vertex_t) {
        .xo = x | o,
        .zn = z | n,
        .y = y,
        .t = u | v
    };
}

static void _pack_face_to_mesh(uint16_t *v_cnt, uint16_t *i_cnt, 
                               packed_vertex_t *vbuf, uint16_t *ibuf,
                               const quad_desc_t *desc)
{
    packed_vertex_t quad_verts[4] = {
        _packed_vertex(0, desc),
        _packed_vertex(1, desc),
        _packed_vertex(2, desc),
        _packed_vertex(3, desc)
    };

    const uint16_t v_ofst = *v_cnt;
    for (size_t i = 0; i < 4; i++) 
        vbuf[v_ofst + i] = quad_verts[i];

    *v_cnt += 4;

    const uint16_t i_ofst = *i_cnt;
    ibuf[i_ofst + 0] = v_ofst + 0;
    ibuf[i_ofst + 1] = v_ofst + 2;
    ibuf[i_ofst + 2] = v_ofst + 1;
    ibuf[i_ofst + 3] = v_ofst + 0;
    ibuf[i_ofst + 4] = v_ofst + 3;
    ibuf[i_ofst + 5] = v_ofst + 2;

    *i_cnt += 6;
}

void geom_generate_mesh(mesh_t **res_o, chunk_set_t cs)
{
    mesh_t *opaque = malloc(sizeof(mesh_t));
    uint16_t v_max_o = V_MAX / 8;
    uint16_t i_max_o = I_MAX / 8;
    uint16_t v_cnt_o = 0;
    uint16_t i_cnt_o = 0;
    packed_vertex_t *v_buf_o = malloc(v_max_o * sizeof(packed_vertex_t));
    uint16_t *i_buf_o = malloc(i_max_o * sizeof(uint16_t));

    ENGINE_ASSERT(opaque != NULL, "Failed to allocate opaque mesh.\n");
    ENGINE_ASSERT(v_buf_o != NULL, "Failed to allocate opaque vertex buffer.\n");
    ENGINE_ASSERT(i_buf_o != NULL, "Failed to allocate opaque vertex buffer.\n");

    /* Iterating x, y, z, as one dimension in row-major order for less nesting. */
    uint8_t *ptr = &cs.cd->types[0][0][0];
    for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++)
    {
        const cube_type_e type = *ptr;
        ptr++;

        if (type == CUBETYPE_AIR) 
            continue; // Ignore empty space.

        const uint8_t x = i / (CHUNK_HEIGHT * CHUNK_SIZE);
        const uint8_t y = (i / CHUNK_SIZE) % CHUNK_HEIGHT;
        const uint8_t z = i % CHUNK_SIZE;

        for (uint8_t face = 0; face < 6; face++)
        {
            const cube_type_e nhbr = _get_adj_neighbour(x, y, z, face, cs);
            if (!geom_cube_is_transparent(nhbr)) continue;

            if (v_cnt_o + 4 >= v_max_o)
            {
                v_buf_o = realloc(v_buf_o, (v_max_o *= 2) * sizeof(packed_vertex_t));
                i_buf_o = realloc(i_buf_o, (i_max_o *= 2) * sizeof(uint16_t));
            }
            _pack_face_to_mesh(&v_cnt_o, &i_cnt_o, v_buf_o, i_buf_o, &(quad_desc_t) {
                .x = x,
                .y = y,
                .z = z,
                .face = face,
                .type = type
            });
        }
    }

    // Only save to result pointers if there is some mesh.
    if (i_cnt_o > 0)
    {
        mesh_t mo = {
            .i_cnt = i_cnt_o,
            .v_cnt = v_cnt_o,
            .i_buf = i_buf_o,
            .v_buf = v_buf_o
        };
        memcpy(opaque, &mo, sizeof(mo));
        *res_o = opaque;
    }
}
