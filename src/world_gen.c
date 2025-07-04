#import "world_gen.h"
#include "src/cube.h"

static bool _valid_instance_count(uint16_t count)
{
	if (count >= MAX_INSTANCES)
	{
		fprintf(stderr, "Maximum instances reached\n");
		return false;
	}
	return true;
}

void gen_instantiate_cube(state_t *state, em_vec3 pos, cube_type_e type)
{
	if (!_valid_instance_count(state->instance_count + 1)) return;

	size_t new_size = (size_t) (sizeof(cube_instance_t) * (state->instance_count + 1));
	cube_instance_t instance = {.pos = pos, .type = type};

	if (cube_is_transparent(type))
	{
		state->deferred = realloc(state->deferred, new_size);
		state->deferred[state->deferred_count++] = instance;
	} 
	else 
	{
		state->instances = realloc(state->instances, new_size);
		state->instances[state->instance_count++] = instance;
	}
}

void gen_instantiate_chunk(state_t *state, em_vec3 pos)
{
	if (!_valid_instance_count(state->instance_count + 256)) return;

	size_t new_size = (size_t) (sizeof(cube_instance_t) * (state->instance_count + 256));
	state->instances = realloc(state->instances, new_size);

	for (size_t i = 0; i < 16; i++)
	{
		for (size_t j = 0; j < 16; j++)
		{
			em_vec3 inst_pos = {
				pos.x + (float) i,
				pos.y,
				pos.z + (float) j
			};

			state->instances[state->instance_count++] = (cube_instance_t) {
				.pos = inst_pos,
				.type = CUBETYPE_GRASS
			};
		}
	}
}
