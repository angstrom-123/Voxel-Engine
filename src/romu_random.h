#ifndef ROMU_RANDOM_H
#define ROMU_RANDOM_H

#define ROTL(x, n) ((x << (n)) | (x >> (8 * sizeof(x) - (n))))

#define MONO32_MAX UINT16_MAX

#include <stdint.h>

extern void romu_mono32_init(uint32_t seed);
extern uint16_t romu_mono32_random();

#endif
