#include "geometry.h"

static cube_type_e _get_adj_neighbour(uint8_t x, uint8_t y, uint8_t z,
                                      cube_face_idx_e face, chunk_data_t *blocks,
                                      chunk_data_t *n, chunk_data_t *e, 
                                      chunk_data_t *s, chunk_data_t *w)
{
    switch (face) {
    case FACEIDX_BACK:
        if (z == 0) return s ? s->types[x][y][CHUNK_SIZE - 1] : CUBETYPE_NUM;
        return blocks->types[x][y][z - 1];

    case FACEIDX_FRONT:
        if (z == CHUNK_SIZE - 1) return n ? n->types[x][y][0] : CUBETYPE_NUM;
        return blocks->types[x][y][z + 1];

    case FACEIDX_RIGHT:
        if (x >= CHUNK_SIZE - 1) return e ? e->types[0][y][z] : CUBETYPE_NUM;
        return blocks->types[x + 1][y][z];

    case FACEIDX_LEFT:
        if (x == 0) return w ? w->types[CHUNK_SIZE - 1][y][z] : CUBETYPE_NUM;
        return blocks->types[x - 1][y][z];

    case FACEIDX_BOTTOM:
        if (y == 0) return CUBETYPE_AIR;
        return blocks->types[x][y - 1][z];

    case FACEIDX_TOP:
        if (y == CHUNK_HEIGHT - 1) return CUBETYPE_AIR;
        return blocks->types[x][y + 1][z];

    };
}

bool geom_cube_is_transparent(cube_type_e type)
{
    switch (type) {
    case CUBETYPE_AIR: 
    case CUBETYPE_LEAF: 
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
    if (desc->face == FACEIDX_BACK) n |= (1 << 2);
    if (desc->face == FACEIDX_BOTTOM) n |= (1 << 1);
    if (desc->face == FACEIDX_LEFT) n |= 1;

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


mesh_t *geom_generate_mesh(chunk_data_t *cb, 
                                  chunk_data_t *nb, chunk_data_t *eb,
                                  chunk_data_t *sb, chunk_data_t *wb)
{
    mesh_t *res = malloc(sizeof(mesh_t));
    
    uint16_t v_cnt = 0;
    uint16_t i_cnt = 0;

    packed_vertex_t *v_buf = malloc(V_MAX * sizeof(packed_vertex_t));
    uint16_t *i_buf = malloc(I_MAX * sizeof(uint16_t));

    ENGINE_ASSERT(res != NULL, "Failed to allocate mesh.\n");
    ENGINE_ASSERT(v_buf != NULL, "Failed to allocate vertex buffer.\n");
    ENGINE_ASSERT(i_buf != NULL, "Failed to allocate vertex buffer.\n");

    /* Iterating x, y, z, as one dimension in row-major order for less nesting. */
    uint8_t *ptr = &cb->types[0][0][0];
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
            const cube_type_e nhbr = _get_adj_neighbour(x, y, z, face, cb,
                                                        nb, eb, sb, wb);
            if (!geom_cube_is_transparent(nhbr)) 
                continue; // Opaque neighbour on this side, block face not visible.

            _pack_face_to_mesh(&v_cnt, &i_cnt, v_buf, i_buf, &(quad_desc_t) {
                .x = x,
                .y = y,
                .z = z,
                .face = face,
                .type = type
            });
        }
    }

    res->i_cnt = i_cnt;
    res->v_cnt = v_cnt;
    res->i_buf = i_buf;
    res->v_buf = v_buf;

    return res;
}
