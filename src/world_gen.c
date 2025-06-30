#import "world_gen.h"
#include "src/mesh.h"

void gen_add_cube_instance(state_t *state, em_vec3 pos)
{
	if (state->instance_count >= MAX_INSTANCES)
	{
		fprintf(stderr, "Maximum instances reached\n");
		return;
	}

	state->instances[state->instance_count++] = (cube_instance_t) {pos};
}
