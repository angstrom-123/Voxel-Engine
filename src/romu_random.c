#include "romu_random.h"

static uint32_t state_mono32;

void romu_mono32_init(uint32_t seed) 
{
	// 29 seed bits used
	state_mono32 = (seed & 0x1fffffffu) + 1156979152u; 
}

uint16_t romu_mono32_random()
{
	uint16_t res = state_mono32 >> 16;
	state_mono32 *= 3611795771u;
	state_mono32 = ROTL(state_mono32, 12);
	return res;
}
