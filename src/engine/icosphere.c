#include "icosphere.h"

icosphere_t make_icosphere(void)
{
    icosphere_t ico;
    ico.v_cnt = ICOSPHERE_VERT_CNT;
    ico.i_cnt = ICOSPHERE_INDEX_CNT;

    ico.v_buf = malloc(ico.v_cnt * sizeof(ico_vertex_t));
    ico.i_buf = malloc(ico.i_cnt * sizeof(uint16_t));

    memcpy(ico.v_buf, ico_verts, sizeof(ico_verts));
    memcpy(ico.i_buf, ico_indices, sizeof(ico_indices));

    return ico;
}
